#include "IOEvent.h"

int Nova::IOEvent::TranslateAction( std::string code ){
    if( code == "M_MOTION" )
        return M_MOTION;
    if( code == "M_UP" )
        return M_UP;
    if( code == "M_DOWN" )
        return M_DOWN;
    if( code == "M_SCROLL" )
        return M_SCROLL;
    if( code == "K_UP" )
        return K_UP;
    if( code == "K_DOWN" )
        return K_DOWN;
    if( code == "K_HOLD" )
        return K_HOLD;
    return -1;
}

int Nova::IOEvent::TranslateModifier( std::string code ){
    if( code == "MOD_SHIFT" )
        return mSHIFT;
    if( code == "MOD_CONTROL" )
        return mCONTROL;
    if( code == "MOD_ALT" )
        return mALT;
    if( code == "MOD_SUPER" )
        return mSUPER;
    return -1;
}

int Nova::IOEvent::TranslateCode( std::string code ){
    if( code == "M_LEFT" )
        return M_LEFT;
    if( code == "M_RIGHT" )
        return M_RIGHT;
    if( code == "M_MIDDLE" )
        return M_MIDDLE;
    if( code == "M_MOVE" )
        return M_MOVE;
    if( code == "KEY_SPACE" )
        return KEY_SPACE;
    if( code == "KEY_APOSTROPHE" )
        return KEY_APOSTROPHE;
    if( code == "KEY_COMMA" )
        return KEY_COMMA;
    if( code == "KEY_MINUS" )
        return KEY_MINUS;
    if( code == "KEY_PERIOD" )
        return KEY_PERIOD;
    if( code == "KEY_SLASH" )
        return KEY_SLASH;
    if( code == "KEY_0" )
        return KEY_0;
    if( code == "KEY_1" )
        return KEY_1;
    if( code == "KEY_2" )
        return KEY_2;
    if( code == "KEY_3" )
        return KEY_3;
    if( code == "KEY_4" )
        return KEY_4;
    if( code == "KEY_5" )
        return KEY_5;
    if( code == "KEY_6" )
        return KEY_6;
    if( code == "KEY_7" )
        return KEY_7;
    if( code == "KEY_8" )
        return KEY_8;
    if( code == "KEY_9" )
        return KEY_9;
    if( code == "KEY_SEMICOLON" )
        return KEY_SEMICOLON;
    if( code == "KEY_EQUAL" )
        return KEY_EQUAL;
    if( code == "KEY_A" )
        return KEY_A;
    if( code == "KEY_B" )
        return KEY_B;
    if( code == "KEY_C" )
        return KEY_C;
    if( code == "KEY_D" )
        return KEY_D;
    if( code == "KEY_E" )
        return KEY_E;
    if( code == "KEY_F" )
        return KEY_F;
    if( code == "KEY_G" )
        return KEY_G;
    if( code == "KEY_H" )
        return KEY_H;
    if( code == "KEY_I" )
        return KEY_I;
    if( code == "KEY_J" )
        return KEY_J;
    if( code == "KEY_K" )
        return KEY_K;
    if( code == "KEY_L" )
        return KEY_L;
    if( code == "KEY_M" )
        return KEY_M;
    if( code == "KEY_N" )
        return KEY_N;
    if( code == "KEY_O" )
        return KEY_O;
    if( code == "KEY_P" )
        return KEY_P;
    if( code == "KEY_Q" )
        return KEY_Q;
    if( code == "KEY_R" )
        return KEY_R;
    if( code == "KEY_S" )
        return KEY_S;
    if( code == "KEY_T" )
        return KEY_T;
    if( code == "KEY_U" )
        return KEY_U;
    if( code == "KEY_V" )
        return KEY_V;
    if( code == "KEY_W" )
        return KEY_W;
    if( code == "KEY_X" )
        return KEY_X;
    if( code == "KEY_Y" )
        return KEY_Y;
    if( code == "KEY_Z" )
        return KEY_Z;
    if( code == "KEY_LEFT_BRACKET" )
        return KEY_LEFT_BRACKET;
    if( code == "KEY_BACKSLASH" )
        return KEY_BACKSLASH;
    if( code == "KEY_RIGHT_BRACKET" )
        return KEY_RIGHT_BRACKET;
    if( code == "KEY_GRAVE_ACCENT" )
        return KEY_GRAVE_ACCENT;
    if( code == "KEY_WORLD_1" )
        return KEY_WORLD_1;
    if( code == "KEY_WORLD_2" )
        return KEY_WORLD_2;
    if( code == "KEY_ESCAPE" )
        return KEY_ESCAPE;
    if( code == "KEY_ENTER" )
        return KEY_ENTER;
    if( code == "KEY_TAB" )
        return KEY_TAB;
    if( code == "KEY_BACKSPACE" )
        return KEY_BACKSPACE;
    if( code == "KEY_INSERT" )
        return KEY_INSERT;
    if( code == "KEY_DELETE" )
        return KEY_DELETE;
    if( code == "KEY_RIGHT" )
        return KEY_RIGHT;
    if( code == "KEY_LEFT" )
        return KEY_LEFT;
    if( code == "KEY_DOWN" )
        return KEY_DOWN;
    if( code == "KEY_UP" )
        return KEY_UP;
    if( code == "KEY_PAGE_UP" )
        return KEY_PAGE_UP;
    if( code == "KEY_PAGE_DOWN" )
        return KEY_PAGE_DOWN;
    if( code == "KEY_HOME" )
        return KEY_HOME;
    if( code == "KEY_END" )
        return KEY_END;
    if( code == "KEY_CAPS_LOCK" )
        return KEY_CAPS_LOCK;
    if( code == "KEY_SCROLL_LOCK" )
        return KEY_SCROLL_LOCK;
    if( code == "KEY_NUM_LOCK" )
        return KEY_NUM_LOCK;
    if( code == "KEY_PRINT_SCREEN" )
        return KEY_PRINT_SCREEN;
    if( code == "KEY_PAUSE" )
        return KEY_PAUSE;
    if( code == "KEY_F1" )
        return KEY_F1;
    if( code == "KEY_F2" )
        return KEY_F2;
    if( code == "KEY_F3" )
        return KEY_F3;
    if( code == "KEY_F4" )
        return KEY_F4;
    if( code == "KEY_F5" )
        return KEY_F5;
    if( code == "KEY_F6" )
        return KEY_F6;
    if( code == "KEY_F7" )
        return KEY_F7;
    if( code == "KEY_F8" )
        return KEY_F8;
    if( code == "KEY_F9" )
        return KEY_F9;
    if( code == "KEY_F10" )
        return KEY_F10;
    if( code == "KEY_F11" )
        return KEY_F11;
    if( code == "KEY_F12" )
        return KEY_F12;
    if( code == "KEY_F13" )
        return KEY_F13;
    if( code == "KEY_F14" )
        return KEY_F14;
    if( code == "KEY_F15" )
        return KEY_F15;
    if( code == "KEY_F16" )
        return KEY_F16;
    if( code == "KEY_F17" )
        return KEY_F17;
    if( code == "KEY_F18" )
        return KEY_F18;
    if( code == "KEY_F19" )
        return KEY_F19;
    if( code == "KEY_F20" )
        return KEY_F20;
    if( code == "KEY_F21" )
        return KEY_F21;
    if( code == "KEY_F22" )
        return KEY_F22;
    if( code == "KEY_F23" )
        return KEY_F23;
    if( code == "KEY_F24" )
        return KEY_F24;
    if( code == "KEY_F25" )
        return KEY_F25;
    if( code == "KEY_KP_0" )
        return KEY_KP_0;
    if( code == "KEY_KP_1" )
        return KEY_KP_1;
    if( code == "KEY_KP_2" )
        return KEY_KP_2;
    if( code == "KEY_KP_3" )
        return KEY_KP_3;
    if( code == "KEY_KP_4" )
        return KEY_KP_4;
    if( code == "KEY_KP_5" )
        return KEY_KP_5;
    if( code == "KEY_KP_6" )
        return KEY_KP_6;
    if( code == "KEY_KP_7" )
        return KEY_KP_7;
    if( code == "KEY_KP_8" )
        return KEY_KP_8;
    if( code == "KEY_KP_9" )
        return KEY_KP_9;
    if( code == "KEY_KP_DECIMAL" )
        return KEY_KP_DECIMAL;
    if( code == "KEY_KP_DIVIDE" )
        return KEY_KP_DIVIDE;
    if( code == "KEY_KP_MULTIPLY" )
        return KEY_KP_MULTIPLY;
    if( code == "KEY_KP_SUBTRACT" )
        return KEY_KP_SUBTRACT;
    if( code == "KEY_KP_ADD" )
        return KEY_KP_ADD;
    if( code == "KEY_KP_ENTER" )
        return KEY_KP_ENTER;
    if( code == "KEY_KP_EQUAL" )
        return KEY_KP_EQUAL;
    if( code == "KEY_LEFT_SHIFT" )
        return KEY_LEFT_SHIFT;
    if( code == "KEY_LEFT_CONTROL" )
        return KEY_LEFT_CONTROL;
    if( code == "KEY_LEFT_ALT" )
        return KEY_LEFT_ALT;
    if( code == "KEY_LEFT_SUPER" )
        return KEY_LEFT_SUPER;
    if( code == "KEY_RIGHT_SHIFT" )
        return KEY_RIGHT_SHIFT;
    if( code == "KEY_RIGHT_CONTROL" )
        return KEY_RIGHT_CONTROL;
    if( code == "KEY_RIGHT_ALT" )
        return KEY_RIGHT_ALT;
    if( code == "KEY_RIGHT_SUPER" )
        return KEY_RIGHT_SUPER;
    if( code == "KEY_MENU" )
        return KEY_MENU;
    return -1;
}


