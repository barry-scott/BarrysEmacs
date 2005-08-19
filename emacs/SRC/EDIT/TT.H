/*** MODULE tt_struct ***/
/*	Copyright (c) 1982, 1983, 1984, 1985                                */
/*		Barry A. Scott and nick Emery                               */

#ifdef vms
#include <ttdef.h>
/*										*/
/*	fields of an IOSB							*/
/*										*/
struct iosb_def {
	int w_status : 16;
	union  {
/*	normal read IOSB                                                    */
	    struct  {
	        short int w_read_size : 16;
	        short int w_terminator : 16;
	        short int w_terminator_size : 16;
	        } r_read;
/*	sensemode IOSB                                                      */
	    struct  {
	        unsigned char b_tx_speed : 8;
	        unsigned char b_rx_speed : 8;
	        unsigned char b_cr_fill : 8;
	        unsigned char b_lf_fill : 8;
	        unsigned char b_parity : 8;
	        unsigned char b_mbz_1 : 8;
	        } r_sensemode;
/*	write iosb                                                          */
	    struct  {
	        short int w_count : 16;
	        int l_mbz_2 : 32;
	        } r_write;
	    } r_mode;
	};
/*                                                                          */
/*	fields of a sense mode block from tt                                */
/*                                                                          */
struct tt_mode {
	unsigned char b_class;
	unsigned char b_type;
	short int w_width;
	unsigned v_char_1 : 24;
	unsigned char b_length;
	int l_char_2;
	};
#endif
#ifdef unix
#if defined(EMACS_USE_TERMIOS)
#include <termios.h>
typedef struct termios emacs_termio;
#else
#include <termio.h>
typedef struct termio emacs_termio;
#endif
#endif

#define TT_STRUCT_VERSION 3
#define K_CHAR_SIZE 80
struct trmcontrol {
/*                                                                          */
/*	Screen control                                                      */
/*                                                                          */
	int t_version;
	void *t_context;                 /* terminal context                 */
	void (*t_topos)(struct trmcontrol *,int, int);	/* move the cursor to the indicated (row,column); (1,1) is the upper left */
	void (*t_reset)(struct trmcontrol *);		/* reset terminal (screen is in unkown state, convert it to a known one) */
	void (*t_update_begin)(struct trmcontrol *);
	void (*t_update_end)(struct trmcontrol *);
	void (*t_insert_mode)(struct trmcontrol *,int);		/* set or reset character insert mode */
	void (*t_highlight_mode)(struct trmcontrol *,int);		/* set or reset highlighting        */
	void (*t_inslines)(struct trmcontrol *,int);	/* insert n lines                   */
	void (*t_dellines)(struct trmcontrol *,int);	/* delete n lines                   */
	void (*t_blanks)(struct trmcontrol *,int);		/* print n blanks                   */
	void (*t_select)( struct trmcontrol *, unsigned char *, unsigned char *, unsigned char *);		/* select a terminal for operations */
	void (*t_init)( struct trmcontrol *);	/* initialize terminal settings     */
	void (*t_cleanup)(struct trmcontrol *);	/* clean up terminal settings       */
	void (*t_wipe_line)(struct trmcontrol *,int);	/* erase to the end of the line     */
	void (*t_wipe_screen)(struct trmcontrol *);		/* erase the entire screen          */
	void (*t_delete_chars)(struct trmcontrol *,int);		/* delete n characters              */
	void (*t_write_chars)(struct trmcontrol *,unsigned char *, unsigned char *);		/* write characters; either inserting or overwriting according to the current character insert */
	void (*t_window)(struct trmcontrol *,int);		/* set the screen window so that IDline operations only affect the first n lines of the screen */
	void (*t_flash)(struct trmcontrol *);		/* Flash the screen -- not set if this terminal type won't support it. */
	void (*t_geometry_change)(struct trmcontrol *);
	void (*t_display_activity)( struct trmcontrol *, unsigned char );
/* costs are expressed as number_affected*mf + ov                           */
/*	cost to insert/delete 1 line: (number of lines left)*ILmf+ILov      */
/*	cost to insert one character: (number of chars left on line)*ICmf+ICov */
/*	cost to delete n characters:  n*DCmf+DCov                           */
	int t_il_mf;			/* insert lines multiply factor     */
	int t_il_ov;			/* insert lines overhead            */
	int t_ic_mf;			/* insert character multiply factor */
	int t_ic_ov;			/* insert character overhead        */
	int t_dc_mf;			/* delete character multiply factor */
	int t_dc_ov;			/* delete character overhead        */
	void (*t_change_width)(struct trmcontrol *,int);	/* Routine to call when changing width */
	void (*t_update_line)(struct trmcontrol *,struct emacs_line *oldl, struct emacs_line *newl, int ln);	/* Routine to call to update a line */
	void (*t_change_attributes)(struct trmcontrol *);	/* Routine to change attributes     */
	int t_length;			/* screen length                    */
	int t_width;			/* screen width                     */
	int t_baud_rate;		/* buad rate of terminal            */
	void (*t_beep)(struct trmcontrol *);

/*                                                                          */
/*	Keyboard routines                                                   */
/*                                                                          */
	void *k_context;		/* context of keyboard not used by emacs */
	int (*k_input_enable)(struct trmcontrol *);	/* start input                      */
	int (*k_input_disable)(struct trmcontrol *);	/* stop input                       */
	void (*k_input_char)(struct trmcontrol *,int);	/* pass new charater to Emacs       */
	int (*k_input_event)(struct trmcontrol *, unsigned char *, unsigned int);		/* Pointer and other "smart" input  */
	int (*k_input_end)(struct trmcontrol *);	/* input terminated                 */
	void (*k_check_for_input)(struct trmcontrol *);	/* check for any input */
#ifdef SAVE_ENVIRONMENT
	void * (*t_alloc_mem)(int, enum malloc_block_type );	/* allocate memory */
#else
 	void * (*t_alloc_mem)(int);
#endif
	void (*t_free_mem)(void *);		/* free allocated memory */
	void (*t_io_putchar)( struct trmcontrol *,unsigned char );
	void (*t_io_print)( struct trmcontrol *,unsigned char * );
	void (*t_io_printf)( struct trmcontrol *,unsigned char *, ... );
	void (*t_io_flush)(struct trmcontrol *);

	struct emacs_line **t_phys_screen;	/* the current( physical ) screen */
	struct emacs_line **t_desired_screen;	/* the desired( virtual ) screen */

	unsigned char k_char [80];

	unsigned t_attr_valid : 1;
	unsigned k_enabled : 1;	/* keyboard is enabled              */
#if INT_MAX == 32767
	unsigned t_bit_fill : 14;
#else
	unsigned t_bit_fill : 30;
#endif
#ifdef vms
/*                                                                          */
/*	The current attributes that Emacs is running with                   */
/*                                                                          */
	struct tt_mode t_cur_attributes;
/*										*/
/*	The users attributes that existed when emacs started.			*/
/*										*/
	struct tt_mode t_user_attributes;
	int k_input_readtimed;			/* true when need to do a read with timeout */
	struct iosb_def k_iosb;
	struct iosb_def t_iosb;
#endif
#ifdef unix
	emacs_termio t_cur_attributes;
	emacs_termio t_user_attributes;
	int input_channel;
	int output_channel;
#endif
	};

#define MISSINGFEATURE 9999
