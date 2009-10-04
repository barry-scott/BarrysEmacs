//    Copyright (c) 1982-2002
//        Barry A. Scott

// #define DEBUG_SET_BF

//   structure that defines a marker
//   A marker is conceptually a (buffer,position) pair.  m_buf
//   indicates which buffer is marked, and m_pos indicates which
//   position is marked.  All markers for a particular buffer are
//   chained together by m_next, rooted at b_markset.  The
//   interpretation if m_pos is rather odd: it is the index from the
//   beginning of the allocated area (b_base) of the marked position
//   -- it is not the character number of the marked position.  This
//   interpretation causes markers to be invariant over insertions and
//   deletions, the only things that affect them are gap motions,
//   which should be far less frequent.
//
class Marker : public QueueEntry<Marker>
{
public:
    EMACS_OBJECT_FUNCTIONS( Marker )
    Marker();
    Marker( EmacsBuffer *b, int p, int right=0 );
    Marker( const Marker &src );
    virtual ~Marker();

    Marker &operator=( const Marker & );

    int to_mark(void) const;
    int get_mark(void) const;
    void set_mark( EmacsBuffer *b, int p, int right=0 );
    void set_mark( const Marker &old_mark );
    void unset_mark(void);
    bool isSet() const { return m_buf != NULL; }
    EmacsString asString() const;
private:
    void delink_mark(void);
public:
    EmacsBuffer *m_buf;        //  0 the buffer that this marker refers to
    int m_pos;            //  4 the position in the buffer of the character referred to
    unsigned m_right : 1;        // 10 true iff this is a right handed marker
    unsigned m_modified : 1;    // 10 true iff this marker has been modified since it was set
};

class ModeSpecific : public EmacsObject
{
public:
    enum { MODESTRING_SIZE = 80 };
    enum { MODELINE_SIZE = 256 };    // same as MScreenWidth rounded to longwords
    ModeSpecific();
    virtual ~ModeSpecific();
    EMACS_OBJECT_FUNCTIONS( ModeSpecific )

    ModeSpecific &operator=( ModeSpecific & );

    AbbrevTable *md_abbrev;        // the abbrev table in use in this buffer
    SyntaxTable *md_syntax;        // the syntax table in use in this buffer
    int md_rightmargin;        // Right margin for auto-newline
    int md_leftmargin;        // Left margin for auto-newline
    int md_commentcolumn;        // Comment column for auto-newline
    int md_tabsize;            // Size of one tab stop. 8 is the default
    int md_indent_use_tab;        // True if tab chars can be used by indenting functions
    int md_headclip;        // The number of characters clipped off the head of the buffer by restrict-region +1
    int md_tailclip;        // The number of characters clipped off the tail of the buffer by restrict-region
    KeyMap *md_keys;        // Keys that are bound local to this buffer (stuff like $J)
    int md_abbrevon;        // true iff abbrev mode has been enabled for this buffer
    int md_foldcase;        // true iff case folded comparisons are to be done
    int md_replace;            // true iff we replace instead of insert characters
    int md_highlight;        // true if this buffers region should be highlighted
    int md_displaynonprinting;    // true if non printing chars are to shown up as printable chars
    int md_display_eof;        // true if the EOF diamond is to be shown
    int md_display_c1;        // true if the C1 characters in the buffer are to be show as graphics
    int md_readonly;        // true if the buffer is read-only
    int md_wrap_lines;        // true if long lines should be wrapped for this buffer
    int md_syntax_colouring;    // true if syntax colouring is enabled
    int md_syntax_array;        // true if the sytax array methods are to be used
    EmacsString md_modestring;    // The commentary string that appears in the modeline of each window
    EmacsString md_modeformat;    // The format of the mode line for this buffer
    EmacsString md_prefixstring;    // The auto-newline prefix string
    BoundName *md_auto_fill_proc;    // The command that will be executed
                    // when the right margin is passed
    BoundName *md_process_key_proc;    // Process key proc
};

    // the "kinds" of stuff that can be in a buffer
