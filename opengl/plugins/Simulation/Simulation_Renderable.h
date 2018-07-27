//!#####################################################################
//! \file Simulation_Renderable.h
//!#####################################################################
// Class Simulation_Renderable
//######################################################################
#ifndef __Simulation_Renderable_h__
#define __Simulation_Renderable_h__

#include "RenderableManager.h"
#include "ApplicationFactory.h"
#include "Shader.h"

namespace Nova{
template<class T,int d>
class Simulation_Renderable: public Renderable
{
  public:
    Simulation_Renderable(ApplicationFactory& app,const std::string& directory_name_input,int max_frame_input)
        :Renderable(app),directory_name(directory_name_input),active_frame(0),max_frame(max_frame_input)
    {
        app.GetIOService().On("NEXT-FRAME",[&](IOEvent& event){this->Next_Frame();});
        app.GetIOService().On("PREV-FRAME",[&](IOEvent& event){this->Previous_Frame();});
        app.GetIOService().On("RESET-FRAME",[&](IOEvent& event){this->Reset_Frame();});
    }

    virtual ~Simulation_Renderable() {}

    virtual void draw()
    {
        _app.GetTextRenderingService().Render(std::string("Frame: ")+std::to_string(active_frame),1.0f,0,0.0f);
    }

    virtual void Load_Active_Frame() {}

    void Next_Frame()
    {
        if(active_frame < max_frame)
        {
            ++active_frame;
            Load_Active_Frame();
        }
    }

    void Previous_Frame()
    {
        if(active_frame > 0)
        {
            --active_frame;
            Load_Active_Frame();
        }
    }

    void Reset_Frame()
    {
        if(active_frame != 0)
        {
            active_frame=0;
            Load_Active_Frame();
        }
    }

  protected:
    std::string directory_name;
    int active_frame,max_frame;
};
}
#endif
