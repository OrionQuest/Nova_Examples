//!#####################################################################
//! \file main.cpp
//!#####################################################################
#include "../Embedded_Deformables_Driver.h"
#include "Standard_Tests/Standard_Tests.h"
using namespace Nova;

int main(int argc,char** argv)
{
    enum {d=3};
    typedef float T;typedef Vector<T,d> TV;
    typedef Vector<int,d> T_INDEX;

    Embedded_Deformables_Example<T,d> *example=new Standard_Tests<T,d>();
    example->Parse(argc,argv);

    File_Utilities::Create_Directory(example->output_directory);
    File_Utilities::Create_Directory(example->output_directory+"/common");
    Log::Instance()->Copy_Log_To_File(example->output_directory+"/common/log.txt",false);

    Embedded_Deformables_Driver<T,d> driver(*example);
    driver.Execute_Main_Program();

    delete example;

    return 0;
}