enum buffer_types
{
    FILEBUFFER,        // Contains info from a file
                // (WriteModifiedFiles will dump it)
    SCRATCHBUFFER,        // Scratch stuff -- automatically generated
                // by emacs for stuff like ^X^B
    MACROBUFFER        // contains the body of a macro, in which
                // case the file name is actually the macro name
};

class RenditionRegion : public EmacsObject
{
public:
    RenditionRegion( int start, int end, int colour );
    virtual ~RenditionRegion();
    EMACS_OBJECT_FUNCTIONS( RenditionRegion )

    RenditionRegion *rr_next;
    Marker rr_start_mark;
    Marker rr_end_mark;
    int rr_start_pos, rr_end_pos;
    int rr_colour;
};

class syntax_buffer_data
{
public:
    syntax_buffer_data( EmacsBuffer & );
    ~syntax_buffer_data();
    bool initBuffer();
    int syntax_valid;        //  The number of valid positions in the
                    //  syntax array
    int syntax_update_credit;    //  number of times the array may be updated
    SyntaxCharData_t *syntax_base;    //  points to the beginning of the syntax
                    //  data for the buffer
                    //  maps 1-to-1 on the b_base array
private:
    syntax_buffer_data();        // hide default constructor

    EmacsBuffer &buffer;
};

class BufferNameTable : public EmacsStringTable
{
public:
    EMACS_OBJECT_FUNCTIONS( BufferNameTable )
    BufferNameTable( int init, int grow )
        : EmacsStringTable( init, grow )
    { }
    virtual ~BufferNameTable()
    { }

    void add( const EmacsString &key, EmacsBuffer *value )
    { EmacsStringTable::add( key, value ); }
    EmacsBuffer *remove( const EmacsString &key )
    { return (EmacsBuffer *)EmacsStringTable::remove( key ); }
    EmacsBuffer *find( const EmacsString &key )
    { return (EmacsBuffer *)EmacsStringTable::find( key ); }
    EmacsBuffer *value( int index )
    { return (EmacsBuffer *)EmacsStringTable::value( index ); }
};

class EmacsBuffer : public EmacsObject
{
    friend class Marker;
private:
    enum { InitialBufferSize = 2048};        // how big a buffer is when created
    enum { BufferSizeIncrement = 2048};        // how much extra space to add when increasing the size of a buffer
    EmacsBuffer();
public:
    EMACS_OBJECT_FUNCTIONS( EmacsBuffer )
    EmacsBuffer( const EmacsString &name );
    virtual ~EmacsBuffer();

    // set this as the current buffer
#ifdef DEBUG_SET_BF
    void __set_bf( const char *__file, int __line );
#define set_bf() __set_bf( __FILE__, __LINE__ )
#else
    void set_bf();
#endif

    // calculate the column at buf_pos
    int calculateColumn( int buf_pos );

    // set the marker of the buffer
    void set_mark( int pos, int right, bool gui );
    // unset the marker of the buffer
    void unset_mark();

    // find a buffer by name
    static EmacsBuffer *find( const EmacsString &name );

    // return one of the keys in the table otherwise NULL
    static EmacsBuffer *get_word_mlisp()
    {
        EmacsString result;
        return find( name_table.get_word_mlisp( result ) );
    }
    static EmacsBuffer *get_word_interactive( const EmacsString &prompt )
    {
        EmacsString result;
        return find( name_table.get_word_interactive( prompt, EmacsString::null, result ) );
    }
    static EmacsBuffer *get_word_interactive( const EmacsString &prompt, const EmacsString &default_value )
    {
        EmacsString result;
        return find( name_table.get_word_interactive( prompt, default_value, result ) );
    }

    // return one of the keys in the table otherwise NULL
    static EmacsString &get_esc_word_mlisp( EmacsString &result )
    { return name_table.get_esc_word_mlisp( result ); }
    static EmacsString &get_esc_word_interactive( const EmacsString &prompt, EmacsString &result )
    { return name_table.get_esc_word_interactive( prompt, EmacsString::null, result ); }
    static EmacsString &get_esc_word_interactive( const EmacsString &prompt, const EmacsString &default_value, EmacsString &result )
    { return name_table.get_esc_word_interactive( prompt, default_value, result ); }

    // set the current buffer by name
    static void set_bfn( const EmacsString &name );

