//
//    unix_trm.cpp
//    Copyright (c) 1997-1998 Barry A. Scott
//

#include <emacs.h>

#include <tt_unix.h>

#include <unistd.h>
#include <sys/ioctl.h>

extern TerminalControl *tt;

int term_expand_c1 = 1;


//
//    Emacs output
//
//    all characters passed to t_putchar and t_printf are collected
//    in the output_buffer. When the output_size is greater then
//    the output_length less the output_overrun size the buffer is
//    output.
//

const int OUTPUT_OVERRUN( 128 );
const int OUTPUT_LENGTH( 512 + OUTPUT_OVERRUN );

int output_size = 0;
EmacsChar_t output_buffer[OUTPUT_LENGTH];

void TerminalControl_CHAR::t_io_flush()
{
    if( output_size == 0 )
    {
        return;
    }

#if DBG_DISPLAY
    if( dbg_flags&DBG_DISPLAY )
    {
        EmacsString repr( EmacsString::copy, output_buffer, output_size );
        _dbg_msg( FormatString("t_io_flush: '%r'") << repr );
    }
#endif

    // convert to utf-8
    int  utf8_length = length_unicode_to_utf8( output_size, output_buffer );
    unsigned char utf8_buffer[ utf8_length ];
    convert_unicode_to_utf8( output_size, output_buffer, utf8_buffer );

    write( output_channel, &utf8_buffer, utf8_length );

    output_size = 0;
}

void TerminalControl_CHAR::t_io_print( const unsigned char *str )
{
    EmacsChar_t ch;
    const unsigned char *s = str;

    if( !term_expand_c1 )
    {
        while( (ch = *s++) != 0 )
        {
            output_buffer[ output_size ] = ch;
            output_size++;
        }
    }
    else
    {
        while( (ch = *s++) != 0 )
        {
            if( ch >= 128 && ch <= 128+31 )
            {
                output_buffer[ output_size ] = EmacsChar_t( 0x1b );
                output_size++;

                ch = ch - 0x40;
            }

            output_buffer[ output_size ] = ch;
            output_size++;
        }
    }

    if( output_size > term_output_buffer_size )
    {
        t_io_flush();
    }
}

void TerminalControl_CHAR::t_io_putchar( EmacsChar_t ch )
{
    //
    //    If there is no eightbit support on the terminal
    //    and the character is C1 control fold onto a 7-bit code.
    //
    if( term_expand_c1
    && ch >= 128 && ch <= 128+31 )
    {
        output_buffer[ output_size ] = 0x1b;
        output_size++;

        ch = ch - 0x40;
    }

    output_buffer[ output_size ] = ch;
    output_size++;

    if( output_size > term_output_buffer_size )
    {
        t_io_flush();
    }
}

void TerminalControl_CHAR::t_change_attributes()
{

    term_output_buffer_size = OUTPUT_LENGTH - OUTPUT_OVERRUN;
    t_cur_attributes.c_iflag |= IGNBRK;
    t_cur_attributes.c_iflag &= ~(INLCR|ICRNL|IGNCR|ISTRIP|IXON);
    t_cur_attributes.c_oflag &= ~(OPOST);

    t_cur_attributes.c_lflag &= ~(ECHO|ICANON|ISIG|IEXTEN);

    t_cur_attributes.c_cflag &= ~(CSIZE | PARENB);
    t_cur_attributes.c_cflag |= CS8;

    t_cur_attributes.c_cc[VMIN] = 1;
    t_cur_attributes.c_cc[VTIME] = 0;

    int status = tcsetattr( input_channel, TCSADRAIN, &t_cur_attributes );
    if( status == -1 )
    {
        _dbg_msg( FormatString("tcsetattr errno: %s") << strerror( errno ) );
    }
}

#if defined( CBAUD )
int baud_convert[CBAUD+1] =
{ 0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800,
    2400, 4800, 9600, 19200, 38400 };
#endif

void TerminalControl_CHAR::t_select()
{
    if( !t_attr_valid )
    {
#ifdef TIOCGWINSZ
        struct winsize ws;

        ioctl( input_channel, TIOCGWINSZ, &ws );
        t_width = ws.ws_col;
        t_length = ws.ws_row;
        if( t_width < 20 )
        {
            t_width = 80;
        }
        if( t_length < 4 )
        {
            t_length = 25;
        }
#else
        t_width = 80;
        t_length = 25;
#endif
        tcgetattr( input_channel, &t_user_attributes );
        tcgetattr( input_channel, &t_cur_attributes );
        t_attr_valid = 1;
    }

    t_baud_rate =
#if defined( CBAUD )
        baud_convert[ t_cur_attributes.c_cflag&CBAUD ];
#else
        t_cur_attributes.c_ospeed;
#endif
    term_width = t_width;
    term_length = t_length;

    t_il_mf = 0;
    t_il_ov = 1;
    t_ic_mf = 1;
    t_ic_ov = 0;

    return;
}

void TerminalControl_CHAR::k_check_for_input()
{
    return;
}

void TerminalControl_CHAR::term_io_channels( const EmacsString & )
{
    input_channel = STDIN_FILENO;
    output_channel = STDOUT_FILENO;

    term_length = t_length = 25;
    term_width = t_width = 80;

    term_nopadding = 1;
}

void TerminalControl_CHAR::term_restore_charactistics()
{
    if( t_attr_valid )
    {
        tcsetattr( input_channel, TCSADRAIN, &t_user_attributes );
    }
}
