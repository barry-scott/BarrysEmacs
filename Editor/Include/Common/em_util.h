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
    _dbg_fn_trace( const EmacsString &fn_name );
    ~_dbg_fn_trace();

    static int callDepth();
private:
    static int s_call_depth;
    const EmacsString m_fn_name;
    int m_call_depth;
};