    // create a new scratch buffer by name
    static void scratch_bfn( const EmacsString &name, int disp );

    void erase_bf();

    // true if there is a buffer restriction in effect
    bool isRestricted() const;

    void gap_to(int n);
    void gap_outside_of_range( int left, int right );
    int gap_room(int k);

    // Write file operation types

    enum WriteFileOperation_t
    {
        APPEND_WRITE = 2,                   // Appends to a file
        CHECKPOINT_WRITE = 1,           // Writes a checkpoint file
        ORDINARY_WRITE = 0,           // Writes an ordinary file
        MIN_WRITEFILE = ORDINARY_WRITE,
        MAX_WRITEFILE = APPEND_WRITE
    };


    int read_file( const EmacsString &fn, int erase, int createnew );
    int write_file( const EmacsString &fn, WriteFileOperation_t appendit );


    inline EmacsChar_t char_at( int n )
    {
        if( n > b_size1 )
            return b_base[b_gap+n-1];
        else
            return b_base[n-1];
    }

    inline EmacsChar_t *ref_char_at( int n )
    {
        if( n > b_size1 )
            return &b_base[b_gap+n-1];
        else
            return &b_base[n-1];
    }

    inline bool char_is( int ch, int prop )
    {
        return (b_mode.md_syntax->s_table[ch].s_kind & prop) != 0;
    }

    inline bool char_at_is( int pos, int prop )
    {
        return (b_mode.md_syntax->s_table[char_at( pos )].s_kind & prop) != 0;
    }

    inline SyntaxData_t syntax_at( int n )
    {
        if( n > b_size1 )
            return (SyntaxData_t)b_syntax.syntax_base[b_gap+n-1].data;
        else
            return (SyntaxData_t)b_syntax.syntax_base[n-1].data;
    }

    inline bool syntax_at_is( int n, int prop )
    {
        return (syntax_at( n ) & prop) != 0;
    }

    inline void set_syntax_at( int n, SyntaxData_t d )
    {
        if( n > b_size1 )
            b_syntax.syntax_base[b_gap+n-1].data = d;
        else
            b_syntax.syntax_base[n-1].data = d;
    }

    inline void set_syntax_at( int n, SyntaxData_t d, SyntaxData_t o )
    {
        if( n > b_size1 )
        {
            b_syntax.syntax_base[b_gap+n-1].data = d;
            b_syntax.syntax_base[b_gap+n-1].outline_depth = o;
            b_syntax.syntax_base[b_gap+n-1].outline_visible = 1;
        }
        else
        {
            b_syntax.syntax_base[n-1].data = d;
            b_syntax.syntax_base[n-1].outline_depth = o;
            b_syntax.syntax_base[b_gap+n-1].outline_visible = 1;
        }
    }


    bool syntax_fill_in_array( int required );
    void syntax_update_buffer( int pos, int len );

    void insert_at( int n, EmacsChar_t c );
    void ins_cstr( const EmacsString &s );
    void ins_cstr( const EmacsChar_t *s, int n );
    void ins_cstr( const char *s, int n );
    void insert_buffer( EmacsBuffer *buf );
    void del_frwd( int n, int k );
    void del_back( int n, int k );

    inline void ins_str( const unsigned char *insstr_str )
    {
        ins_cstr( insstr_str, strlen( (const char *)insstr_str ) );
    }

    inline void ins_str( const char *insstr_str )
    {
        ins_cstr( (const unsigned char *)insstr_str, strlen( insstr_str ) );
    }



    void bufferExtent( EmacsChar_t *&p1, int &s1, EmacsChar_t *&p2, int &s2 );

    inline int first_character()
    {
        return b_mode.md_headclip;
    }

    inline int num_characters()
    {
        return b_size1 + b_size2 - b_mode.md_tailclip;
    }

    int restrictedSize() { return num_characters() - first_character() + 1; }
    int unrestrictedSize() { return b_size1 + b_size2; }
    int allocatedSize() { return b_size1 + b_size2 + b_gap; }

//private:
    void saveGlobalState();
    void restoreGlobalState();


