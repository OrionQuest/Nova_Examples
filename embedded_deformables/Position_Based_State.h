//!#####################################################################
//! \file Position_Based_State.h
//!#####################################################################
// Class Position_Based_State
//######################################################################
#ifndef __Position_Based_State__
#define __Position_Based_State__

#include <nova/Geometry/Topology_Based_Geometry/Tetrahedralized_Volume.h>
#include <nova/Tools/Matrices/Matrix.h>
#include <nova/Tools/Utilities/Constants.h>

namespace Nova{
template<class T,int d>
class Position_Based_State
{
    using TV                        = Vector<T,d>;
    using INDEX                     = Vector<int,d+1>;
    using T_Volume                  = Tetrahedralized_Volume<T,d>;

  public:
    const T_Volume* simulation_mesh;
    Array<Matrix<T,d,d>> Dm_inverse;
    Array<T> element_rest_volume;

    Position_Based_State(const T_Volume* simulation_mesh_input)
        :simulation_mesh(simulation_mesh_input)
    {
        Dm_inverse.resize(simulation_mesh->elements.size());
        element_rest_volume.resize(simulation_mesh->elements.size());
        for(size_t i=0;i<simulation_mesh->elements.size();++i){
            Matrix<T,d,d> Dm=Get_Shape_Matrix(simulation_mesh->points,i);
            Dm_inverse(i)=Dm.Inverse();
            element_rest_volume(i)=(d==2)?fabs(Dm.Determinant())*(T).5:fabs(Dm.Determinant())*(T)one_sixth;}
    }

    virtual ~Position_Based_State() {}

    Matrix<T,d,d> Get_Shape_Matrix(const Array<TV>& X,const int index) const
    {
        Vector<TV,d> columns;
        const INDEX& e=simulation_mesh->elements(index);
        for(int i=1;i<d+1;++i) columns(i-1)=X(e(i))-X(e(0));
        return Matrix<T,d,d>(columns);
    }

    void Distribute_To_Simulation_Particles(Array<TV>& force,const Matrix<T,d,d>& G,const int index) const
    {
        const INDEX& e=simulation_mesh->elements(index);TV sum;
        for(int i=1;i<d+1;++i){sum+=G.Column(i-1);
            force(e(i))+=G.Column(i-1);}
        force(e(0))-=sum;
    }

//######################################################################
    virtual void Initialize() {}
    virtual void Update_Position_Based_State(const Array<TV>& X)=0;
//######################################################################
};
}
#endif
