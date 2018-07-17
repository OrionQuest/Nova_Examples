#include "CommandDispatch.h"
#include "ApplicationFactory.h"
#include "TextRenderingService.h"
#include "IOEvent.h"

#include <iostream>

Nova::CommandDispatch::CommandDispatch( Nova::ApplicationFactory& app) : _app(app), _commandstarted(false)
{


}

Nova::CommandDispatch::~CommandDispatch()
{

}

std::string common_prefix( const std::vector< std::string >& potential_strings  ){
    std::string prefix = "";
    if( potential_strings.size() == 0 )
        return prefix;
    bool matches = true;
    int index = 0;
    do{
        if( potential_strings[0].size() > index ){
            char symbol = potential_strings[0][index];
            matches = true;
            for( auto s : potential_strings ){
                if( s.size() <= index ){
                    matches = false;
                    continue;
                }
                if( symbol != s[index] )
                    matches = false;
            }
            if( matches ){
                prefix += symbol;
                index++;
            }
        }
        else
            matches = false;
    } while( matches );

    return prefix;
}

void
Nova::CommandDispatch::Input( const Nova::IOEvent& event )
{
    if( _commandstarted ){
        if( event.key_data->key == IOEvent::KEY_GRAVE_ACCENT ){
            // std::cout << "Console closed." << std::endl;
            _app.GetIOService().PriorityClear( IOService::KEY_DOWN );
            _commandstarted = false;
            _buffer.str("");
            return;
        }
        
        if( event.key_data->key >= IOEvent::KEY_SPACE && event.key_data->key < IOEvent::KEY_GRAVE_ACCENT )
            _buffer << char( event.key_data->key );

        if( event.key_data->key == IOEvent::KEY_UP ){
            if( _history_pointer != _command_history.begin() ){
                _history_pointer--;
                _buffer.str(*_history_pointer);
                _buffer.seekp(0,_buffer.end);
            }
        }

        if( event.key_data->key == IOEvent::KEY_DOWN ){
            if( _history_pointer != _command_history.end() ){
                _history_pointer++;
            }

            if( _history_pointer != _command_history.end() ){
                _buffer.str(*_history_pointer);
                _buffer.seekp(0,_buffer.end);
            }
            else{
                _buffer.str("");
                _buffer.seekp(0,_buffer.end);               
            }             
        }

        if( event.key_data->key == IOEvent::KEY_BACKSPACE ){
            std::string last_buffer = _buffer.str();
            if( last_buffer.size() > 0 )
                last_buffer.pop_back();
            _buffer.str(last_buffer);
            _buffer.seekp (0, _buffer.end);
        }        
        
        if(  event.key_data->key == IOEvent::KEY_TAB ){
            _tab_count += 1;
            std::vector<std::string> commands = _app.GetIOService().CompleteCommand( _buffer.str() );
            if( commands.size() == 0 ){
                _tab_count = 0;
            }
            else{
                _buffer.str(common_prefix(commands));
                _buffer.seekp(0, _buffer.end );
                if( commands.size() == 1 )
                    _tab_count = 0;
                if( commands.size() > 1 && _tab_count > 1){
                    std::cout << "Potential Completions: "<< std::endl;
                    for( auto command : commands ){
                        std::cout << command << std::endl;
                    }
                }
            }
        }

        if( event.key_data->key == IOEvent::KEY_ENTER ||
            event.key_data->key == IOEvent::KEY_KP_ENTER ){
            _buffer.seekp (0, _buffer.beg);            
            std::string raw_command = _buffer.str();
            if( raw_command != "" ){
                _command_history.push_back( raw_command );
                _history_pointer = _command_history.end();
            }
            std::string command;
            _buffer >> command;
            std::vector< std::string > args;
            while( !_buffer.eof() ){
                std::string arg;
                _buffer>>arg;
                if( arg != "" )
                    args.push_back( arg );
            }
            
            _buffer.str("");
            _buffer.seekp (0, _buffer.beg);
            _commandstarted = false;
            _app.GetIOService().PriorityClear( IOService::KEY_DOWN );

            //std::cout << "Command Entered: " << command << std::endl;
            //std::cout << "\tArgs: ";
            //for( auto arg : args )
            //    std::cout << arg << ", ";
            //std::cout << std::endl;

            IOEvent event( IOEvent::COMMAND );
            event.command_data->command = command;
            event.command_data->args = args;
            _app.GetIOService().Trigger( event ); // Fire the command out!

            return;
        }

    }
    else{
        if( event.key_data->key == IOEvent::KEY_GRAVE_ACCENT ){
            try{
                // std::cout << "Console opened." << std::endl;
                _app.GetIOService().PriorityOn( IOService::KEY_DOWN,  [&](IOEvent& event){ this->Input( event ); });
                _tab_count = 0;
                _commandstarted = true;
                _buffer.clear();
                _buffer.str("");
                _buffer.seekp (0, _buffer.beg);
                _history_pointer = _command_history.end();
            }
            catch( std::exception& e ){
                _commandstarted = false;                
            }
        }
    }
}

void
Nova::CommandDispatch::Render( const Nova::IOEvent& event )
{
    // std::cout << "Console render." << _commandstarted << std::endl;
    if( _commandstarted ){
        std::string displayText = "> ";
        displayText += _buffer.str();
        _app.GetTextRenderingService() << displayText << std::endl;
    }

}
