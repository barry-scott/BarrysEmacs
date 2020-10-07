//    Copyright (c) 1982-2010
//        Barry A. Scott
// key to procedure mapping table definitions
//
#include <map>
#include <vector>

// Bit mask values for mouse-enable
const int MOUSE_BUTTON_EVENTS( 1 );
const int MOUSE_MOTION_EVENTS( 2 );

const int CHARACTER_QUEUE_SIZE( 4000 );
enum CE_TYPE_type
{
    CE_TYPE_FREE_CELL=0,
    CE_TYPE_DO_DSP,     // Signal that do_dsp() needs to be called

    CE_TYPE_CHAR,

    CE_TYPE_PAR_CHAR,   // for parsed ESC/CSI sequences
    CE_TYPE_PAR_SEP,    // for parsed ESC/CSI sequences
    CE_TYPE_FIN_CHAR,   // for parsed ESC/CSI sequences

    CE_TYPE_PARM_LIST_FIN_CHAR
};

class CharElement : public QueueEntry<CharElement>
{
public:
    void set( EmacsChar_t ch, CE_TYPE_type type, bool shift )
    {
        ce_char = ch;
        ce_type = type;
        ce_shift = shift;
        ce_all_params.clear();
    }

    void set( EmacsChar_t ch, CE_TYPE_type type, bool shift, std::vector<int> all_params )
    {
        ce_char = ch;
        ce_type = type;
        ce_shift = shift;
        ce_all_params = all_params;
    }

    ~CharElement()
    {}

    EmacsChar_t         ce_char;
    CE_TYPE_type        ce_type;
    bool                ce_shift;
    std::vector<int>    ce_all_params;
};

const int M_CS_CVT_CSI( 1 );
const int M_CS_CVT_8BIT( 2 );
const int M_CS_PAR_CHAR( 4 );
const int M_CS_PAR_SEP( 8 );
const int M_CS_FIN_CHAR( 16 );
const int M_CS_INT_CHAR( 32 );

typedef std::map< EmacsChar_t, BoundName * >    EmacsCharToBoundName_t;

class KeyMap;
class ScanMapHistory;

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

    void scan_map(
        void (*proc)( BoundName *b, EmacsString &keys, int range ),
        ScanMapHistory *history,
        const EmacsString &keys,        // get a copy not a ref - important
        bool fold_case );

    EmacsString k_name;

    void q() const;

    class const_iterator
    {
        friend class KeyMap;
    public:
        const_iterator( const const_iterator &it );

        EmacsChar_t firstChar() const   { return m_first_char; }
        int length() const              { return m_length; }
        BoundName *boundName() const    { return m_bound_name; }

        bool operator==( const const_iterator &other ) const;
        bool operator!=( const const_iterator &other ) const;
        const_iterator &operator++();

    private:
        enum state_t
        {
            state_init,
            state_default_binding,  // returning from k_default_binding
            state_iterator,         // returning from k_binding
            state_end               // no more keys
        };
        const_iterator( const KeyMap &map );
        const_iterator( const KeyMap &map, state_t state );

    private:
        // iterator state
        state_t                                 m_state;
        const KeyMap                            &m_keymap;
        EmacsCharToBoundName_t::const_iterator  m_it;
        // value of iterator
        EmacsChar_t                             m_first_char;
        int                                     m_length;
        BoundName                               *m_bound_name;
    };

    const_iterator begin() const;
    const_iterator end() const;

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
