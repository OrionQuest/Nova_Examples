//!#####################################################################
//! \file Embedded_Deformables_Driver.cpp
//!#####################################################################
#include <nova/Tools/Krylov_Solvers/Conjugate_Gradient.h>
#include "CG_System.h"
#include "Embedded_Deformables_Driver.h"
using namespace Nova;
//######################################################################
// Constructor
//######################################################################
template<class T,int d> Embedded_Deformables_Driver<T,d>::
Embedded_Deformables_Driver(Embedded_Deformables_Example<T,d>& example_input)
    :Base(example_input),example(example_input)
{}
//######################################################################
// Initialize
//######################################################################
template<class T,int d> void Embedded_Deformables_Driver<T,d>::
Initialize()
{
    Base::Initialize();

    if(!example.restart) example.Initialize();
    else{example.Initialize_Position_Based_State();
        example.Read_Output_Files(example.restart_frame);}

    example.Initialize_Auxiliary_Structures();
}
//######################################################################
// Advance_One_Newton_Iteration
//######################################################################
template<class T,int d> void Embedded_Deformables_Driver<T,d>::
Advance_One_Newton_Iteration(const T target_time,const T dt)
{
    example.Set_Kinematic_Positions(time+dt,example.simulation_mesh->points);
    example.Interpolate_Embedded_Values(example.embedded_surface->points,example.simulation_mesh->points);
    example.Set_Kinematic_Velocities(time+dt,example.volume_velocities);
    example.Interpolate_Embedded_Values(example.surface_velocities,example.volume_velocities);

    const size_t number_of_particles=example.simulation_mesh->points.size();
    Array<TV> rhs(number_of_particles);
    example.position_based_state->Update_Position_Based_State(example.simulation_mesh->points);
    example.Add_Elastic_Forces(example.simulation_mesh->points,rhs);
    example.Add_Damping_Forces(example.volume_velocities,rhs);
    example.Add_External_Forces(rhs);
    for(size_t i=0;i<number_of_particles;++i) rhs(i)+=(example.mass(i)*(Vp(i)-example.volume_velocities(i)))/dt;
    example.Clear_Values_Of_Kinematic_Particles(rhs);

    Array<TV> delta_X(number_of_particles);
    Array<TV> temp_q(number_of_particles),temp_s(number_of_particles),temp_r(number_of_particles),temp_k(number_of_particles),temp_z(number_of_particles);
    CG_Vector<T,d> cg_x(delta_X),cg_b(rhs),cg_q(temp_q),cg_s(temp_s),cg_r(temp_r),cg_k(temp_k),cg_z(temp_z);

    CG_System<T,d> cg_system(example,dt);
    Conjugate_Gradient<T> cg;

    T b_norm=cg_system.Convergence_Norm(cg_b);
    Log::cout<<"Norm: "<<b_norm<<std::endl;

    cg.print_residuals=true;
    cg.print_diagnostics=true;
    cg.restart_iterations=example.cg_restart_iterations;

    // solve
    cg.Solve(cg_system,cg_x,cg_b,cg_q,cg_s,cg_r,cg_k,cg_z,example.cg_tolerance,0,example.cg_iterations);

    // update position and velocity
    example.Clear_Values_Of_Kinematic_Particles(delta_X);
    example.simulation_mesh->points+=delta_X;example.volume_velocities+=delta_X/dt;
    example.Interpolate_Embedded_Values(example.embedded_surface->points,example.simulation_mesh->points);
    example.Interpolate_Embedded_Values(example.surface_velocities,example.volume_velocities);
}
//######################################################################
// Advance_To_Target_Time
//######################################################################
template<class T,int d> void Embedded_Deformables_Driver<T,d>::
Advance_To_Target_Time(const T target_time)
{
    bool done=false;
    for(int substep=1;!done;substep++){
        Log::Scope scope("SUBSTEP","substep "+std::to_string(substep));
        T dt=Compute_Dt(time,target_time);
        Example<T,d>::Clamp_Time_Step_With_Target_Time(time,target_time,dt,done);

        Vp=example.volume_velocities;
        for(size_t i=0;i<example.simulation_mesh->points.size();++i) example.simulation_mesh->points(i)+=dt*Vp(i);
        for(int iteration=0;iteration<example.newton_iterations;++iteration){
            Log::cout<<"Newton Iteration: "<<iteration+1<<"/"<<example.newton_iterations<<std::endl;
            Advance_One_Newton_Iteration(time,dt);}

        if(!done) example.Write_Substep("END Substep",substep,0);
        time+=dt;}
}
//######################################################################
// Simulate_To_Frame
//######################################################################
template<class T,int d> void Embedded_Deformables_Driver<T,d>::
Simulate_To_Frame(const int target_frame)
{
    example.frame_title="Frame "+std::to_string(example.current_frame);
    if(!example.restart) Write_Output_Files(example.current_frame);

    while(example.current_frame<target_frame){
        Log::Scope scope("FRAME","Frame "+std::to_string(++example.current_frame));

        Advance_To_Target_Time(example.Time_At_Frame(example.current_frame));

        example.frame_title="Frame "+std::to_string(example.current_frame);
        Write_Output_Files(++example.output_number);

        *(example.output)<<"TIME = "<<time<<std::endl;}
}
//######################################################################
template class Nova::Embedded_Deformables_Driver<float,2>;
template class Nova::Embedded_Deformables_Driver<float,3>;
#ifdef COMPILE_WITH_DOUBLE_SUPPORT
template class Nova::Embedded_Deformables_Driver<double,2>;
template class Nova::Embedded_Deformables_Driver<double,3>;
#endif
