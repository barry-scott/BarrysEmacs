//    Copyright (c) 1982-1997
//        Barry A. Scott
// key to procedure mapping table definitions
//
#include <map>

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
    EmacsChar_t ce_char;
    int ce_type;
    bool ce_shift;
};
const int M_CS_CVT_CSI( 1 );
const int M_CS_CVT_8BIT( 2 );
const int M_CS_PAR_CHAR( 4 );
const int M_CS_PAR_SEP( 8 );
const int M_CS_FIN_CHAR( 16 );
const int M_CS_INT_CHAR( 32 );

typedef std::map< EmacsChar_t, BoundName * >    EmacsCharToBoundName_t;

class KeyMap : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( KeyMap )
    KeyMap( const EmacsString &name );
    virtual ~KeyMap();

    BoundName *getBinding( EmacsChar_t c );
    BoundName **getBindingRef( EmacsChar_t c );
    void addBinding( EmacsChar_t c, BoundName *b );
    void removeBinding( EmacsChar_t c );
    void removeAllBindings( void );

    EmacsString k_name;
private:
    
    BoundName *k_default_binding;
    EmacsCharToBoundName_t k_binding;
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
