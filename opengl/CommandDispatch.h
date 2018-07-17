#ifndef __OPENGL_3D_COMMAND_DISPATCH_H__
#define __OPENGL_3D_COMMAND_DISPATCH_H__

#include <sstream>
#include <string>
#include <list>

namespace Nova {

    class ApplicationFactory;
    class IOEvent;
    
    class CommandDispatch {
    public:
        CommandDispatch( ApplicationFactory& app);
        virtual ~CommandDispatch();

        void Input( const IOEvent& event );
        void Render( const IOEvent& event );
        
    private:
        ApplicationFactory& _app;
        std::stringstream _buffer;
        bool _commandstarted;
        int _tab_count;
        std::list< std::string > _command_history;
        std::list< std::string >::const_iterator _history_pointer;
        
    };

};


#endif
