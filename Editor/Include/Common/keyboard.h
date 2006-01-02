//    Copyright (c) 1982-1997
//        Barry A. Scott
// key to procedure mapping table definitions

// Bit mask values for mouse-enable
const int MOUSE_BUTTON_EVENTS( 1 );
const int MOUSE_MOTION_EVENTS( 2 );

const int CHARACTER_QUEUE_SIZE( 4000 );
enum CE_TYPE_type
{
    CE_TYPE_FREE_CELL=0,
    CE_TYPE_CHAR,
    CE_TYPE_PAR_CHAR,
    CE_TYPE_PAR_SEP,
    CE_TYPE_FIN_CHAR
};

class CharElement : public QueueEntry<CharElement>
{
public:
    unsigned char ce_char;
    unsigned char ce_type;
    bool ce_shift;
};
const int M_CS_CVT_CSI( 1 );
const int M_CS_CVT_8BIT( 2 );
const int M_CS_PAR_CHAR( 4 );
const int M_CS_PAR_SEP( 8 );
const int M_CS_FIN_CHAR( 16 );
const int M_CS_INT_CHAR( 32 );

class KeyMapInside : public EmacsObject
{
public:
    KeyMapInside();
    virtual ~KeyMapInside();

    virtual BoundName *getBinding( int c ) = 0;
    virtual BoundName **getBindingRef( int c ) = 0;
    virtual int addBinding( int c, BoundName *b ) = 0;
    virtual void removeBinding( int c ) = 0;
    virtual void removeAllBindings( void ) = 0;
};

class KeyMapShort : public KeyMapInside
{
    friend class KeyMapLong;
public:
    KeyMapShort();
    virtual ~KeyMapShort();
    EMACS_OBJECT_FUNCTIONS( KeyMapShort )

    virtual BoundName *getBinding( int c );
    virtual BoundName **getBindingRef( int c );
    virtual int addBinding( int c, BoundName *b );
    virtual void removeBinding( int c );
    virtual void removeAllBindings( void );

private:
    int k_used;
    enum { KEYMAP_SHORT_SIZE = 12 };

    unsigned char k_chars [KEYMAP_SHORT_SIZE];
    BoundName *k_sbinding [KEYMAP_SHORT_SIZE];
};

class KeyMapLong : public KeyMapInside
{
public:
    KeyMapLong( KeyMapShort *smap );
    virtual ~KeyMapLong();
    EMACS_OBJECT_FUNCTIONS( KeyMapLong )

    virtual BoundName *getBinding( int c );
    virtual BoundName **getBindingRef( int c );
    virtual int addBinding( int c, BoundName *b );
    virtual void removeBinding( int c );
    virtual void removeAllBindings( void );

private:
    BoundName *k_binding[256];
};

class KeyMap : public EmacsObject
{
    friend class KeyMapShort;
    friend class KeyMapLong;
public:
    EMACS_OBJECT_FUNCTIONS( KeyMap )
    KeyMap( const EmacsString &name );
    virtual ~KeyMap();

    BoundName *getBinding( int c );
    BoundName **getBindingRef( int c );
    void addBinding( int c, BoundName *b );
    void removeBinding( int c );
    void removeAllBindings( void );

    EmacsString k_name;
private:
    KeyMapInside *k_map;
};

const int MEMLEN( 1002 );                     // (MemLen+2) % 4  must equal 0

class EmacsWorkItem : public QueueEntry<EmacsWorkItem>
{
public:
    EMACS_OBJECT_FUNCTIONS( EmacsWorkItem )
    virtual void workAction(void) = 0;
    void addItem(void);
    static void processQueue(void);
    static bool enableWorkQueue(void) { return enabled; }
    static bool enableWorkQueue( bool enable);
private:
    static QueueHeader<EmacsWorkItem> work_queue;
    static bool enabled;
    static int queued_while_disabled;
};
