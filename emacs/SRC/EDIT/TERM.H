/* 
 *	term.h
 * 
 *	Copyright 1993 Barry A. Scott 
 */
#include <os.h>			/* target specific definitions */

/* 
 *	structure definitions 
 */
#include <em_mac.h>
#include <em_gen.h>
#include <display.h>
#include <tt.h>

/* variables */
extern int black_on_white;

extern int
	dbg_flags,
	term_edit,
	term_width,
	term_length,
	term_deccrt,
	term_nopadding;

extern unsigned char *mode_line_rendition_ref;
extern unsigned char *region_rendition_ref;
extern unsigned char *window_rendition_ref;
extern int term_output_buffer_size;

/* routines */
extern int term_init_device( struct trmcontrol * );
extern void term_io_channels ( struct trmcontrol *, unsigned char * );
extern void term_restore_charactistics(struct trmcontrol *);
