//
//    Copyright (c) 1982-1998
//        Barry A. Scott
//

const int TT_STRUCT_VERSION( 3 );
const int K_CHAR_SIZE( 80 );
class TerminalControl
{
public:
    TerminalControl();
    virtual ~TerminalControl();
    //
    //    Screen control
    //
    virtual void t_topos( int, int ) { }        // move the cursor to the indicated (row,column); (1,1) is the upper left
    virtual void t_reset() { }                  // reset terminal (screen is in unknown state, convert it to a known one)
    virtual bool t_update_begin() { return true; }
    virtual void t_update_end() { }
    virtual void t_insert_mode( int ) { }       // set or reset character insert mode
    virtual void t_insert_lines( int ) { }      // insert n lines
    virtual void t_delete_lines( int ) { }      // delete n lines
    virtual void t_select() { }                 // select a terminal for operations
    virtual void t_init() { }                  // initialize terminal settings
    virtual void t_cleanup();                   // clean up terminal settings
    virtual void t_wipe_line( int ) { }         // erase to the end of the line
    virtual bool t_window( int ) { return false; }    // set the screen window so that IDline operations only affect the first n lines of the screen
    virtual void t_flash() { }                  // Flash the screen -- not set if this terminal type won't support it.
    virtual void t_geometry_change();
    virtual void t_display_activity( unsigned char ) { }
    virtual void t_move_line( int old_line_num, int new_line_num )
    {
        int delta = new_line_num - old_line_num;

        if( delta > 0 )
        {
            t_window( new_line_num );
            t_topos( old_line_num, 1 );
            t_insert_lines( delta );
        }
        else
        {
            t_window( old_line_num );
            t_topos( new_line_num, 1 );
            t_delete_lines( -delta );
        }
    }
    //
    // costs are expressed as number_affected*mf + ov
    //    cost to insert/delete 1 line: (number of lines left)*ILmf+ILov
    //    cost to insert one character: (number of chars left on line)*ICmf+ICov
    //    cost to delete n characters:  n*DCmf+DCov
    //
    int t_il_mf;                    // insert lines multiply factor
    int t_il_ov;                    // insert lines overhead
    int t_ic_mf;                    // insert character multiply factor
    int t_ic_ov;                    // insert character overhead
    int t_dc_mf;                    // delete character multiply factor
    int t_dc_ov;                    // delete character overhead
    virtual void t_change_width( int ) { }        // Routine to call when changing width
    virtual void t_update_line( EmacsLinePtr oldl, EmacsLinePtr newl, int ln );    // Routine to call to update a line
    virtual void t_change_attributes() { }        // Routine to change attributes
    int t_baud_rate;                // buad rate of terminal
    virtual void t_beep() { }


    int t_length;                   // screen length
    int t_width;                    // screen width

    EmacsLinePtr t_phys_screen[ MSCREENLENGTH + 1 ];        // the current( physical ) screen
    EmacsLinePtr t_desired_screen[ MSCREENLENGTH + 1 ];     // the desired( virtual ) screen

    //
    //    Keyboard routines
    //
    virtual void k_input_enable();              // start input
    virtual void k_input_disable();             // stop input
    virtual void k_input_char( int, bool );     // pass new charater to Emacs
    virtual void k_input_char_string( const unsigned char *keys, bool );        // pass new charaters to Emacs
    virtual void k_input_char_string( const char *keys, bool shift)
    {
        k_input_char_string( (const unsigned char *)keys, shift );
    }

    virtual int k_input_event( unsigned char *, unsigned int ) { return 0; }    // Pointer and other "smart" input
    virtual void k_check_for_input() { }        // check for any input
    virtual void t_io_putchar( unsigned char ) { }
    virtual void t_io_print( const unsigned char * ) { }
    virtual void t_io_printf( const char *, ... );
    virtual void t_io_flush() { }

    void k_interrupt_emacs();                   // call to cause a interruption of emacs from keyboard or gui

    bool k_input_is_enabled;                    // must be true for input to go to emacs
    unsigned char k_char[80];

    unsigned t_attr_valid : 1;
    unsigned k_enabled : 1;                     // keyboard is enabled
#if INT_MAX == 32767
    unsigned t_bit_fill : 14;
#else
    unsigned t_bit_fill : 30;
#endif
};

const int MISSINGFEATURE( 9999 );
