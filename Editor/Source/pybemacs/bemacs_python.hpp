//
//    bemacs_python.cpp
//
//    Copyright (c) 1999-2010 Barry A. Scott
//
#include "CXX/Objects.hxx"
#include "CXX/Extensions.hxx"

extern Expression convertPyObjectToEmacsExpression( const Py::Object &obj );
extern Py::Object convertEmacsExpressionToPyObject( const Expression &expr );

class BemacsVariables: public Py::PythonExtension<BemacsVariables>
{
// static methods
public:
    static void init_type();

// member methods
public:
    BemacsVariables();
    virtual ~BemacsVariables();

private:
    virtual Py::Object getattr( const char *c_name );
    virtual int setattr( const char *c_name, const Py::Object &value );
};


//================================================================================
//
//
//    BemacsFunctions type - allows access to all emacs MLisp variables
//
//
//================================================================================
class BemacsFunctions: public Py::PythonExtension<BemacsFunctions>
{
// static methods
public:
    static void init_type();

// member methods
public:
    BemacsFunctions();
    virtual ~BemacsFunctions();

private:
    static PyObject *call_bemacs_function( PyObject *self, PyObject *_args );
    virtual Py::Object getattr( const char *c_name );
    virtual int setattr( const char *c_name, Py::Object value );
};

//================================================================================
//
//
//    BemacsBufferData type
//
//
//================================================================================
class BemacsBufferData: public Py::PythonExtension<BemacsBufferData>
{
// static methods
public:
    static void init_type();

// member methods
public:
    BemacsBufferData( EmacsBuffer *_buffer );
    virtual ~BemacsBufferData();

private:
    EmacsBufferRef buffer;

    virtual Py::Object repr();

    virtual Py::Object getattr( const char *c_name );
    virtual int setattr( const char *c_name, Py::Object value );

    //--------------------------------------------------------------------------------
    //
    // Sequence methods
    //
    //--------------------------------------------------------------------------------
    virtual int sequence_length();
    virtual Py::Object sequence_concat( const Py::Object & );
    virtual Py::Object sequence_repeat( int );
    virtual Py::Object sequence_item( int position );
    virtual Py::Object sequence_slice( int start, int end );
    virtual int sequence_ass_item( int, const Py::Object & );
    virtual int sequence_ass_slice( int, int, const Py::Object & );
};


//================================================================================
//
//
//    BemacsBufferSyntax type
//
//
//================================================================================
class BemacsBufferSyntax: public Py::PythonExtension<BemacsBufferSyntax>
{
// static methods
public:
    static void init_type();

// member methods
public:
    BemacsBufferSyntax( EmacsBuffer *_buffer );
    virtual ~BemacsBufferSyntax();

private:
    EmacsBufferRef buffer;

    virtual Py::Object repr();
    virtual Py::Object getattr( const char *c_name );
    virtual int setattr( const char *c_name, Py::Object value );

    //--------------------------------------------------------------------------------
    //
    // Sequence methods
    //
    //--------------------------------------------------------------------------------
    virtual int sequence_length();
    virtual Py::Object sequence_concat( const Py::Object & );
    virtual Py::Object sequence_repeat( int );
    virtual Py::Object sequence_item( int position );
    virtual Py::Object sequence_slice( int start, int end );
    virtual int sequence_ass_item( int, const Py::Object & );
    virtual int sequence_ass_slice( int, int, const Py::Object & );
};

//================================================================================
//
//
//    BemacsMarker type
//
//
//================================================================================
class BemacsMarker: public Py::PythonExtension<BemacsMarker>
{
// static methods
public:
    static void init_type();

// member methods
public:
    BemacsMarker( Marker *_marker );
    virtual ~BemacsMarker();

    Expression value();

private:
    Marker marker;

    virtual Py::Object repr();
    virtual Py::Object getattr( const char *c_name );
    virtual int setattr( const char *c_name, Py::Object /*value*/ );
    Py::Object as_tuple( const Py::Tuple &args );
};

