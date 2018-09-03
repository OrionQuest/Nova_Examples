//!#####################################################################
//! \file Rotated_Stress_Derivative.cpp
//!#####################################################################
#include <nova/Tools/Matrices/Symmetric_Matrix_2x2.h>
#include <nova/Tools/Matrices/Symmetric_Matrix_3x3.h>
#include "Rotated_Stress_Derivative.h"
using namespace Nova;
//######################################################################
// dP_hat
//######################################################################
template<class T> Matrix<T,2> Rotated_Stress_Derivative<T,2>::
dP_hat(const Matrix<T,2>& dF_hat) const
{
    Matrix<T,2> dP_hat;
    dP_hat(0,0)=a0000*dF_hat(0,0)+a0011*dF_hat(1,1);
    dP_hat(1,1)=a0011*dF_hat(0,0)+a1111*dF_hat(1,1);
    dP_hat(0,1)=a0101*dF_hat(0,1)+a0110*dF_hat(1,0);
    dP_hat(1,0)=a0110*dF_hat(0,1)+a0101*dF_hat(1,0);

    return dP_hat;
}
//######################################################################
// Make_Positive_Definite
//######################################################################
template<class T> void Rotated_Stress_Derivative<T,2>::
Make_Positive_Definite()
{
    Symmetric_Matrix<T,2> A(a0000,a0011,a1111);
    A=A.Positive_Definite_Part();
    a0000=A(0,0);a0011=A(0,1);a1111=A(1,1);

    T lambda1=std::max(.5*(a0101+a0110),0.),lambda2=std::max(.5*(a0101-a0110),0.);
    a0101=lambda1+lambda2;a0110=lambda1-lambda2;
}
//######################################################################
// operator+=
//######################################################################
template<class T> Rotated_Stress_Derivative<T,2> Rotated_Stress_Derivative<T,2>::
operator+=(const Rotated_Stress_Derivative& input)
{
    a0000+=input.a0000;a0011+=input.a0011;a1111+=input.a1111;
    a0101+=input.a0101;a0110+=input.a0110;
    return *this;
}
//######################################################################
// operator-=
//######################################################################
template<class T> Rotated_Stress_Derivative<T,2> Rotated_Stress_Derivative<T,2>::
operator-=(const Rotated_Stress_Derivative& input)
{
    a0000-=input.a0000;a0011-=input.a0011;a1111-=input.a1111;
    a0101-=input.a0101;a0110-=input.a0110;
    return *this;
}
//######################################################################
// dP_hat
//######################################################################
template<class T> Matrix<T,3> Rotated_Stress_Derivative<T,3>::
dP_hat(const Matrix<T,3>& dF_hat) const
{
    Matrix<T,3> dP_hat;
    dP_hat(0,0)=a0000*dF_hat(0,0)+a0011*dF_hat(1,1)+a0022*dF_hat(2,2);
    dP_hat(1,1)=a0011*dF_hat(0,0)+a1111*dF_hat(1,1)+a1122*dF_hat(2,2);
    dP_hat(2,2)=a0022*dF_hat(0,0)+a1122*dF_hat(1,1)+a2222*dF_hat(2,2);
    dP_hat(0,1)=a0101*dF_hat(0,1)+a0110*dF_hat(1,0);
    dP_hat(1,0)=a0110*dF_hat(0,1)+a0101*dF_hat(1,0);
    dP_hat(0,2)=a0202*dF_hat(0,2)+a0220*dF_hat(2,0);
    dP_hat(2,0)=a0220*dF_hat(0,2)+a0202*dF_hat(2,0);
    dP_hat(1,2)=a1212*dF_hat(1,2)+a1221*dF_hat(2,1);
    dP_hat(2,1)=a1221*dF_hat(1,2)+a1212*dF_hat(2,1);

    return dP_hat;
}
//######################################################################
// Make_Positive_Definite
//######################################################################
template<class T> void Rotated_Stress_Derivative<T,3>::
Make_Positive_Definite()
{
    Symmetric_Matrix<T,3> A(a0000,a0011,a0022,a1111,a1122,a2222);
    A=A.Positive_Definite_Part();
    a0000=A(0,0);a0011=A(0,1);a0022=A(0,2);a1111=A(1,1);a1122=A(1,2);a2222=A(2,2);

    T lambda1=std::max(.5*(a0101+a0110),0.),lambda2=std::max(.5*(a0101-a0110),0.);
    a0101=lambda1+lambda2;a0110=lambda1-lambda2;

    lambda1=std::max(.5*(a0202+a0220),0.);lambda2=std::max(.5*(a0202-a0220),0.);
    a0202=lambda1+lambda2;a0220=lambda1-lambda2;

    lambda1=std::max(.5*(a1212+a1221),0.);lambda2=std::max(.5*(a1212-a1221),0.);
    a1212=lambda1+lambda2;a1221=lambda1-lambda2;
}
//######################################################################
// operator+=
//######################################################################
template<class T> Rotated_Stress_Derivative<T,3> Rotated_Stress_Derivative<T,3>::
operator+=(const Rotated_Stress_Derivative& input)
{
    a0000+=input.a0000;a0011+=input.a0011;a0022+=input.a0022;
    a1111+=input.a1111;a1122+=input.a1122;a2222+=input.a2222;
    a0101+=input.a0101;a0110+=input.a0110;
    a0202+=input.a0202;a0220+=input.a0220;
    a1212+=input.a1212;a1221+=input.a1221;
    return *this;
}
//######################################################################
// operator-=
//######################################################################
template<class T> Rotated_Stress_Derivative<T,3> Rotated_Stress_Derivative<T,3>::
operator-=(const Rotated_Stress_Derivative& input)
{
    a0000-=input.a0000;a0011-=input.a0011;a0022-=input.a0022;
    a1111-=input.a1111;a1122-=input.a1122;a2222-=input.a2222;
    a0101-=input.a0101;a0110-=input.a0110;
    a0202-=input.a0202;a0220-=input.a0220;
    a1212-=input.a1212;a1221-=input.a1221;
    return *this;
}
//######################################################################
template class Nova::Rotated_Stress_Derivative<float,2>;
template class Nova::Rotated_Stress_Derivative<float,3>;
#ifdef COMPILE_WITH_DOUBLE_SUPPORT
template class Nova::Rotated_Stress_Derivative<double,2>;
template class Nova::Rotated_Stress_Derivative<double,3>;
#endif
