#ifndef __OPENGL_3D_IO_SERVICE_H__
#define __OPENGL_3D_IO_SERVICE_H__

#include "IOEvent.h"
#include <functional>
#include <map>
#include <vector>

namespace Nova{

    class ApplicationFactory;
    
    class IOService {
    public:
        IOService( ApplicationFactory& app);
        virtual ~IOService();

        struct Timer {
        private:
            double _interval; // How often do we fire (seconds)
            bool _repeat; // Do we repeat?
            bool _debounce; // Use this to control if timer will fire multiple times if waited on too long...
            double _time_passed;

        public:
            enum TIMER_FLAGS { REPEAT=1, DEBOUNCE=2 };
            Timer(double interval, unsigned int flags) :
                _interval(interval), _repeat(flags & REPEAT), _debounce(flags & DEBOUNCE)
            {};
            virtual ~Timer() {};
            void AddTime( float increment ){
                _time_passed += increment;
            }

            void HardReset(){ _time_passed = 0.0; };

            bool CheckAndReset(){
                if( _time_passed > _interval ){
                    if( _debounce )
                        _time_passed = 0;
                    else
                        _time_passed -= _interval;
                    return true;
                }
                else
                    return false;
            }

            bool Repeat() { return _repeat; };
        };

        enum EventType { MOUSE_DOWN, MOUSE_UP, MOUSE_MOVE, KEY_DOWN, KEY_UP, KEY_HOLD, REDRAW, TIME, SCROLL,
                         DROP, COMMAND };
        
        typedef std::function<void(IOEvent&)> IOEvent_Callback;        

        void Trigger( IOEvent& event );

        unsigned int On( std::string commandName, IOEvent_Callback callback);               
        unsigned int On( EventType type, IOEvent_Callback callback);               

        void Clear( std::string commandName, unsigned int callback_id );
        void Clear( EventType type, unsigned int callback_id );

        // These are a little different, so they get their own prototypes...
        void SetTime( double new_time );
        void ClearTime();
        unsigned int OnTimer( Timer timer, IOEvent_Callback callback);
        void ClearTimer( unsigned int callback_id );

        void PriorityOn( std::string commandName, IOEvent_Callback callback);               
        void PriorityOn( EventType type, IOEvent_Callback callback);

        void PriorityClear( std::string commandName );
        void PriorityClear( EventType type );

        std::vector< std::string > RegisteredCommands();
        std::vector< std::string > CompleteCommand( std::string partial_command );

    private:

        EventType translateEventType( const IOEvent& ) const;
        
        unsigned int next_callback_id;
        double last_time;

        ApplicationFactory& _app;
        std::map< EventType , std::map<unsigned int, IOEvent_Callback> > _callbacks;
        std::map< EventType , IOEvent_Callback > _priority_callback; 

        std::map< std::string , std::map<unsigned int, IOEvent_Callback> > _command_callbacks;
        std::map< std::string , IOEvent_Callback > _command_priority_callback;

        std::map< int, std::pair< Timer, IOEvent_Callback > > _timer_callbacks;
    };

    struct Binding {
        // Must match the following ...
        int trigger;
        int action;
        int modifiers;

        // ... to produce the following:
        std::string command;
        std::vector< std::string > args;

        bool match( Binding other){
            return other.trigger == trigger &&
                other.action == action &&
                other.modifiers == modifiers;
        }
    };
    
    class KeyBinder {
    public:
        KeyBinder( ApplicationFactory& app );
        virtual ~KeyBinder();
        
        void ListKeyBindings() const;
        bool Translate( std::string raw_binding, Binding& binding ) const ;
        void Bind( Binding b);
        void UnBind( Binding b);
        
        void Dispatch( const IOEvent& event ) const;

    private:
        bool FindBinding( Binding& binding_template ) const;
        ApplicationFactory& _app;
        std::map<int, std::vector< Binding > > _boundActions;        
    };

}

#endif
