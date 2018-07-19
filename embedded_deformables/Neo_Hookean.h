//!#####################################################################
//! \file Neo_Hookean.h
//!#####################################################################
// Class Neo_Hookean
//######################################################################
#ifndef __Neo_Hookean__
#define __Neo_Hookean__

#include "Force.h"
#include "Position_Based_State_Neo_Hookean.h"
#include "Rotated_Stress_Derivative.h"

namespace Nova{
template<class T,int d>
class Neo_Hookean: public Force<T,d>
{
    using TV                        = Vector<T,d>;
    using Base                      = Force<T,d>;
    using PBS_Neo_Hookean           = Position_Based_State_Neo_Hookean<T,d>;

  public:
    using Base::youngs_modulus;using Base::poissons_ratio;using Base::mu;using Base::lambda;
    using Base::Initialize_Lame_Parameters;

    const PBS_Neo_Hookean *position_based_state;

    Neo_Hookean(const PBS_Neo_Hookean *position_based_state_input)
        :Base(),position_based_state(position_based_state_input)
    {
        Initialize_Lame_Parameters();
    }

    static Rotated_Stress_Derivative<T,2> Stress_Derivative(const Diagonal_Matrix<T,2>& Sigma,const T mu,const T lambda)
    {
        T J=Sigma.Determinant();
        T alpha00=mu,alpha01=mu,alpha11=mu;

        T beta_const=(mu-lambda*(T)log(J));
        T beta00=(1/(Sigma(0,0)*Sigma(0,0)))*beta_const;
        T beta01=(1/(Sigma(0,0)*Sigma(1,1)))*beta_const;
        T beta11=(1/(Sigma(1,1)*Sigma(1,1)))*beta_const;

        T gamma00=lambda/(Sigma(0,0)*Sigma(0,0));
        T gamma01=lambda/(Sigma(0,0)*Sigma(1,1));
        T gamma11=lambda/(Sigma(1,1)*Sigma(1,1));

        Rotated_Stress_Derivative<T,2> rotated_dPdF;

        rotated_dPdF.a0000=alpha00+beta00+gamma00;
        rotated_dPdF.a0011=gamma01;
        rotated_dPdF.a1111=alpha11+beta11+gamma11;

        rotated_dPdF.a0101=alpha01;
        rotated_dPdF.a0110=beta01;

        return rotated_dPdF;
    }

    static Rotated_Stress_Derivative<T,3> Stress_Derivative(const Diagonal_Matrix<T,3>& Sigma,const T mu,const T lambda)
    {
        T J=Sigma.Determinant();

        T alpha00=mu,alpha01=mu,alpha02=mu;
        T alpha11=mu,alpha12=mu,alpha22=mu;

        T beta_const=(mu-lambda*(T)log(J));
        T beta00=beta_const/(Sigma(0,0)*Sigma(0,0));
        T beta01=beta_const/(Sigma(0,0)*Sigma(1,1));
        T beta02=beta_const/(Sigma(0,0)*Sigma(2,2));
        T beta11=beta_const/(Sigma(1,1)*Sigma(1,1));
        T beta12=beta_const/(Sigma(1,1)*Sigma(2,2));
        T beta22=beta_const/(Sigma(2,2)*Sigma(2,2));

        T gamma00=lambda/(Sigma(0,0)*Sigma(0,0));
        T gamma01=lambda/(Sigma(0,0)*Sigma(1,1));
        T gamma02=lambda/(Sigma(0,0)*Sigma(2,2));
        T gamma11=lambda/(Sigma(1,1)*Sigma(1,1));
        T gamma12=lambda/(Sigma(1,1)*Sigma(2,2));
        T gamma22=lambda/(Sigma(2,2)*Sigma(2,2));

        Rotated_Stress_Derivative<T,3> rotated_dPdF;

        rotated_dPdF.a0000=alpha00+beta00+gamma00;
        rotated_dPdF.a0011=gamma01;
        rotated_dPdF.a0022=gamma02;
        rotated_dPdF.a1111=alpha11+beta11+gamma11;
        rotated_dPdF.a1122=gamma12;
        rotated_dPdF.a2222=alpha22+beta22+gamma22;

        rotated_dPdF.a0101=alpha01;
        rotated_dPdF.a0110=beta01;
        rotated_dPdF.a0202=alpha02;
        rotated_dPdF.a0220=beta02;
        rotated_dPdF.a1212=alpha12;
        rotated_dPdF.a1221=beta12;

        return rotated_dPdF;
    }

    virtual void Add_Elastic_Forces(const Array<TV>& X,Array<TV>& force) override
    {
        for(size_t i=0;i<position_based_state->simulation_mesh->elements.size();++i){
            const Matrix<T,d,d>& U=position_based_state->U(i);
            const Matrix<T,d,d>& V=position_based_state->V(i);
            const Diagonal_Matrix<T,d>& Sigma=position_based_state->Sigma(i);
            T J=Sigma.Determinant();
            Diagonal_Matrix<T,d> P_hat=mu*Sigma+(lambda*(T)log(J)-mu)*Sigma.Inverse();
            Matrix<T,d,d> P=U*P_hat*V.Transposed();
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
            const Matrix<T,d,d>& U=position_based_state->U(i);
            const Matrix<T,d,d>& V=position_based_state->V(i);
            const Diagonal_Matrix<T,d>& Sigma=position_based_state->Sigma(i);
            Matrix<T,d,d> dDs=position_based_state->Get_Shape_Matrix(DX,i);
            Matrix<T,d,d> dF=dDs*position_based_state->Dm_inverse(i);
            Rotated_Stress_Derivative<T,d> rotated_dPdF=Stress_Derivative(Sigma,mu,lambda);
            rotated_dPdF.Make_Positive_Definite();
            Matrix<T,d,d> dF_hat=U.Transposed()*dF*V;
            Matrix<T,d,d> dP_hat=rotated_dPdF.dP_hat(dF_hat);
            Matrix<T,d,d> dP=U*dP_hat*V.Transposed();
            Matrix<T,d,d> dG=-dP*position_based_state->Dm_inverse(i).Transposed()*position_based_state->element_rest_volume(i);
            position_based_state->Distribute_To_Simulation_Particles(force,dG,i);}
    }
};
}
#endif
