//	Copyright (c) 1982-2001
//	Barry A. Scott


class EmacsWindow;
class EmacsWindowGroup;
class EmacsView;
//
// structure that defines a window
//
class EmacsWindowGroup;
class EmacsWindow : public EmacsObject
	{
	friend class EmacsWindowGroup;
public:
	EMACS_OBJECT_FUNCTIONS( EmacsWindow )
	EmacsWindow( EmacsWindowGroup &group );
	EmacsWindow( EmacsWindow &w, EmacsWindowGroup &group );
	~EmacsWindow();

	inline bool isCurrentWindow();

	//
	// windows are organized in a double linked list.  Each window has 
	// its own value of 'dot' and is tied to some buffer 
	//
	EmacsWindow *w_next;            // next window 
	EmacsWindow *w_prev;            // preceeding window 
	EmacsWindow *w_right;           // preceeding window 
	EmacsWindow *w_left;            // preceeding window 
	EmacsBuffer *w_buf;		// buffer tied to this window 
	int w_height;			// number of screen lines devoted to this window; includes space for the mode line 
	int w_width;			// number of columns used 
	int w_lastuse;			// sequence counter for LRU window finding 
	int w_force;			// true iff the value of start MUST be used on the next redisplay 
	int w_horizontal_scroll;	// horizontal offset into start  of winodw 
	bool w_user_horizontal_scrolled;// true if the scroll bars where used to since last update

	//
	//	public function
	//
	EmacsWindow *split_win_horz();
	EmacsWindow *split_win_vert();
	void set_win();				// called when the user sets the window
	void set_win_internal();		// called when emacs needs to changes buffers
	void tie_win( EmacsBuffer *b );

	int change_w_height( int delta, int dir );
	int change_w_width( int delta, int dir );
	int dump_line_from_buffer
		(
		bool is_users_current_window,
		int line_start_n,
		int sline,
		int scolumn,
		int initial_column,
		bool *dot_found,
		int *dot_column,
		bool &line_wrapped,
		ModeSpecific &_mode
		);

	//
	//	Get and Set
	//
	void setWindowStart( int pos );
	void unsetWindowStart();
	int getWindowStart();

	void setWindowDot( const Marker &mark );
	void unsetWindowDot();
	const Marker &getWindowDot() const;

	void setWindowMark( const Marker &mark, bool gui_input_mode_set_mark );
	void unsetWindowMark();
	const Marker &getWindowMark() const;
	bool getWindowGuiInputModeSetMark() const;

	EmacsString getDescription() const;
private:
	const int w_window_id;		// used for diagnostic messages
	static int last_used_window_id;
	void dump_mode( int line, int col );
	// return true if dot is missed in the current window
	bool dump_win( bool is_users_current_window, int line, int col, bool can_move );

	Marker w_start_;		// start character position in the tied buffer of the display 
	Marker w_dot_;			// value of "dot" for this window 
	Marker w_mark_;			// value of "mark" for this window if it is not current
	bool w_gui_input_mode_set_mark_;// true if gui input mode set the mark
	EmacsWindowGroup *group;
	};


class EmacsWindowGroup : public QueueEntry<EmacsWindowGroup>
	{
	friend class SavedEmacsView;
	friend class EmacsWindow;
public:
	EMACS_OBJECT_FUNCTIONS( EmacsWindowGroup )
	virtual ~EmacsWindowGroup();
	EmacsWindowGroup( EmacsView *_view );
	EmacsWindowGroup( EmacsWindowGroup & );
	EmacsWindowGroup &operator=( EmacsWindowGroup & );

	EmacsWindow *currentWindow() { return current_window; }
	static void de_ref_buf( EmacsBuffer *b );
	void del_win( EmacsWindow *w );

	static void init(void);		// one off init code
	static void restore(void);	// restore code

	int full_upd( int &cant_ever_opt );
	void fit_windows_to_screen( EmacsView *new_view );

	// set the group up as the active view
	void set_current_windows( EmacsView *new_view, EmacsWindowGroup &group );

	EmacsWindow *split_largest_window( void );
	EmacsWindow *LRU_win( void );
	int window_on( EmacsBuffer *bf );

	void do_dsp();

	int change_window_height(int delta);
	int change_window_width(int delta);

	EmacsWindow *windows;		// the first window in the group
	EmacsView *view;		// the guy that displays us
private:
	// special constructure used by save enviroment to empty out a group
	EmacsWindowGroup( SavedEmacsView &save );

	void derefBufferForOneWindowGroup( EmacsBuffer *b );
	void deleteWindows();	// delete all the windows in the group
	void copyWindows( EmacsWindowGroup &from );	// copy all the windows in from to us

	Marker one_line_start;		// starting character position of the line
					// containing dot -- used when doing the one
					// line redisplay optimization.
	bool one_line_valid;		// True iff OneLStart points at something valid
	int one_line_line;		// the display line which contains dot
	int MB_line;			// The line on which the minibuf starts

	EmacsWindow *current_window;	// the current window of the group
	Marker current_buffer;		// the current buffer of the group
	int buffer_is_visible : 1;	// true if the current buffer is visible

	static QueueHeader<EmacsWindowGroup> header;
	};

