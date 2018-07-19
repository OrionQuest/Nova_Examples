//!#####################################################################
//! \file Standard_Tests.h
//!#####################################################################
// Class Standard_Tests
//######################################################################
#ifndef __Standard_Tests__
#define __Standard_Tests__

#include <nova/Geometry/Basic_Geometry/Sphere.h>
#include "../../Embedded_Deformables_Example.h"
#include "../../Linear_Elasticity.h"
#include "../../Neo_Hookean.h"

namespace Nova{
template<class T,int d>
class Standard_Tests: public Embedded_Deformables_Example<T,d>
{
    using TV                = Vector<T,d>;
    using T_INDEX           = Vector<int,d>;
    using Base              = Embedded_Deformables_Example<T,d>;

  public:
    using Base::output_directory;using Base::test_number;using Base::simulation_mesh;using Base::embedded_surface;using Base::position_based_state;using Base::mass;
    using Base::surface_velocities;using Base::volume_velocities;using Base::forces;using Base::indices_of_fixed_particles;using Base::rest_position_of_fixed_particles;
    using Base::counts;

    T edge_velocity;

    /****************************
     * example explanation:
     *
     * 1. Embedded sphere stretched horizontally from left and right with linear elasticity.
     * 2. Embedded circle stretched horizontally from left and right with neo hookean model.
     ****************************/

    Standard_Tests()
        :Base()
    {}

//######################################################################
    void Parse_Options()
    {
        Base::Parse_Options();
        output_directory="Test_"+std::to_string(test_number)+"_Resolution_"+std::to_string(counts(0));
    }
//######################################################################
    void Initialize_Simulation_Mesh() override
    {
        if(test_number==1 || test_number==2){simulation_mesh->Initialize_Cube_Mesh(Grid<T,d>(counts,Range<T,d>(TV(-.5),TV(.5))));
            indices_of_fixed_particles.resize(2);rest_position_of_fixed_particles.resize(2);
            for(size_t i=0;i<simulation_mesh->points.size();++i){const TV& X=simulation_mesh->points(i);
                if(X(0)==(T)-.5){indices_of_fixed_particles(0).Append(i);rest_position_of_fixed_particles(0).Append(X);}
                if(X(0)==(T).5){indices_of_fixed_particles(1).Append(i);rest_position_of_fixed_particles(1).Append(X);}}
            Log::cout<<"# of left particles: "<<indices_of_fixed_particles(0).size()<<std::endl;
            Log::cout<<"# of right particles: "<<indices_of_fixed_particles(1).size()<<std::endl;}

        volume_velocities.resize(simulation_mesh->points.size(),TV());
    }
//######################################################################
    void Initialize_Position_Based_State() override
    {
        if(test_number==1) position_based_state=new Position_Based_State_Linear_Elasticity<T,d>(simulation_mesh);
        else if(test_number==2) position_based_state=new Position_Based_State_Neo_Hookean<T,d>(simulation_mesh);
    }
//######################################################################
    void Initialize_Embedded_Mesh() override
    {
        if(test_number==1 || test_number==2){Sphere<T,d> sphere(TV(),(T).45);
            embedded_surface->Initialize_Sphere_Tessellation(sphere);}

        surface_velocities.resize(embedded_surface->points.size(),TV());
    }
//######################################################################
    void Initialize_Auxiliary_Structures() override
    {
        if(test_number==1) forces.Append(new Linear_Elasticity<T,d>(dynamic_cast<Position_Based_State_Linear_Elasticity<T,d>*>(position_based_state)));
        else if(test_number==2) forces.Append(new Neo_Hookean<T,d>(dynamic_cast<Position_Based_State_Neo_Hookean<T,d>*>(position_based_state)));
        position_based_state->Initialize();

        mass.resize(simulation_mesh->points.size(),(T).1);

        if(test_number==1 || test_number==2) edge_velocity=(T).5*.24;
    }
//######################################################################
    TV Get_Embedding_Weights(const int element,const TV& X) override
    {return simulation_mesh->Get_Element(element).First_Three_Barycentric_Coordinates(X);}
//######################################################################
    void Set_Kinematic_Positions(const T time,Array<TV>& X) const override
    {
        if(test_number==1 || test_number==2){
            for(size_t i=0;i<indices_of_fixed_particles(0).size();++i) X(indices_of_fixed_particles(0)(i))=rest_position_of_fixed_particles(0)(i)-TV({edge_velocity*time,0,0});
            for(size_t i=0;i<indices_of_fixed_particles(1).size();++i) X(indices_of_fixed_particles(1)(i))=rest_position_of_fixed_particles(1)(i)+TV({edge_velocity*time,0,0});}
    }
//######################################################################
    virtual void Set_Kinematic_Velocities(const T time,Array<TV>& V) const override
    {
        if(test_number==1 || test_number==2){
            for(size_t i=0;i<indices_of_fixed_particles(0).size();++i) V(indices_of_fixed_particles(0)(i))=TV({-edge_velocity,0,0});
            for(size_t i=0;i<indices_of_fixed_particles(1).size();++i) V(indices_of_fixed_particles(1)(i))=TV({edge_velocity,0,0});}
    }
//######################################################################
    virtual void Clear_Values_Of_Kinematic_Particles(Array<TV>& array) const override
    {
        if(test_number==1 || test_number==2){
            for(size_t i=0;i<indices_of_fixed_particles(0).size();++i) array(indices_of_fixed_particles(0)(i))=TV();
            for(size_t i=0;i<indices_of_fixed_particles(1).size();++i) array(indices_of_fixed_particles(1)(i))=TV();}
    }
//######################################################################
};
}
#endif
