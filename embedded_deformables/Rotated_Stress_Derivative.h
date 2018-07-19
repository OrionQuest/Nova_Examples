//!#####################################################################
//! \file Rotated_Stress_Derivative.h
//!#####################################################################
// Class Rotated_Stress_Derivative
//######################################################################
#ifndef __Rotated_Stress_Derivative__
#define __Rotated_Stress_Derivative__

#include <nova/Tools/Matrices/Matrix.h>

namespace Nova{

template<class T,int d> class Rotated_Stress_Derivative;

template<class T>
class Rotated_Stress_Derivative<T,2>
{
    using TV        = Vector<T,2>;

  public:
    T a0000,a0011,a1111;
    T a0101,a0110;

//######################################################################
    Matrix<T,2,2> dP_hat(const Matrix<T,2,2>& dF_hat) const;                    // contraction
    void Make_Positive_Definite();
    Rotated_Stress_Derivative operator+=(const Rotated_Stress_Derivative& input);
    Rotated_Stress_Derivative operator-=(const Rotated_Stress_Derivative& input);
//######################################################################
};

template<class T>
class Rotated_Stress_Derivative<T,3>
{
    using TV        = Vector<T,3>;

  public:
    T a0000,a0011,a0022,a1111,a1122,a2222;
    T a0101,a0110,a0202,a0220,a1212,a1221;

//######################################################################
    Matrix<T,3,3> dP_hat(const Matrix<T,3,3>& dF_hat) const;                    // contraction
    void Make_Positive_Definite();
    Rotated_Stress_Derivative operator+=(const Rotated_Stress_Derivative& input);
    Rotated_Stress_Derivative operator-=(const Rotated_Stress_Derivative& input);
//######################################################################
};
}
#endif