inline 	bool EmacsWindow::isCurrentWindow() { return this == group->currentWindow(); }

//
// structure that defines a view to a set of windows
//
class EmacsView;
extern EmacsView *theActiveView;

class EmacsView : public TerminalControl, public QueueEntry<EmacsView>
	{
public:
	EMACS_OBJECT_FUNCTIONS( EmacsView )
	EmacsView();
	virtual ~EmacsView();

	int window_on( EmacsBuffer *bf )
		{ return windows.window_on( bf ); }
	EmacsWindow *currentWindow()
		{ return windows.currentWindow(); }
	void fit_windows_to_screen( void )
		{ windows.fit_windows_to_screen( this ); }
	void do_dsp()
		{ windows.do_dsp(); }
	int full_upd( int &cant_ever_opt )
		{ return windows.full_upd( cant_ever_opt ); }
	void set_current_windows( EmacsWindowGroup &group )
		{ windows.set_current_windows( this, group ); }

	void update_screen( int slow_update );
	void setpos( int row, int col );
	void clearline( int row );
	void copyline(int row);
	void dump_str
		(
		bool is_users_current_window,
		const unsigned char *s,
		int limit,
		int line, int column,
		int highlight
		);

	int dump_line_from_buffer
		(
		EmacsWindow *window,
		bool is_users_current_window,
		int line_start_n,
		int sline,
		int scolumn,
		int initial_column,
		bool *dot_found,
		int *dot_column,
		bool &_line_wrapped,
		ModeSpecific &_mode
		);

	void dsputc( int c, int a );

	int curs_x;			// X and Y coordinates of the cursor
	int curs_y;			// between updates.
	int current_line;		// current line for writing to the virtual screen.
	int columns_left;		// number of columns left on the current line
					// of the virtual screen.
	DisplayBody_t *text_cursor;	// pointer into a line object, indicates where
	DisplayAttr_t *attr_cursor;	// to put the next character

	int window_size;		// the number of lines on which line Insert Delete
					// operations should be done
	struct screen_update_command_t
		{
		enum {idc_done,idc_update_line, idc_scroll_up,idc_scroll_down} idc_command;
		int idc_upper_edge;
		int idc_lower_edge;
		int idc_amount;
		int idc_old_line;
		int idc_new_line;
		};
private:
	screen_update_command_t screen_update_commands[128];
	int num_screen_update_commands;

	void executeInsertDelete();
#ifdef MAGIC_SQUARE
	void calc_matrix( void );
	void calc_insert_delete( int i, int j, int inserts_desired );
	void appendUpdateLineCommand( int old_line, int new_line );
	void appendScrollZone( int zone_start_new_line_num, int zone_start_old_line_num, int win_size );
	void appendScrollUpCommand( int upper_edge, int lower_edge, int amount );
	void appendScrollDownCommand( int upper_edge, int lower_edge, int amount );

	void executeStoredScreenUpdateCommands( void );
#endif

	void updateLine( EmacsLinePtr old_line, EmacsLinePtr new_line, int ln );
	void moveLine( int old_line_num, int new_line_num );

#ifdef DBG_CALC_INS_DEL
	void dbg_dump_screen( const char *title );
#endif
public:
	EmacsWindowGroup windows;

private:
	friend class EmacsSaveRestoreEnvironment;

	static QueueHeader<EmacsView> header;
	};

class EmacsWindowRing : public EmacsObject
	{
public:
	EMACS_OBJECT_FUNCTIONS( EmacsWindowRing )
	EmacsWindowRing( EmacsWindowGroup &group );
	virtual ~EmacsWindowRing();

	EmacsWindowGroup windows;
	EmacsWindowRing *wr_pushed;	// pointer to pushed ring 
	int wr_ref_count;		// number of referances to this ring 
	};


//
//	Default window element colours
//
extern const char *mode_line_rendition_default;
extern const char *region_rendition_default;
extern const char *window_rendition_default;
extern const char *syntax_keyword1_rendition_default;
extern const char *syntax_keyword2_rendition_default;
extern const char *syntax_keyword3_rendition_default;
extern const char *syntax_word_rendition_default;
extern const char *syntax_string1_rendition_default;
extern const char *syntax_string2_rendition_default;
extern const char *syntax_string3_rendition_default;
extern const char *syntax_quote_rendition_default;
extern const char *syntax_comment1_rendition_default;
extern const char *syntax_comment2_rendition_default;
extern const char *syntax_comment3_rendition_default;
extern const char *user_1_rendition_default;
extern const char *user_2_rendition_default;
extern const char *user_3_rendition_default;
extern const char *user_4_rendition_default;
extern const char *user_5_rendition_default;
extern const char *user_6_rendition_default;
extern const char *user_7_rendition_default;
extern const char *user_8_rendition_default;
