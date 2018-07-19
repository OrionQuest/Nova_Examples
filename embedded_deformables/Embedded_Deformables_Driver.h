//!#####################################################################
//! \file Embedded_Deformables_Driver.h
//!#####################################################################
// Class Embedded_Deformables_Driver
//######################################################################
#ifndef __Embedded_Deformables_Driver__
#define __Embedded_Deformables_Driver__

#include <nova/Tools/Utilities/Driver.h>
#include "Embedded_Deformables_Example.h"

namespace Nova{
template<class T,int d>
class Embedded_Deformables_Driver: public Driver<T,d>
{
    using TV                = Vector<T,d>;
    using Base              = Driver<T,d>;

    using Base::time;
    using Base::Compute_Dt;using Base::Write_Output_Files;

  public:
    Embedded_Deformables_Example<T,d>& example;
    Array<TV> Vp;

    Embedded_Deformables_Driver(Embedded_Deformables_Example<T,d>& example_input);
    ~Embedded_Deformables_Driver() {}

//######################################################################
    void Initialize();
    void Advance_One_Newton_Iteration(const T target_time,const T dt);
    void Advance_To_Target_Time(const T target_time) override;
    void Simulate_To_Frame(const int frame) override;
//######################################################################
};
}
#endif
