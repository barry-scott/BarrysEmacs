/*
 *	msdos.h
 */

/* interrupt vectors */

#define	INT_abs_read_disk	0x25
#define INT_abs_write_disk	0x26
#define INT_stay_resident	0x27
#define INT_msdos		0x21

/* INT_msdos calls */
#define DOS_terminate_program	0x00
#define DOS_read_kbd_and_echo	0x01
#define	DOS_display_char	0x02
#define	DOS_aux_input		0x03
#define	DOS_aux_output		0x04
#define	DOS_print_char		0x05
#define	DOS_dir_console_io	0x06
#define	DOS_dir_console_input	0x07
#define	DOS_read_kbd		0x08
#define	DOS_display		0x09
#define	DOS_get_string		0x0a
#define	DOS_check_kbd_status	0x0b
#define	DOS_flush_and_read_kbd	0x0c
#define	DOS_reset_disk		0x0d
#define	DOS_select_disk		0x0e
#define	DOS_open		0x0f
#define	DOS_close		0x10
#define	DOS_search_first	0x11
#define	DOS_search_next		0x12
#define	DOS_delete		0x13
#define	DOS_read_seq		0x14
#define	DOS_write_seq		0x15
#define	DOS_create		0x16
#define	DOS_rename		0x17
#define	DOS_current_disk	0x19
#define	DOS_set_dta		0x1a
#define	DOS_def_drive_data	0x1b
#define	DOS_drive_data		0x1c
#define	DOS_read_ran		0x21
#define	DOS_write_ran		0x22
#define	DOS_file_size		0x23
#define	DOS_set_relative_record	0x24
#define	DOS_set_vector		0x25
#define	DOS_create_psp		0x26
#define	DOS_ran_block_read	0x27
#define	DOS_ran_block_write	0x28
#define	DOS_parse		0x29
#define	DOS_get_date		0x2a
#define	DOS_set_date		0x2b
#define	DOS_get_time		0x2c
#define	DOS_set_time		0x2d
#define	DOS_verify		0x2e
#define	DOS_get_dta		0x2f
#define	DOS_get_version		0x30
#define	DOS_keep_process	0x31
#define	DOS_ctrl_c_dk		0x33
#define	DOS_get_vector		0x35
#define	DOS_get_disk_space	0x36
#define	DOS_get_country		0x38
#define	DOS_set_country		0x38
#define	DOS_make_dir		0x39
#define	DOS_rem_dir		0x3a
#define	DOS_change_dir		0x3b
#define	DOS_create_handle	0x3c
#define	DOS_open_handle		0x3d
#define	DOS_close_handle	0x3e
#define	DOS_read_handle		0x3f
#define	DOS_write_handle	0x40
#define	DOS_delete_entry	0x41
#define	DOS_move_ptr		0x42
#define	DOS_change_mode		0x43
#define	DOS_ioctl_get_data	0x4400
#define	DOS_ioctl_set_data	0x4401
#define	DOS_ioctl_get_char	0x4402
#define	DOS_ioctl_set_char	0x4403
#define	DOS_ioctl_get_status	0x4404
#define	DOS_ioctl_set_status	0x4405
#define	DOS_ioctl_get_block	0x4406
#define	DOS_ioctl_set_block	0x4407
#define	DOS_ioctl_get_change	0x4408
#define	DOS_ioctl_set_rblock	0x4409
#define	DOS_ioctl_get_rhandle	0x440a
#define	DOS_ioctl_set_retry	0x440b
#define	DOS_xdup		0x45
#define	DOS_xdup2		0x46
#define	DOS_get_dir		0x47
#define	DOS_allocate_memory	0x48
#define	DOS_free_memory		0x49
#define	DOS_set_block		0x4a
#define	DOS_exec		0x4b00
#define	DOS_exec_ovl		0x4b03
#define	DOS_end_process		0x4c
#define	DOS_wait		0x4d
#define	DOS_find_first_file	0x4e
#define	DOS_find_next_file	0x4f
#define	DOS_get_verify		0x54
#define	DOS_rename_file		0x56
#define	DOS_get_set_date_time	0x57
#define	DOS_alloc_strat		0x58
#define	DOS_get_error		0x59
#define	DOS_create_temp		0x5a
#define	DOS_create_new		0x5b
#define	DOS_lock		0x5c00
#define	DOS_unlock		0x5c01
#define	DOS_get_machine_name	0x5e00
#define	DOS_printer_setup	0x5e02
#define	DOS_get_list		0x5f02
#define	DOS_redir		0x5f03
#define	DOS_cancel_redir	0x5f04
#define	DOS_get_psp		0x62

/* File attributes */
#define	FA_normal	0x00
#define	FA_read_only	0x01
#define	FA_hidden	0x02
#define	FA_system	0x04
#define	FA_volume_id	0x08
#define	FA_subdirectory	0x10
#define	FA_archive	0x20

#define	FD_date( date )	((date) & 0x001f)
#define	FD_month( date ) (((date) >> 5) & 0x000f)
#define	FD_year( date ) (((date) >> 9) & 0x003f)

#define	FT_seconds( time ) ((time) & 0x001f)
#define	FT_minutes( time ) (((time) >> 5) & 0x003f)
#define	FT_hours( time ) (((time) >> 11) & 0x001f)


struct dos_find_files
	{
	char _find_next_file[21];	/* reserved for find next files use */
	unsigned char attributes;	/* File attributes */
	short int modify_time;		/* Time file was last modified */
	short int modify_date;		/* Date file was last modified */
	long int file_size;		/* Size of the file */
	char file_name[13];		/* name of entry */
	};

struct deviceparams
	{
	unsigned char dpSpecFunc;
	unsigned char dpDevType;
	unsigned short dpDevAttr;
	unsigned short dpSylinders;
	unsigned char dpMediaType;
	unsigned short dpBytesPerSec;
	unsigned char SecPerCluster;
	unsigned short dpResSectors;
	unsigned char dpFATs;
	unsigned short dpRootDirEnts;
	unsigned short dpSectors;
	unsigned char dpMedia;
	unsigned short dpFATsecs;
	unsigned short dpSecPerTrack;
	unsigned short dpHeads;
	unsigned long dpHiddenSecs;
	unsigned long dpHugeSectors;
	};

struct rwblock
	{
	unsigned char rwSpecFunc;
	unsigned short 	rwHead;
	unsigned short 	rwCylinder;
	unsigned short 	rwFirstSector;
	unsigned short 	rwSectors;
	unsigned char *	rwBuffer;
	};

/* end of msdos.h */
