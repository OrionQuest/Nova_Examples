//!#####################################################################
//! \file CG_System.h
//!#####################################################################
// Class CG_System
//######################################################################
#ifndef __CG_System__
#define __CG_System__

#include <nova/Tools/Krylov_Solvers/Krylov_System_Base.h>
#include "CG_Vector.h"
#include "Embedded_Deformables_Example.h"

namespace Nova{
template<class T,int d>
class CG_System: public Krylov_System_Base<T>
{
    using TV                                = Vector<T,d>;
    using Base                              = Krylov_System_Base<T>;
    using Vector_Base                       = Krylov_Vector_Base<T>;

    Embedded_Deformables_Example<T,d>& example;
    const T dt;
    T one_over_dt,one_over_dt_squared;

  public:
    CG_System(Embedded_Deformables_Example<T,d>& example_input,const T& dt_input)
        :Base(false,false),example(example_input),dt(dt_input)
    {
        one_over_dt=(T)1./dt;
        one_over_dt_squared=one_over_dt*one_over_dt;
    }

    void Multiply(const Vector_Base& v,Vector_Base& result) const
    {
        Array<TV>& v_array                  = CG_Vector<T,d>::CG_Array(const_cast<Vector_Base&>(v));
        Array<TV>& result_array             = CG_Vector<T,d>::CG_Array(result);

        Array<TV> G(v_array.size()),K(v_array.size());
        result_array.Fill(TV());

        example.Add_Damping_Forces(v_array,G);
        example.Add_Force_Differentials(v_array,K);

        for(size_t i=0;i<example.simulation_mesh->points.size();++i)
            result_array(i)=(example.mass(i)*v_array(i))*one_over_dt_squared-one_over_dt*G(i)-K(i);
    }

    double Inner_Product(const Vector_Base& x,const Vector_Base& y) const
    {
        const Array<TV>& x_array            = CG_Vector<T,d>::CG_Array(x);
        const Array<TV>& y_array            = CG_Vector<T,d>::CG_Array(y);

        double result=(T)0.;
        for(size_t i=0;i<example.simulation_mesh->points.size();++i)
            result+=x_array(i).Dot_Product(y_array(i));
        return result;
    }

    T Convergence_Norm(const Vector_Base& x) const
    {
        const Array<TV>& x_array            = CG_Vector<T,d>::CG_Array(x);

        T result=(T)0.;
        for(size_t i=0;i<example.simulation_mesh->points.size();++i)
            result=std::max(result,x_array(i).Norm());
        return result;
    }

    void Project(Vector_Base& x) const
    {
        Array<TV>& x_array                  = CG_Vector<T,d>::CG_Array(x);
        example.Clear_Values_Of_Kinematic_Particles(x_array);
    }

    void Set_Boundary_Conditions(Vector_Base& v) const {}
    void Project_Nullspace(Vector_Base& x) const {}
};
}
#endif
