//-------------------------------------------------------------
//    $Id: simulator.h 232 2011-10-19 13:30:15Z bangerth $
//
//    Copyright (C) 2011 by the authors of the ASPECT code
//
//-------------------------------------------------------------
#ifndef __aspect__geometry_model_base_h
#define __aspect__geometry_model_base_h

#include <deal.II/base/parameter_handler.h>
#include <deal.II/distributed/tria.h>


namespace aspect
{
  /**
   * A namespace for the definition of properties of the geometry. This
   * primarily includes the definition of the shape of the domain
   * (e.g. whether it is a full spherical shell, a quadrant/octant,
   * a description of the geoid, etc. The classes and functions of this
   * namespace also describe which kinds of boundary conditions hold on
   * the different parts of the boundary of the geometry.
   *
   * @ingroup GeometryModels
   */
  namespace GeometryModel
  {
    using namespace dealii;

    /**
     * Base class for classes that describe particular geometries for the
     * domain. These classes must also be able to create coarse meshes
     * and describe what kinds of boundary conditions hold where.
     *
     * @ingroup GeometryModels
     */
    template <int dim>
    class Interface
    {
      public:
        /**
         * Destructor. Made virtual to enforce that derived classes also have
         * virtual destructors.
         */
        virtual ~Interface();

        /**
         * Generate a coarse mesh for the geometry described by this class.
         */
        virtual
        void create_coarse_mesh (parallel::distributed::Triangulation<dim> &coarse_grid) const = 0;

        /**
         * Declare the parameters this class takes through input files.
         * The default implementation of this function does not describe
         * any parameters. Consequently, derived classes do not have to
         * overload this function if they do not take any runtime parameters.
         */
        static
        void
        declare_parameters (ParameterHandler &prm);

        /**
         * Read the parameters this class declares from the parameter
         * file. The default implementation of this function does not read
         * any parameters. Consequently, derived classes do not have to
         * overload this function if they do not take any runtime parameters.
         */
        virtual
        void
        parse_parameters (ParameterHandler &prm);
    };
    
    
    /**
     * Register a geometry model so that it can be selected from the parameter file.
     *
     * @param name A string that identifies the geometry model
     * @param declare_parameters_function A pointer to a function that can be used to
     *   declare the parameters that this geometry model wants to read from input files.
     * @param factory_function A pointer to a function that can create an object of
     *   this geometry model.
     *
     * @ingroup GeometryModels
     */
    template <int dim>
    void
    register_geometry_model (const std::string &name,
                             void (*declare_parameters_function) (ParameterHandler &),
                             Interface<dim> * (*factory_function) ());

    /**
     * A function that given the name of a model returns a pointer to an object
     * that describes it. Ownership of the pointer is transferred to the caller.
     *
     * @ingroup GeometryModels
     */
    template <int dim>
    Interface<dim> *
    create_geometry_model (ParameterHandler &prm);


    /**
     * Declare the runtime parameters of the registered geometry models.
     *
     * @ingroup GeometryModels
     */
    void
    declare_parameters (ParameterHandler &prm);

    namespace internal
    {
      /**
       * An internal class that is used in the definition of the
       * ASPECT_REGISTER_MATERIAL_MODEL macro below. Given a name
       * and a classname, it registers the geometry model.
       */
      template <const char **name, class GeometryModelClass>
      struct GeometryModelHelper
      {
        GeometryModelHelper ()
        {
          register_geometry_model
          (*name,
           &GeometryModelClass::declare_parameters,
           &factory);
        }

        static
        Interface<deal_II_dimension> * factory ()
        {
          return new GeometryModelClass();
        }
      };
    }


    /**
     * Given a name and a classname for a postprocessor, register it with
     * the aspect::Postprocess::Manager class.
     *
     * @ingroup GeometryModels
     */
#define ASPECT_REGISTER_GEOMETRY_MODEL(name,classname) \
  namespace ASPECT_REGISTER_MATERIAL_MODEL_ ## classname \
  { const char *local_name = name; \
    aspect::GeometryModel::internal::GeometryModelHelper<&local_name,classname<deal_II_dimension> > \
    dummy_ ## classname; }
  }
}


#endif
