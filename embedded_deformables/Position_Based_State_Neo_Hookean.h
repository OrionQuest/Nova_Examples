//!#####################################################################
//! \file Position_Based_State_Neo_Hookean.h
//!#####################################################################
// Class Position_Based_State_Neo_Hookean
//######################################################################
#ifndef __Position_Based_State_Neo_Hookean__
#define __Position_Based_State_Neo_Hookean__

#include <nova/Tools/Matrices/Diagonal_Matrix.h>
#include "Position_Based_State.h"

namespace Nova{
template<class T,int d>
class Position_Based_State_Neo_Hookean: public Position_Based_State<T,d>
{
    using TV                        = Vector<T,d>;
    using Base                      = Position_Based_State<T,d>;
    using T_Volume                  = Tetrahedralized_Volume<T,d>;

  public:
    using Base::simulation_mesh;using Base::Dm_inverse;
    using Base::Get_Shape_Matrix;

    Array<Matrix<T,d>> U,V;
    Array<Diagonal_Matrix<T,d>> Sigma;

    Position_Based_State_Neo_Hookean(const T_Volume* simulation_mesh)
        :Base(simulation_mesh)
    {}

    virtual void Initialize() override
    {
        U.resize(simulation_mesh->elements.size());
        V.resize(simulation_mesh->elements.size());
        Sigma.resize(simulation_mesh->elements.size());
    }

    virtual void Update_Position_Based_State(const Array<TV>& X) override
    {
        for(size_t i=0;i<simulation_mesh->elements.size();++i){
            Matrix<T,d> Ds=Get_Shape_Matrix(X,i);
            Matrix<T,d> F=Ds*Dm_inverse(i);
            F.Fast_Singular_Value_Decomposition(U(i),Sigma(i),V(i));
            Sigma(i)=Sigma(i).Clamp_Min((T).05);}
    }
};
}
#endif
