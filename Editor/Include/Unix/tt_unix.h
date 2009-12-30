#ifdef vms
# include <ttdef.h>
//
//    fields of an IOSB
//
struct iosb_def
{
    int w_status : 16;
    union  {
//    normal read IOSB
        struct  {
            short int w_read_size : 16;
            short int w_terminator : 16;
            short int w_terminator_size : 16;
            } r_read;
//    sensemode IOSB
        struct  {
            unsigned char b_tx_speed : 8;
            unsigned char b_rx_speed : 8;
            unsigned char b_cr_fill : 8;
            unsigned char b_lf_fill : 8;
            unsigned char b_parity : 8;
            unsigned char b_mbz_1 : 8;
            } r_sensemode;
//    write iosb
        struct  {
            short int w_count : 16;
            int l_mbz_2 : 32;
            } r_write;
        } r_mode;
};
//
//    fields of a sense mode block from tt
//
struct tt_mode
{
    unsigned char b_class;
    unsigned char b_type;
    short int w_width;
    unsigned v_char_1 : 24;
    unsigned char b_length;
    int l_char_2;
};
#endif
#ifdef __unix__
# include <termios.h>
#endif

//
//    TerminalControl_CHAR
//
class TerminalControl_CHAR : public EmacsView
{
public:
    TerminalControl_CHAR( const EmacsString &device );
    virtual ~TerminalControl_CHAR();
    void *operator new(size_t size);
    void operator delete(void *p);

    //
    //    Screen control
    //
    virtual void t_topos(int, int);         // move the cursor to the indicated (row,column); (1,1) is the upper left
    virtual void t_reset();                 // reset terminal (screen is in unkown state, convert it to a known one)
    virtual bool t_update_begin();
    virtual void t_update_end();
    virtual void t_insert_mode(int);        // set or reset character insert mode
    virtual void t_insert_lines(int);       // insert n lines
    virtual void t_delete_lines(int);       // delete n lines
    virtual void t_select();                // select a terminal for operations
    virtual void t_init();                  // initialize terminal settings
    virtual void t_cleanup();               // clean up terminal settings
    virtual void t_wipe_line(int);          // erase to the end of the line
    virtual bool t_window(int);             // set the screen window so that IDline operations only affect the first n lines of the screen
    virtual void t_flash();                 // Flash the screen -- not set if this terminal type won't support it.
    virtual void t_display_activity(  unsigned char );
    virtual void t_change_width(int);       // Routine to call when changing width
    virtual void t_update_line(EmacsLinePtr oldl, EmacsLinePtr newl, int ln);    // Routine to call to update a line
    virtual void t_change_attributes();     // Routine to change attributes
    virtual void t_beep();
    //
    //    Keyboard routines
    //
    virtual int k_input_event( unsigned char *, unsigned int);        // Pointer and other "smart" input
    virtual void k_check_for_input();       // check for any input
    virtual void t_io_putchar( unsigned char );
    virtual void t_io_print( const unsigned char * );
    virtual void t_io_flush();

private:
    void pad( int n, float f );
    void PAD( int n, float f );

    int ansi_cur_x, ansi_cur_y;
    int window_size;

    int cur_region_highlight, cur_special_graphics;

    int cur_hl;

    void term_io_channels( const EmacsString &device );
    int term_init_device();
    void term_change_attributes();
    void term_restore_charactistics();
    void ansi_topos    (
        int row, int column,
        DisplayBody_t *cur_line,
        DisplayAttr_t *cur_line_attr
        );
    void ansi_putchar( DisplayBody_t ch, DisplayAttr_t region_highlight );

#ifdef vms
    //
    //    The current attributes that Emacs is running with
    //
    struct tt_mode t_cur_attributes;
    //
    //    The users attributes that existed when emacs started.
    //
    struct tt_mode t_user_attributes;
    int k_input_readtimed;          // true when need to do a read with timeout
    struct iosb_def k_iosb;
    struct iosb_def t_iosb;
#endif
#ifdef __unix__
    void t_flush(void);             // flush the output buffer
    void t_print( unsigned char *str );
    void t_putchar( unsigned char ch );
    termios t_cur_attributes;
    termios t_user_attributes;
    int input_channel;
    int output_channel;
#endif
};
