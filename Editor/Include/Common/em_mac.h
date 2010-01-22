//    Copyright 1985-1995
//        Barry A. Scott
//
//    macros required to access Emacs data structures
//

inline int interactive() { return macro_replay_next < 0 && cur_exec == NULL; }

//
// cur_col returns the current print column number for dot, which may
// have to be calculated
//
inline int cur_col()
{
    if( col_valid )
        return dot_col;
    else
        return calc_col();
}

//
// dot should ONLY be given a value by calling set_dot(new) -- it ensures
// that all associated bookkeeping is done.
//
inline int set_dot( int n )
{
    if( input_mode == 1 ) gui_input_mode_set_dot( n );
    col_valid = 0;
    dot = n;
    return n;
}

//
// dot should be moved left or right using the following macros -- they
// attempt (or will, eventually) to keep dot_col valid.  They do not check
// the new valid of dot: you have to do that.
//
inline void dot_right( int n )
{
    set_dot( dot + n );
}

inline void dot_left( int n )
{
    set_dot( dot - n );
}

inline bool control_character( int c)
{
    if( c >= 0x0000 && c <= 0x001f )
        return true;
    if( c == 0x007f )
        return true;
    if( c >= 0x0080 && c <= 0x009f )
        return true;

    return false;
}

//
// the functions used by the macros above
//
extern int getnum( const EmacsString &prompt );
extern int get_number_interactive( const EmacsString &prompt );
extern int get_number_mlisp();

extern EmacsString getstr( const EmacsString &prompt );
extern EmacsString getnbstr( const EmacsString &prompt );
extern EmacsString get_string_interactive( const EmacsString &prompt );
extern EmacsString get_string_interactive( const EmacsString &prompt, const EmacsString &default_value );
extern EmacsString get_nb_string_interactive( const EmacsString &prompt );
extern EmacsString get_nb_string_interactive( const EmacsString &prompt, const EmacsString &default_value );
extern EmacsString get_string_mlisp();

extern EmacsString get_key( KeyMap *kmap, const EmacsString &prompt );
extern EmacsString get_key_interactive( KeyMap *kmap, const EmacsString &prompt );
extern EmacsString get_key_mlisp();

extern EmacsString br_get_string_interactive( int breaksp, const EmacsString &prefix, const EmacsString &prompt );
extern EmacsString br_get_string_mlisp();
