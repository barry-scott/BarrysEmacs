//
//    mem_man.h
//
//        Copyright 1995 Barry A. Scott
//
#define LOOK_ASIDE 1


//
// guard size must ensure that the offsetof(struct heap_entry, user_data)
// is a multiple of sizeof(double) to ensure that double variables are
// aligned correctly
//
const int GUARD_SIZE( 20 );


struct queue
{
    struct queue *next;
    struct queue *prev;
};

struct heap_entry
{
    struct queue q;
    long int user_size;    // use long int to cause machine independant alignment
    enum malloc_block_type user_type;
#ifdef SAVE_ENVIRONMENT
    EmacsObject *theObject;
    int ptr_index;
#endif

#if DBG_ALLOC_CHECK
    // the number of users of this block
    // calculated via a save environment scan
    int ref_count;

    // true after a scan has seen that this is not referenced
    bool lost_entry;

    // The file and line that allocated this block
    const char *fileName;
    int lineNumber;
    // Unique number for this instances of allocating this block
    int sequence_number;
    unsigned char front_guard[GUARD_SIZE];
#endif
    unsigned char user_data[GUARD_SIZE];
};

#ifdef LOOK_ASIDE
const int look_aside_max( 256 );
const int look_aside_granularity( 1<<4 );        // must be a power of 2
const int look_aside_granularity_mask( look_aside_granularity-1 );
#endif

extern struct queue all_emacs_memory;

//
//    This class holds all the information required to save or restore
//    the emacs environment
//
class EmacsSaveRestoreEnvironment : public EmacsObject
{
public:
    EmacsSaveRestoreEnvironment();
    ~EmacsSaveRestoreEnvironment();

    EMACS_OBJECT_FUNCTIONS( EmacsSaveRestoreEnvironment )

    // return 1 is can restore
    // return 0 is cannot restore, not error message required
    // return -1 is cannot restore and an error needs generating
    int canRestore( const EmacsString &rest_fn, EmacsString &full_name, EmacsString &reason );

    void DoSave();
    bool DoRestore();

    typedef unsigned char byte;


    void _serialize_bytes( byte *bytes, int length );
    void _serialize_ptr( void **ptr );
    void _serialize_ptr_indirect( void **ptr );
    bool savingEnvironment() { return saving_environment; }
private:
    void serialize_star_star( void **ptr, int size );
    void serialize_savres_globals(void);
    void serialize_EmacsObject( EmacsObject *o, int size );

    FILE *serialize_file;
    bool saving_environment;
    int allocation_sequence_number_at_construction;
    byte *conversion_buffer;
};

// the one and only EmacsSaveRestoreEnvironment object that is allowed
extern EmacsSaveRestoreEnvironment *EmacsSaveRestoreEnvironmentObject;
