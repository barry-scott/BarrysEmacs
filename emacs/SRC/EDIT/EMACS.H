/* 
 *	emacs.h
 * 
 *	Copyright 1986 Barry A. Scott 
 */

#ifdef __cplusplus
struct emacs_buffer;
struct syntax_table;
struct windowring;
struct emacs_array;
struct marker;
struct binding;
struct prognode;
struct bound_name;
struct keymap;
struct emacs_buffer;
struct abbrevtable;
struct journal_block;
#endif

#include <errno.h>
#include <os.h>			/* target specific definitions */
#include <em_gen.h>		/* general constants used in emacs */
#include <em_mac.h>		/* general useful macros */

/* 
 *	structure definitions 
 */
#include <queue.h>
#include <em_time.h>
#include <syntax.h>
#include <display.h>
#include <mlisp.h>
#include <fileio.h>
#include <keyboard.h>
#include <buffer.h>
#include <abbrev.h>
#include <ndbm.h>
#include <search.h>
#include <tt.h>
#include <window.h>
#include <undo.h>
#include <journal.h>

#include <fileserv.h>

/* 
 *	declare routines as external 
 */
#include <em_rtn.h>

/* 
 *	declare variables as external 
 */
#include <em_var.h>