std::string Nova::IOEvent::TranslateCode( int code ){
    if( code == M_LEFT )
        return "LEFT_MOUSE";
    if( code == M_RIGHT )
        return "RIGHT_MOUSE";
    if( code == M_MIDDLE )
        return "MIDDLE_MOUSE";
    if( code == M_MOVE )
        return "MOVE_MOUSE";
    if( code == KEY_SPACE )
        return "[SPACE]";
    if( code == KEY_APOSTROPHE )
        return "'";
    if( code == KEY_COMMA )
        return ",";
    if( code == KEY_MINUS )
        return "-";
    if( code == KEY_PERIOD )
        return ".";
    if( code == KEY_SLASH )
        return "/";
    if( code == KEY_0 )
        return "0";
    if( code == KEY_1 )
        return "1";
    if( code == KEY_2 )
        return "2";
    if( code == KEY_3 )
        return "3";
    if( code == KEY_4 )
        return "4";
    if( code == KEY_5 )
        return "5";
    if( code == KEY_6 )
        return "6";
    if( code == KEY_7 )
        return "7";
    if( code == KEY_8 )
        return "8";
    if( code == KEY_9 )
        return "9";
    if( code == KEY_SEMICOLON )
        return ";";
    if( code == KEY_EQUAL )
        return "=";
    if( code == KEY_A )
        return "A";
    if( code == KEY_B )
        return "B";
    if( code == KEY_C )
        return "C";
    if( code == KEY_D )
        return "D";
    if( code == KEY_E )
        return "E";
    if( code == KEY_F )
        return "F";
    if( code == KEY_G )
        return "G";
    if( code == KEY_H )
        return "H";
    if( code == KEY_I )
        return "I";
    if( code == KEY_J )
        return "J";
    if( code == KEY_K )
        return "K";
    if( code == KEY_L )
        return "L";
    if( code == KEY_M )
        return "M";
    if( code == KEY_N )
        return "N";
    if( code == KEY_O )
        return "O";
    if( code == KEY_P )
        return "P";
    if( code == KEY_Q )
        return "Q";
    if( code == KEY_R )
        return "R";
    if( code == KEY_S )
        return "S";
    if( code == KEY_T )
        return "T";
    if( code == KEY_U )
        return "U";
    if( code == KEY_V )
        return "V";
    if( code == KEY_W )
        return "W";
    if( code == KEY_X )
        return "X";
    if( code == KEY_Y )
        return "Y";
    if( code == KEY_Z )
        return "Z";
    if( code == KEY_LEFT_BRACKET )
        return "[";
    if( code == KEY_BACKSLASH )
        return "\\";
    if( code == KEY_RIGHT_BRACKET )
        return "]";
    if( code == KEY_GRAVE_ACCENT )
        return "`";
    if( code == KEY_WORLD_1 )
        return "WORLD_1";
    if( code == KEY_WORLD_2 )
        return "WORLD_2";
    if( code == KEY_ESCAPE )
        return "[ESCAPE]";
    if( code == KEY_ENTER )
        return "[ENTER]";
    if( code == KEY_TAB )
        return "[TAB]";
    if( code == KEY_BACKSPACE )
        return "[BACKSPACE]";
    if( code == KEY_INSERT )
        return "[INSERT]";
    if( code == KEY_DELETE )
        return "[DELETE]";
    if( code == KEY_RIGHT )
        return "[RIGHT]";
    if( code == KEY_LEFT )
        return "[LEFT]";
    if( code == KEY_DOWN )
        return "[DOWN]";
    if( code == KEY_UP )
        return "[UP]";
    if( code == KEY_PAGE_UP )
        return "[PAGE_UP]";
    if( code == KEY_PAGE_DOWN )
        return "[PAGE_DOWN]";
    if( code == KEY_HOME )
        return "[HOME]";
    if( code == KEY_END )
        return "[END]";
    if( code == KEY_CAPS_LOCK )
        return "[CAPS_LOCK]";
    if( code == KEY_SCROLL_LOCK )
        return "[SCROLL_LOCK]";
    if( code == KEY_NUM_LOCK )
        return "[NUM_LOCK]";
    if( code == KEY_PRINT_SCREEN )
        return "[PRINT_SCREEN]";
    if( code == KEY_PAUSE )
        return "[PAUSE]";
    if( code == KEY_F1 )
        return "F1";
    if( code == KEY_F2 )
        return "F2";
    if( code == KEY_F3 )
        return "F3";
    if( code == KEY_F4 )
        return "F4";
    if( code == KEY_F5 )
        return "F5";
    if( code == KEY_F6 )
        return "F6";
    if( code == KEY_F7 )
        return "F7";
    if( code == KEY_F8 )
        return "F8";
    if( code == KEY_F9 )
        return "F9";
    if( code == KEY_F10 )
        return "F10";
    if( code == KEY_F11 )
        return "F11";
    if( code == KEY_F12 )
        return "F12";
    if( code == KEY_F13 )
        return "F13";
    if( code == KEY_F14 )
        return "F14";
    if( code == KEY_F15 )
        return "F15";
    if( code == KEY_F16 )
        return "F16";
    if( code == KEY_F17 )
        return "F17";
    if( code == KEY_F18 )
        return "F18";
    if( code == KEY_F19 )
        return "F19";
    if( code == KEY_F20 )
        return "F20";
    if( code == KEY_F21 )
        return "F21";
    if( code == KEY_F22 )
        return "F22";
    if( code == KEY_F23 )
        return "F23";
    if( code == KEY_F24 )
        return "F24";
    if( code == KEY_F25 )
        return "F25";
    if( code == KEY_KP_0 )
        return "KP_0";
    if( code == KEY_KP_1 )
        return "KP_1";
    if( code == KEY_KP_2 )
        return "KP_2";
    if( code == KEY_KP_3 )
        return "KP_3";
    if( code == KEY_KP_4 )
        return "KP_4";
    if( code == KEY_KP_5 )
        return "KP_5";
    if( code == KEY_KP_6 )
        return "KP_6";
    if( code == KEY_KP_7 )
        return "KP_7";
    if( code == KEY_KP_8 )
        return "KP_8";
    if( code == KEY_KP_9 )
        return "KP_9";
    if( code == KEY_KP_DECIMAL )
        return "KP_DECIMAL";
    if( code == KEY_KP_DIVIDE )
        return "KP_DIVIDE";
    if( code == KEY_KP_MULTIPLY )
        return "KP_MULTIPLY";
    if( code == KEY_KP_SUBTRACT )
        return "KP_SUBTRACT";
    if( code == KEY_KP_ADD )
        return "KP_ADD";
    if( code == KEY_KP_ENTER )
        return "KP_ENTER";
    if( code == KEY_KP_EQUAL )
        return "KP_EQUAL";
    if( code == KEY_LEFT_SHIFT )
        return "LEFT_SHIFT";
    if( code == KEY_LEFT_CONTROL )
        return "LEFT_CONTROL";
    if( code == KEY_LEFT_ALT )
        return "LEFT_ALT";
    if( code == KEY_LEFT_SUPER )
        return "LEFT_SUPER";
    if( code == KEY_RIGHT_SHIFT )
        return "RIGHT_SHIFT";
    if( code == KEY_RIGHT_CONTROL )
        return "RIGHT_CONTROL";
    if( code == KEY_RIGHT_ALT )
        return "RIGHT_ALT";
    if( code == KEY_RIGHT_SUPER )
        return "RIGHT_SUPER";
    if( code == KEY_MENU )
        return "MENU";
    return "";
}
