//
//
//    em_util.h
//
//


//
//
//    The Save classes make a copy of there arg and
//    restore the arg on destruction
//
//
template <class type>
class Save
{
public:
    Save( type *v ) : from( v ) , value( *v ) { }
    ~Save() { *from = value; }
private:
    type *from;
    type value;
};


class _dbg_fn_trace
{
public:
    _dbg_fn_trace( const EmacsString &fn_name, bool enabled );
    ~_dbg_fn_trace();

    void _msg( const EmacsString &msg );
    void _result( const EmacsString &results );

    static int callDepth();
private:
    const bool m_enabled;
    static int s_call_depth;
    const EmacsString m_fn_name;
    EmacsString m_result;
    int m_call_depth;
};

