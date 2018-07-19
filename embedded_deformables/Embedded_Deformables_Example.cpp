//!#####################################################################
//! \file Embedded_Deformables_Example.cpp
//!#####################################################################
#include <nova/Tools/Utilities/File_Utilities.h>
#include "Embedded_Deformables_Example.h"
using namespace Nova;
//######################################################################
// Initialize
//######################################################################
template<class T,int d> void Embedded_Deformables_Example<T,d>::
Initialize()
{
    Initialize_Simulation_Mesh();
    Initialize_Position_Based_State();
    Initialize_Embedded_Mesh();
    Initialize_Embedding_Map_And_Weights();
}
//######################################################################
// Initialize_Embedding_Map_And_Weights
//######################################################################
template<class T,int d> void Embedded_Deformables_Example<T,d>::
Initialize_Embedding_Map_And_Weights()
{
    simulation_mesh->Initialize_Hierarchy();
    embedding_map.resize(embedded_surface->points.size());
    embedding_weights.resize(embedded_surface->points.size());
    for(size_t i=0;i<embedded_surface->points.size();++i){Array<int> intersection_list;
        const TV& X=embedded_surface->points(i);
        simulation_mesh->hierarchy->Intersection_List(X,intersection_list);

        for(size_t j=intersection_list.size()-1;j>=0 && j<intersection_list.size();--j){
            int candidate_element=intersection_list(j);
            if(simulation_mesh->Get_Element(candidate_element).Outside(X)) intersection_list.Remove_Index(j);}
        int element=intersection_list(0);embedding_map(i)=element;
        embedding_weights(i)=Get_Embedding_Weights(element,X);}
}
//######################################################################
// Interpolate
//######################################################################
template<class T,int d> Vector<T,d> Embedded_Deformables_Example<T,d>::
Interpolate(const Array<TV>& array,const TV& weights,const int element)
{
    TV X;
    const INDEX& e=simulation_mesh->elements(element);
    for(int v=0;v<d;++v) X+=weights(v)*array(e(v));
    X+=((T)1.-weights.Sum())*array(e(d));
    return X;
}
//######################################################################
// Distribute
//######################################################################
template<class T,int d> void Embedded_Deformables_Example<T,d>::
Distribute(Array<TV>& array,const TV& weights,const TV& value,const int element)
{
    const INDEX& e=simulation_mesh->elements(element);
    for(int v=0;v<d;++v) array(e(v))+=weights(v)*value;
    array(e(d))+=((T)1.-weights.Sum())*value;
}
//######################################################################
// Add_External_Forces
//######################################################################
template<class T,int d> void Embedded_Deformables_Example<T,d>::
Add_External_Forces(Array<TV>& force)
{
    for(size_t i=0;i<simulation_mesh->points.size();++i) force(i)-=TV::Axis_Vector(1)*mass(i)*(T)9.81;
}
//######################################################################
// Add_Elastic_Forces
//######################################################################
template<class T,int d> void Embedded_Deformables_Example<T,d>::
Add_Elastic_Forces(const Array<TV>& X,Array<TV>& force)
{
    for(size_t i=0;i<forces.size();++i) forces(i)->Add_Elastic_Forces(X,force);
}
//######################################################################
// Add_Damping_Forces
//######################################################################
template<class T,int d> void Embedded_Deformables_Example<T,d>::
Add_Damping_Forces(const Array<TV>& V,Array<TV>& force)
{
    for(size_t i=0;i<forces.size();++i) forces(i)->Add_Damping_Forces(V,force);
}
//######################################################################
// Add_Force_Differentials
//######################################################################
template<class T,int d> void Embedded_Deformables_Example<T,d>::
Add_Force_Differentials(const Array<TV>& DX,Array<TV>& force)
{
    for(size_t i=0;i<forces.size();++i) forces(i)->Add_Force_Differentials(DX,force);
}
//######################################################################
// Interpolate_Embedded_Values
//######################################################################
template<class T,int d> void Embedded_Deformables_Example<T,d>::
Interpolate_Embedded_Values(Array<TV>& array,const Array<TV>& embedding_array)
{
    for(size_t i=0;i<embedded_surface->points.size();++i)
        array(i)=Interpolate(embedding_array,embedding_weights(i),embedding_map(i));
}
//######################################################################
// Distribute_To_Embedding_Parents
//######################################################################
template<class T,int d> void Embedded_Deformables_Example<T,d>::
Distribute_To_Embedding_Parents(const Array<TV>& array,Array<TV>& embedding_array)
{
    for(size_t i=0;i<embedded_surface->points.size();++i)
        Distribute(embedding_array,embedding_weights(i),array(i),embedding_map(i));
}
//######################################################################
// Register_Options
//######################################################################
template<class T,int d> void Embedded_Deformables_Example<T,d>::
Register_Options()
{
    Base::Register_Options();

    if(d==2) parse_args->Add_Vector_2D_Argument("-size",Vector<double,2>(10.),"n","Grid resolution");
    else parse_args->Add_Vector_3D_Argument("-size",Vector<double,3>(10.),"n","Grid resolution");
    parse_args->Add_Integer_Argument("-newton_iterations",10,"Number of Newton iterations");

    // for CG
    parse_args->Add_Integer_Argument("-cg_iterations",100,"CG iterations");
    parse_args->Add_Integer_Argument("-cg_restart_iterations",200,"CG restart iteration count");
    parse_args->Add_Double_Argument("-cg_tolerance",1e-3,"CG tolerance");
}
//######################################################################
// Parse_Options
//######################################################################
template<class T,int d> void Embedded_Deformables_Example<T,d>::
Parse_Options()
{
    Base::Parse_Options();

    if(d==2){auto cell_counts_2d=parse_args->Get_Vector_2D_Value("-size");for(int v=0;v<d;++v) counts(v)=(int)cell_counts_2d(v);}
    else{auto cell_counts_3d=parse_args->Get_Vector_3D_Value("-size");for(int v=0;v<d;++v) counts(v)=(int)cell_counts_3d(v);}
    newton_iterations=parse_args->Get_Integer_Value("-newton_iterations");

    cg_iterations=parse_args->Get_Integer_Value("-cg_iterations");
    cg_restart_iterations=parse_args->Get_Integer_Value("-cg_restart_iterations");
    cg_tolerance=(T)parse_args->Get_Double_Value("-cg_tolerance");
}
//######################################################################
// Write_Output_Files
//######################################################################
template<class T,int d> void Embedded_Deformables_Example<T,d>::
Write_Output_Files(const int frame) const
{
    if(frame==first_frame){std::string deformables_filename=(d==2)?output_directory+"/common/metadata.deformables2d":output_directory+"/common/metadata.deformables3d";
        File_Utilities::Write_To_Text_File(deformables_filename,std::to_string(frame));
        File_Utilities::Write_To_File(output_directory+"/common/embedding_map",embedding_map);
        File_Utilities::Write_To_File(output_directory+"/common/embedding_weights",embedding_weights);
        File_Utilities::Write_To_File(output_directory+"/common/indices_of_fixed_particles",indices_of_fixed_particles);
        File_Utilities::Write_To_File(output_directory+"/common/rest_position_of_fixed_particles",rest_position_of_fixed_particles);
        File_Utilities::Write_To_File(output_directory+"/common/Dm_inverse",position_based_state->Dm_inverse);
        File_Utilities::Write_To_File(output_directory+"/common/element_rest_volume",position_based_state->element_rest_volume);}

    File_Utilities::Create_Directory(output_directory+"/"+std::to_string(frame));
    File_Utilities::Write_To_Text_File(output_directory+"/"+std::to_string(frame)+"/frame_title",frame_title);
    std::string mesh_filename=(d==2)?output_directory+"/"+std::to_string(frame)+"/mesh.tri2d":output_directory+"/"+std::to_string(frame)+"/mesh.tet";
    File_Utilities::Write_To_File(mesh_filename,*simulation_mesh);
    std::string surface_filename=(d==2)?output_directory+"/"+std::to_string(frame)+"/curve.curve2d":output_directory+"/"+std::to_string(frame)+"/surface.tri";
    File_Utilities::Write_To_File(surface_filename,*embedded_surface);
    File_Utilities::Write_To_File(output_directory+"/"+std::to_string(frame)+"/volume_velocities",volume_velocities);
    File_Utilities::Write_To_File(output_directory+"/"+std::to_string(frame)+"/surface_velocities",surface_velocities);
}
//######################################################################
// Read_Output_Files
//######################################################################
template<class T,int d> void Embedded_Deformables_Example<T,d>::
Read_Output_Files(const int frame)
{
    File_Utilities::Read_From_File(output_directory+"/common/embedding_map",embedding_map);
    File_Utilities::Read_From_File(output_directory+"/common/embedding_weights",embedding_weights);
    File_Utilities::Read_From_File(output_directory+"/common/indices_of_fixed_particles",indices_of_fixed_particles);
    File_Utilities::Read_From_File(output_directory+"/common/rest_position_of_fixed_particles",rest_position_of_fixed_particles);
    File_Utilities::Read_From_File(output_directory+"/common/Dm_inverse",position_based_state->Dm_inverse);
    File_Utilities::Read_From_File(output_directory+"/common/element_rest_volume",position_based_state->element_rest_volume);
    std::string mesh_filename=(d==2)?output_directory+"/"+std::to_string(frame)+"/mesh.tri2d":output_directory+"/"+std::to_string(frame)+"/mesh.tet";
    File_Utilities::Read_From_File(mesh_filename,*simulation_mesh);
    std::string surface_filename=(d==2)?output_directory+"/"+std::to_string(frame)+"/curve.curve2d":output_directory+"/"+std::to_string(frame)+"/surface.tri";
    File_Utilities::Read_From_File(surface_filename,*embedded_surface);
    File_Utilities::Read_From_File(output_directory+"/"+std::to_string(frame)+"/volume_velocities",volume_velocities);
    File_Utilities::Read_From_File(output_directory+"/"+std::to_string(frame)+"/surface_velocities",surface_velocities);
}
//######################################################################
template class Nova::Embedded_Deformables_Example<float,2>;
template class Nova::Embedded_Deformables_Example<float,3>;
#ifdef COMPILE_WITH_DOUBLE_SUPPORT
template class Nova::Embedded_Deformables_Example<double,2>;
template class Nova::Embedded_Deformables_Example<double,3>;
#endif
