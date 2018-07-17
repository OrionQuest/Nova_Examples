#ifndef __OPENGL_3D_IO_EVENT_H__
#define __OPENGL_3D_IO_EVENT_H__

#include <string>
#include <vector>
#include <memory>

namespace Nova{

    class IOEvent{
    public:
        enum EVENT_TYPE { TIME = 0, MOUSEBUTTON = 1, MOUSEMOVE = 2,
                          KEYBOARD = 4, DRAW = 8, DROP = 16, SCROLL=32,
                          COMMAND=64};
        
        enum EVENT_MOD { mSHIFT = 1, mCONTROL = 2, mALT = 4, mSUPER = 8};

        enum MOUSE_ACTION { M_MOTION=0, M_UP = 1, M_DOWN = 2, M_SCROLL=3 };
        enum KEY_ACTION { K_UP = 4, K_DOWN = 5, K_HOLD = 6 };

        enum MOUSE_BUTTON { M_LEFT = 1, M_RIGHT = 2, M_MIDDLE = 4, M_OTHER = 8, M_MOVE=16 };
        enum KEY_CODE {KEY_UNKNOWN = -1,KEY_SPACE = 32,KEY_APOSTROPHE = 39,KEY_COMMA = 44,KEY_MINUS = 45,
                       KEY_PERIOD = 46,KEY_SLASH = 47,KEY_0 = 48,KEY_1 = 49,KEY_2 = 50,KEY_3 = 51,
                       KEY_4 = 52,KEY_5 = 53,KEY_6 = 54,KEY_7 = 55,KEY_8 = 56,KEY_9 = 57,KEY_SEMICOLON = 59,
                       KEY_EQUAL = 61,KEY_A = 65,KEY_B = 66,KEY_C = 67,KEY_D = 68,KEY_E = 69,
                       KEY_F = 70,KEY_G = 71,KEY_H = 72,KEY_I = 73,KEY_J = 74,KEY_K = 75,KEY_L = 76,
                       KEY_M = 77,KEY_N = 78,KEY_O = 79,KEY_P = 80,KEY_Q = 81,KEY_R = 82,KEY_S = 83,
                       KEY_T = 84,KEY_U = 85,KEY_V = 86,KEY_W = 87,KEY_X = 88,KEY_Y = 89,KEY_Z = 90,
                       KEY_LEFT_BRACKET = 91,KEY_BACKSLASH = 92,KEY_RIGHT_BRACKET = 93,KEY_GRAVE_ACCENT = 96,
                       KEY_WORLD_1 = 161,KEY_WORLD_2 = 162,KEY_ESCAPE = 256,KEY_ENTER = 257,KEY_TAB = 258,
                       KEY_BACKSPACE = 259,KEY_INSERT = 260,KEY_DELETE = 261,KEY_RIGHT = 262,KEY_LEFT = 263,
                       KEY_DOWN = 264,KEY_UP = 265,KEY_PAGE_UP = 266,KEY_PAGE_DOWN = 267,KEY_HOME = 268,
                       KEY_END = 269,KEY_CAPS_LOCK = 280,KEY_SCROLL_LOCK = 281,KEY_NUM_LOCK = 282,
                       KEY_PRINT_SCREEN = 283,KEY_PAUSE = 284,KEY_F1 = 290,KEY_F2 = 291,KEY_F3 = 292,
                       KEY_F4 = 293,KEY_F5 = 294,KEY_F6 = 295,KEY_F7 = 296,KEY_F8 = 297,KEY_F9 = 298,
                       KEY_F10 = 299,KEY_F11 = 300,KEY_F12 = 301,KEY_F13 = 302,KEY_F14 = 303,KEY_F15 = 304,
                       KEY_F16 = 305,KEY_F17 = 306,KEY_F18 = 307,KEY_F19 = 308,KEY_F20 = 309,KEY_F21 = 310,
                       KEY_F22 = 311,KEY_F23 = 312,KEY_F24 = 313,KEY_F25 = 314,KEY_KP_0 = 320,KEY_KP_1 = 321,
                       KEY_KP_2 = 322,KEY_KP_3 = 323,KEY_KP_4 = 324,KEY_KP_5 = 325,KEY_KP_6 = 326,KEY_KP_7 = 327,
                       KEY_KP_8 = 328,KEY_KP_9 = 329,KEY_KP_DECIMAL = 330,KEY_KP_DIVIDE = 331,
                       KEY_KP_MULTIPLY = 332,KEY_KP_SUBTRACT = 333,KEY_KP_ADD = 334,KEY_KP_ENTER = 335,
                       KEY_KP_EQUAL = 336,KEY_LEFT_SHIFT = 340,KEY_LEFT_CONTROL = 341,KEY_LEFT_ALT = 342,
                       KEY_LEFT_SUPER = 343,KEY_RIGHT_SHIFT = 344,KEY_RIGHT_CONTROL = 345,KEY_RIGHT_ALT = 346,
                       KEY_RIGHT_SUPER = 347,KEY_MENU = 348,KEY_LAST = 348 };

        static int TranslateCode( std::string code );
        static std::string TranslateCode( int code );
        static int TranslateAction( std::string code );
        static int TranslateModifier( std::string code );

        EVENT_TYPE type;
        unsigned int data_flags;

        struct _mousebutton_data { 
            MOUSE_BUTTON button;
            int button_raw;
            MOUSE_ACTION action;
            int mods;
            double x;
            double y;
        };
        
        struct _key_data {
            KEY_CODE key;
            int scancode;
            KEY_ACTION action;
            int mods;
        };
        
        struct _mousemotion_data { 
            double x;
            double y;
            int mods;
        };

        struct _scroll_data { 
            double x;
            double y;
            int mods;
        };

        struct _draw_data {
            int x;
            int y;
            int width;
            int height;
        };
        
        struct _drop_data {
            int num_files;
            const char** filenames;
        };

        struct _command_data {
            std::string command;
            std::vector<std::string> args;
        };

        IOEvent() : type(TIME), data_flags(TIME) {}
        IOEvent( EVENT_TYPE _type ) : type(_type), data_flags(_type) {
            if( type == MOUSEBUTTON )
                mousebutton_data = std::unique_ptr<_mousebutton_data>( new _mousebutton_data() );
            if( type == MOUSEMOVE )
                mousemotion_data = std::unique_ptr<_mousemotion_data>( new _mousemotion_data() );
            if( type == KEYBOARD )
                key_data = std::unique_ptr<_key_data>( new _key_data() );
            if( type == DRAW )
                draw_data = std::unique_ptr<_draw_data>( new _draw_data() );
            if( type == DROP )
                drop_data = std::unique_ptr<_drop_data>( new _drop_data() );
            if( type == SCROLL )
                scroll_data = std::unique_ptr<_scroll_data>( new _scroll_data() );
            if( type == COMMAND )
                command_data = std::unique_ptr<_command_data>( new _command_data() );
        }
        
        std::unique_ptr<_mousebutton_data> mousebutton_data;
        std::unique_ptr<_key_data> key_data;
        std::unique_ptr<_mousemotion_data> mousemotion_data;
        std::unique_ptr<_draw_data> draw_data;
        std::unique_ptr<_drop_data> drop_data;
        std::unique_ptr<_scroll_data> scroll_data;
        std::unique_ptr<_command_data> command_data;
    
        double currentTime;
    };

}

#endif