    // An Emacs buffer is maintained as a single block of storage that
    // contains all of the text involved (eg. the entire contents of a
    // file, we're depending on the paging system to do a lot of work
    // for us).  This block is divided into two parts, which when
    // concatenated form one long text string.  The gap in the middle is
    // to allow insertions and deletions to be performed without
    // repeated copying of the entire buffer contents.  "dot" will not
    // necessarily be positioned at the gap, but if any insertions or
    // deletions are to be done around "dot" then the gap must be moved.
    //
    // |<---------------size---------------------------------->|
    // |<---b_size1--->|<---b_gap--->|<---b_size2--->|
    // ^--b_base

private:
    EmacsChar_t *b_base;        //  0 points to the beginning of the
                    //    block of storage used to hold the
                    //    text in the buffer
    int b_size1;            //  c the number of characters in the
                    //    first part of the block
    int b_gap;            // 10 the number of characters in the
                    //    gap between the two parts
    int b_size2;            // 14 the number of characters in the
                    //    second part of the block
public:
    syntax_buffer_data b_syntax;
    int b_line_valid;        //  c The highwater mark for the line number
                    //    calculations
    RenditionRegion *b_rendition_regions;
    EmacsString b_buf_name;        //  4 the name of this buffer
    int b_size;            //  8 the number of characters in the
                    //    block pointed to by b_base.  Not
                    //    all of the characters in the
                    //    block may be valid
    int b_ephemeral_dot;        // 18 The value that dot had the last time that
                    //    this buffer was visible in a window or
                    //    accessed in any way
    EmacsString b_fname;        // 1c the name of the file associated
                    //    with this buffer
    int b_modified;            // 20 true iff this buffer has been
                    //    modified since it was last
                    //    written
    int b_backed_up;        // 24 true iff this buffer has been been backed
                    //    up (if you write to its associated file
                    //    and it hasn't been backed up, then a
                    //    backup will be made)
    int b_checkpointed;        // 28 the value of b_modified at the last
                    //    check_point.  Since b_modified is actually
                    //    a count of the number of changes made
                    //    (which gets zeroed when the file is
                    //    written), deciding whether or not to
                    //    check_point is done on the basis of the
                    //    difference between b_modified and
                    //    b_checkpointed
    EmacsString b_checkpointfn;    // 2c file name used for check_pointing this buffer
    EmacsBuffer *b_next;        // 30 the next buffer in the chain of
                    //    extant buffers
    QueueHeader<Marker> b_markset;    // 34 the markers that refer to this buffer
    Marker b_mark;            // 38 The distinguished mark (set by
                    //    ^@) for this buffer
    bool b_gui_input_mode_set_mark;
    enum buffer_types b_kind;    // 3c The kind of thing in this buffer
    ModeSpecific b_mode;        // 40 The mode specific information for this buffer
    FIO_RMS_Attribute b_rms_attribute;        // the RMS attributes of the file in this buffer
    time_t b_file_time;        // the modification time of the file
    time_t b_synch_file_time;    // the modification time of the file at last synch
    int b_file_access;        // file access
    int b_synch_file_access;    // synch file access
    EmacsBufferJournal *b_journal;
    unsigned int b_journalling : 1;        // True if journalling

    static BufferNameTable name_table;
};

class EmacsBufferRef : public QueueEntry<EmacsBufferRef>
{
    friend class EmacsBuffer;
public:
    EmacsBufferRef( EmacsBuffer *buf );
    virtual ~EmacsBufferRef();

    EmacsBuffer *buffer( EmacsBuffer *buf ) { buffer_pointer = buf; return buf; }
    EmacsBuffer *buffer() { return buffer_pointer; }
    operator EmacsBuffer *() { return buffer_pointer; }
    operator const EmacsBuffer *() { return buffer_pointer; }
    EmacsBuffer *operator->() { return buffer_pointer; }
    EmacsBuffer &operator*() { return *buffer_pointer; }
    bool bufferValid() { return buffer_pointer != NULL; }
#ifdef DEBUG_SET_BF
    void __set_bf( const char *__file, int __line );
#else
    void set_bf();
#endif

private:
    static void markDeletedBuffer( EmacsBuffer *buf );

    EmacsBuffer *buffer_pointer;

    static QueueHeader<EmacsBufferRef> header;
};
