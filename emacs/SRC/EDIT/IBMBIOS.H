/*
 *	ibmBIOS.h
 *
 *	(c)Copyright Barry A. Scott 1987 
 */


/* interrupt vectors */

#define	INT_timer_int		0x08
#define	INT_kb_int		0x09

#define	INT_video		0x10
#define	INT_equipment		0x11
#define	INT_memory		0x12
#define	INT_diskette		0x13
#define	INT_rs232		0x14
#define	INT_cassette		0x15
#define	INT_at_extension	0x15
#define	INT_keyboard		0x16
#define	INT_printer		0x17
#define	INT_basic		0x18
#define	INT_boot_strap		0x19
#define	INT_time_of_day		0x1a
#define	INT_kb_break		0x1b
#define	INT_time_break		0x1c
#define	INT_video_params	0x1d
#define	INT_disk_base		0x1e
#define	INT_video_ext		0x1f

/* BIOS interrupt parameters */


	/* BIOS VIDEO function */

/*	XT, AT and EGA functions */
#define	VD_set_mode		0x00
#define	VD_set_cur_type		0x01
#define	VD_set_cur_pos		0x02
#define	VD_get_cur_pos		0x03
#define	VD_get_pen_pos		0x04
#define	VD_sel_page		0x05
#define	VD_scroll_up		0x06
#define	VD_scroll_down		0x07
#define	VD_read_char		0x08
#define	VD_write_attr_char	0x09
#define	VD_write_char		0x0a
#define	VD_set_colour		0x0b
#define	VD_write_dot		0x0c
#define	VD_read_dot		0x0d
#define	VD_tty_write_char	0x0e
#define	VD_get_mode		0x0f
/* Last XT function */

/*	EGA functions */
#define	VD_set_palette		0x10
#define	VD_char_gen		0x11
#define  VD_CG_info		 0x30
#define	  VD_CG_I_char_ext	  0x00
#define	  VD_CG_I_char_table	  0x01
#define	  VD_CG_I_8x14		  0x02
#define	  VD_CG_I_double_dot	  0x03
#define	  VD_CG_I_double_dot_top  0x04
#define	  VD_CG_I_9x14		  0x05
#define	VD_alt_select		0x12
#define	 VD_AS_get_ega		 0x10
#define	 VD_AS_alt_prt_screen	 0x20
/*	AT and EGA functions */
#define	VD_write_string		0x13


	/* BIOS EQUIPMENT function */

#define EQ_num_printer( V ) (((V)>>14)&3)
#define	EQ_game_port( V ) (((V)>>12)&1)
#define	EQ_num_rs232( V ) (((V)>>9)&7)
#define	EQ_num_diskette( V ) ((V)&1 ? (((V)>>6)&3)+1 : 0)
#define	EQ_init_video( V ) (((V)>>4)&3)


	/* AT extension functions */

#define	AT_event_wait		0x83
#define	AT_joystick		0x84
#define	AT_system_request	0x85
#define	AT_wait			0x86
#define	AT_move_block		0x87
#define	AT_ext_memory		0x88
#define	AT_get_conf		0xc0
#define	AT_error_status		0x86

	/* KEBOARD function */
#define	KEY_get_char		0x00
#define	KEY_key_hit		0x01
#define	KEY_get_shifts		0x02

#define	KEY_sh_right		0x01
#define	KEY_sh_left		0x02
#define	KEY_sh_ctl		0x04
#define	KEY_sh_alt		0x08
#define	KEY_sh_scroll		0x10
#define	KEY_sh_num		0x20
#define	KEY_sh_caps		0x40
#define	KEY_sh_insert		0x80

	/* FLOPPY disk functions */
#define	FD_reset		0x00
#define	FD_status		0x01
#define	FD_read			0x02
#define	FD_write		0x03
#define	FD_verify		0x04
#define	FD_format		0x05
#define	FD_read_params		0x08
#define	FD_read_dasd_type	0x15
#define	FD_change_line		0x16
#define	FD_set_dasd_type	0x17

/* ibmBIOS.h */
