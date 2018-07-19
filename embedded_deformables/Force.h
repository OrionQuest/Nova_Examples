//!#####################################################################
//! \file Force.h
//!#####################################################################
// Class Force
//######################################################################
#ifndef __Force__
#define __Force__

#include <nova/Tools/Arrays/Array.h>
#include <nova/Tools/Vectors/Vector.h>

namespace Nova{

typedef enum {Nonlinear_Rayleigh_Damping,Constant_Rayleigh_Damping} Damping_type;

template<class T,int d>
class Force
{
    using TV                        = Vector<T,d>;

  public:
    T youngs_modulus,poissons_ratio;                                                        // elasticity coefficients
    T rayleigh_coefficient,damping_coefficient;                                             // damping coefficients
    T mu,lambda,alpha,beta;                                                                 // Lame parameters

    Force()
        :youngs_modulus(1e5),poissons_ratio(.4),rayleigh_coefficient(1e-3),damping_coefficient(1e3)
    {}

    void Initialize_Lame_Parameters()
    {
        lambda=youngs_modulus*poissons_ratio/((1+poissons_ratio)*(1-2*poissons_ratio));
        mu=youngs_modulus/(2*(1+poissons_ratio));
        alpha=rayleigh_coefficient*lambda;
        beta=rayleigh_coefficient*mu;
    }

    virtual ~Force() {}

//######################################################################
    virtual void Add_Elastic_Forces(const Array<TV>& X,Array<TV>& force)=0;
    virtual void Add_Damping_Forces(const Array<TV>& V,Array<TV>& force)=0;
    virtual void Add_Force_Differentials(const Array<TV>& DX,Array<TV>& force)=0;
//######################################################################
};
}
#endif
