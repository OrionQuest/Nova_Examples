//!#####################################################################
//! \file Linear_Elasticity.h
//!#####################################################################
// Class Linear_Elasticity
//######################################################################
#ifndef __Linear_Elasticity__
#define __Linear_Elasticity__

#include "Force.h"
#include "Position_Based_State_Linear_Elasticity.h"

namespace Nova{
template<class T,int d>
class Linear_Elasticity: public Force<T,d>
{
    using TV                        = Vector<T,d>;
    using Base                      = Force<T,d>;
    using PBS_Linear_Elasticity     = Position_Based_State_Linear_Elasticity<T,d>;

  public:
    using Base::mu;using Base::lambda;
    using Base::Initialize_Lame_Parameters;

    const PBS_Linear_Elasticity *position_based_state;

    Linear_Elasticity(const PBS_Linear_Elasticity *position_based_state_input)
        :Base(),position_based_state(position_based_state_input)
    {
        Initialize_Lame_Parameters();
    }

    virtual void Add_Elastic_Forces(const Array<TV>& X,Array<TV>& force) override
    {
        for(size_t i=0;i<position_based_state->simulation_mesh->elements.size();++i){
            Matrix<T,d,d> Ds=position_based_state->Get_Shape_Matrix(X,i);
            Matrix<T,d,d> F=Ds*position_based_state->Dm_inverse(i);
            Matrix<T,d,d> epsilon=(T).5*(F+F.Transposed())-(T)1.;
            Matrix<T,d,d> P=(T)2.*mu*epsilon+lambda*epsilon.Trace();
            Matrix<T,d,d> G=-P*position_based_state->Dm_inverse(i).Transposed()*position_based_state->element_rest_volume(i);
            position_based_state->Distribute_To_Simulation_Particles(force,G,i);}
    }

    virtual void Add_Damping_Forces(const Array<TV>& V,Array<TV>& force) override
    {
        Array<TV> force_tmp(force.size());
        Add_Force_Differentials(V,force_tmp);
        T gamma=(T).2;
        force+=force_tmp*gamma;
    }

    virtual void Add_Force_Differentials(const Array<TV>& DX,Array<TV>& force) override
    {
        for(size_t i=0;i<position_based_state->simulation_mesh->elements.size();++i){
            Matrix<T,d,d> dDs=position_based_state->Get_Shape_Matrix(DX,i);
            Matrix<T,d,d> dF=dDs*position_based_state->Dm_inverse(i);
            Matrix<T,d,d> depsilon=(T).5*(dF+dF.Transposed());
            Matrix<T,d,d> dP=(T)2.*mu*depsilon+lambda*depsilon.Trace();
            Matrix<T,d,d> dG=-dP*position_based_state->Dm_inverse(i).Transposed()*position_based_state->element_rest_volume(i);
            position_based_state->Distribute_To_Simulation_Particles(force,dG,i);}
    }
};
}
#endif
