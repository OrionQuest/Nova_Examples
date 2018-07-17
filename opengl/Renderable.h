#ifndef __OPENGL_3D_RENDERABLE_H__
#define __OPENGL_3D_RENDERABLE_H__

#include <string>

namespace Nova{

    class ApplicationFactory;


}


typedef Nova::Renderable* Nova_CreateRenderable_t(Nova::ApplicationFactory& app);
typedef void Nova_DestroyRenderable_t(Nova::Renderable*);

#endif
