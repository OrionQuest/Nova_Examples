//!#####################################################################
//! \file Position_Based_State_Linear_Elasticity.h
//!#####################################################################
// Class Position_Based_State_Linear_Elasticity
//######################################################################
#ifndef __Position_Based_State_Linear_Elasticity__
#define __Position_Based_State_Linear_Elasticity__

#include "Position_Based_State.h"

namespace Nova{
template<class T,int d>
class Position_Based_State_Linear_Elasticity: public Position_Based_State<T,d>
{
    using TV                        = Vector<T,d>;
    using Base                      = Position_Based_State<T,d>;
    using T_Volume                  = Tetrahedralized_Volume<T,d>;

  public:
    using Base::simulation_mesh;

    Position_Based_State_Linear_Elasticity(const T_Volume* simulation_mesh)
        :Base(simulation_mesh)
    {}

    virtual void Update_Position_Based_State(const Array<TV>& X) override
    {}
};
}
#endif