//================================================================================
//
//
//    BemacsWindowRing type
//
//
//================================================================================
class BemacsWindowRing: public Py::PythonExtension<BemacsWindowRing>
{
// static methods
public:
    static void init_type();

// member methods
public:
    BemacsWindowRing( const Expression &_window_ring );
    virtual ~BemacsWindowRing();

    Expression value();

private:
    // use an express as it takes care of the reference counting of windows rings
    Expression window_ring;

    virtual Py::Object repr();
    virtual Py::Object getattr( const char *c_name );
    virtual int setattr( const char *c_name, const Py::Object &/*value*/ );
};

//================================================================================
//
//
//    BemacsArray type
//
//
//================================================================================
class BemacsArray: public Py::PythonExtension<BemacsArray>
{
// static methods
public:
    static void init_type();

// member methods
public:
    BemacsArray( const Expression &_array );
    virtual ~BemacsArray();

    Expression value();

private:
    // use an express as it takes care of the reference counting of windows rings
    Expression array;

    virtual Py::Object repr();
    virtual Py::Object getattr( const char *c_name );
    virtual int setattr( const char *c_name, const Py::Object &/*value*/ );
};

//================================================================================
//
//
//    BemacsBuffersDict type
//
//
//================================================================================
class BemacsBuffersDict: public Py::PythonExtension<BemacsBuffersDict>
{
// static methods
public:
    static void init_type();

// member methods
public:
    BemacsBuffersDict();
    virtual ~BemacsBuffersDict();

private:
    virtual Py::Object getattr( const char *c_name );

    Py::Object keys( const Py::Tuple &args );

    // Mapping
    virtual int mapping_length();
    virtual Py::Object mapping_subscript( const Py::Object &key );
    virtual int mapping_ass_subscript( const Py::Object &, const Py::Object & );
};

//================================================================================
//
//
//    BemacsBuffer type
//
//
//================================================================================
class BemacsBuffer : public Py::PythonExtension<BemacsBuffer>
{
// static methods
public:
    static void init_type();

// member methods
public:
    BemacsBuffer( EmacsBuffer *_buffer );
    virtual ~BemacsBuffer();

private:
    EmacsBufferRef buffer;

    virtual Py::Object repr();
    virtual Py::Object getattr( const char *c_name );
    virtual int setattr( const char *c_name, Py::Object /*value*/ );
};

//--------------------------------------------------------------------------------
//
//  PythonAllowThreads provides a exception safe
//  wrapper for the C idiom:
//
//  Py_BEGIN_ALLOW_THREADS
//  ...Do some blocking I/O operation...
//  Py_END_ALLOW_THREADS
//
//  IN C++ use PythonAllowThreads in main code:
//  {
//      PythonAllowThreads main_permission;
//      ...Do some blocking I/O operation that may throw
//  } // allow d'tor grabs the lock
//
//  In C++ use PythonDisallowThreads in callback code:
//  {
//      PythonDisallowThreads permission( main_permission );
//      ... Python operations that may throw
//  } // allow d'tor to release the lock
//
//--------------------------------------------------------------------------------
class PythonAllowThreads;
class PythonDisallowThreads;

class BemacsEditorAccessControl
{
public:
    friend class PythonAllowThreads;
    friend class PythonDisallowThreads;

    BemacsEditorAccessControl();
    ~BemacsEditorAccessControl();

private:
    void allowOtherThreads();
    void allowThisThread();

    PyThreadState *m_saved_thread_state;
};

class PythonAllowThreads
{
public:
    // calls allowOtherThreads()
    PythonAllowThreads( BemacsEditorAccessControl &control );
    // calls allowThisThread() if necessary
    ~PythonAllowThreads();

private:
    BemacsEditorAccessControl &m_control;
};

class PythonDisallowThreads
{
public:
    // calls allowThisThread()
    PythonDisallowThreads( BemacsEditorAccessControl &control );
    // calls allowOtherThreads() if necessary
    ~PythonDisallowThreads();
private:
    BemacsEditorAccessControl &m_control;
};

extern BemacsEditorAccessControl editor_access_control;
