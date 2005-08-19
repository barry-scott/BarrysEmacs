/*	Copyright (c) 1982, 1983, 1984, 1985                                */
/*		Barry A. Scott and Nick Emery                               */
/* key->procedure mapping table definitions                                 */

#define LAST_KEYS_SIZE 80               /* Number of chars remembered from processing a key */
#define CHARACTER_QUEUE_SIZE 4000
#define CE_TYPE_CHAR 1
#define CE_TYPE_PAR_CHAR 2
#define CE_TYPE_PAR_SEP 3
#define CE_TYPE_FIN_CHAR 4
struct char_element
	{
	struct queue ce_queue;
	unsigned char ce_char;
	unsigned char ce_type;
	};
#define M_CS_CVT_CSI 1
#define M_CS_CVT_8BIT 2
#define M_CS_PAR_CHAR 4
#define M_CS_PAR_SEP 8
#define M_CS_FIN_CHAR 16
#define M_CS_INT_CHAR 32

#define KEYMAP_SHORT_SIZE 12

struct keymap_short
	{
	int k_used;
	unsigned char k_chars [KEYMAP_SHORT_SIZE];
	struct bound_name *k_sbinding [KEYMAP_SHORT_SIZE];
	};
struct keymap_long
	{
	struct bound_name *k_binding [256];
	};

#define KEYMAP_SHORT_TYPE 0
#define KEYMAP_LONG_TYPE 1
struct keymap
	{
	int k_type;
	union k_map_tag
		{
		struct keymap_short *k_short;
		struct keymap_long *k_long;
		} k_map;
	};

#define MEMLEN 1002                     /* (MemLen+2) % 4  must equal 0     */

struct emacs_work_item
	{
	struct queue header;
	void (*rtn)( struct emacs_work_item * );
	int num;
	void *pointer;
	};
