//!#####################################################################
//! \file Embedded_Deformables_Example.h
//!#####################################################################
// Class Embedded_Deformables_Example
//######################################################################
#ifndef __Embedded_Deformables_Example__
#define __Embedded_Deformables_Example__

#include <nova/Tools/Utilities/Example.h>
#include "Force.h"
#include "Position_Based_State.h"

namespace Nova{
template<class T,int d>
class Embedded_Deformables_Example: public Example<T,d>
{
    using TV                        = Vector<T,d>;
    using Base                      = Example<T,d>;
    using T_INDEX                   = Vector<int,d>;
    using INDEX                     = Vector<int,d+1>;
    using T_Volume                  = Tetrahedralized_Volume<T,d>;
    using T_Surface                 = typename T_Volume::T_Surface;

  public:
    using Base::frame_title;using Base::output_directory;using Base::parse_args;using Base::first_frame;

    Array<int> embedding_map;
    Array<TV> embedding_weights,surface_velocities,volume_velocities;
    Array<T> mass;
    T_Volume *simulation_mesh;
    T_Surface *embedded_surface;
    Array<Force<T,d>*> forces;
    Position_Based_State<T,d> *position_based_state;

    Array<Array<int>> indices_of_fixed_particles;
    Array<Array<TV>> rest_position_of_fixed_particles;

    T_INDEX counts;
    int newton_iterations,cg_iterations,cg_restart_iterations;
    T cg_tolerance;

    Embedded_Deformables_Example()
        :Base(),simulation_mesh(new Tetrahedralized_Volume<T,d>()),embedded_surface(new T_Surface()),position_based_state(nullptr)
    {}

    ~Embedded_Deformables_Example()
    {
        if(simulation_mesh!=nullptr) delete simulation_mesh;
        if(embedded_surface!=nullptr) delete embedded_surface;
        if(position_based_state!=nullptr) delete position_based_state;
    }

    void Limit_Dt(T& dt,const T time) override
    {}

//######################################################################
    virtual void Initialize_Simulation_Mesh()=0;
    virtual void Initialize_Position_Based_State()=0;
    virtual void Initialize_Embedded_Mesh()=0;
    virtual void Initialize_Auxiliary_Structures()=0;
    virtual TV Get_Embedding_Weights(const int element,const TV& X)=0;
    virtual void Set_Kinematic_Positions(const T time,Array<TV>& X) const=0;
    virtual void Set_Kinematic_Velocities(const T time,Array<TV>& V) const=0;
    virtual void Clear_Values_Of_Kinematic_Particles(Array<TV>& array) const=0;
//######################################################################
    void Initialize();
    void Initialize_Embedding_Map_And_Weights();
    TV Interpolate(const Array<TV>& array,const TV& weights,const int element);
    void Distribute(Array<TV>& array,const TV& weights,const TV& value,const int element);
    void Add_External_Forces(Array<TV>& force);
    void Add_Elastic_Forces(const Array<TV>& X,Array<TV>& force);
    void Add_Damping_Forces(const Array<TV>& V,Array<TV>& force);
    void Add_Force_Differentials(const Array<TV>& DX,Array<TV>& force);
    void Interpolate_Embedded_Values(Array<TV>& array,const Array<TV>& embedding_array);
    void Distribute_To_Embedding_Parents(const Array<TV>& array,Array<TV>& embedding_array);
    void Register_Options();
    void Parse_Options();
    void Read_Output_Files(const int frame);
    void Write_Output_Files(const int frame) const override;
//######################################################################
};
}
#endif
