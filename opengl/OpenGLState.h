//#####################################################################
// Copyright (c) 2016, Nathan Mitchell.
//#####################################################################

//##################################################################### 
#ifndef __OPENGL_STATE_H__
#define __OPENGL_STATE_H__

namespace Nova {
    
    class GLStateEnable {
        GLenum state_flag;
        GLboolean old_state;
    public:
        GLStateEnable( GLenum flag ) : state_flag(flag) {            
            glGetBooleanv( state_flag, &old_state );
            glEnable( state_flag );
        }
        ~GLStateEnable(){
            if(old_state)
                glEnable(state_flag);
            else
                glDisable(state_flag);
        }
    };

    class GLStateDisable {
        GLenum state_flag;
        GLboolean old_state;
    public:
        GLStateDisable( GLenum flag ) : state_flag(flag) {      
            glGetBooleanv( state_flag, &old_state );
            glDisable( state_flag );
        }
        ~GLStateDisable(){
            if(old_state)
                glEnable(state_flag);
            else
                glDisable(state_flag);
        }
    };


};

#endif
