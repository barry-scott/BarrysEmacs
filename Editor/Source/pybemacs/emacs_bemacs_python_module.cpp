//
//    Emacs_python.cpp
//
//    Copyright (c) 1999 Barry A. Scott
//
#include <string_map.h>

#include <sstream>
#include <algorithm>
#include <functional>

Expression convertPyObjectToEmacsExpression( const Py::Object &obj );
Py::Object convertEmacsExpressionToPyObject( const Expression &expr );

void init_bemacs_python(void);

//================================================================================
//
//
//    BEMACS python module
//
//
//================================================================================

//================================================================================
//
//
//    BemacsVariables type - allows access to all emacs MLisp variables
//
//
//================================================================================
class BemacsVariables: public Py::PythonExtension<BemacsVariables>
{
// static methods
public:
    static void init_type()
    {
        behaviors().name("BemacsVariables");
        behaviors().doc("Access all MLisp variables");
        behaviors().supportGetattr();
        behaviors().supportSetattr();
    }

// member methods
public:
    BemacsVariables() {}
    virtual ~BemacsVariables() {}

private:
    virtual Py::Object getattr( const char *c_name )
    {
        std::string str_name( c_name );

        if( str_name == "__members__" )
        {
            Py::List members;

            int limit = VariableName::name_table.entries();
            for( int index=0; index    < limit; index++ )
            {
                const EmacsString *name = VariableName::name_table.key( index );
                std::string str_name( name->sdata() );

                // map all "-" in name to "_". This works well as Python uses "_" and MLisp uses "-".
                // thus mode-line-format becomes mode_line_format
                std::replace_if( str_name.begin(), str_name.end(), std::bind2nd(std::equal_to<char>(), '-'), '_' );

                members.append( Py::String( str_name ) );
            }

            return members;
        }

        if( str_name == "__methods__" )
            return Py::List();

        // treat all __ names as reserved to python
        if( c_name[0] == '_' && c_name[1] == '_' )
            throw Py::NameError( c_name );

        // map all "_" in name to "-". This works well as Python uses "_" and MLisp uses "-".
        // thus mode_line_format becomes mode-line-format
        std::replace_if( str_name.begin(), str_name.end(), std::bind2nd(std::equal_to<char>(), '_'), '-' );

        EmacsString name( str_name.c_str() );

        VariableName *var_name = VariableName::find( name );
        if( var_name == NULL )
            return getattr_methods( c_name );
            //throw Py::NameError( c_name );

        Expression value;
        if( !var_name->normalValue( value ) )
            throw Py::NameError( c_name );

        Py::Object result;

        result = convertEmacsExpressionToPyObject( value );

        return result;
    }

    virtual int setattr( const char *c_name, const Py::Object &value )
    {
        // treat all __ names as reserved to python
        if( c_name[0] == '_' && c_name[1] == '_' )
            throw Py::NameError( c_name );

        std::string str_name( c_name );

        // map all "_" in name to "-". This works well as Python uses "_" and MLisp uses "-".
        // thus mode_line_format becomes mode-line-format
        std::replace_if( str_name.begin(), str_name.end(), std::bind2nd(std::equal_to<char>(), '_'), '-' );

        EmacsString name( str_name.c_str() );

        VariableName *var_name = VariableName::find( name );
        if( var_name == NULL )
            throw Py::NameError( c_name );

        Expression result = convertPyObjectToEmacsExpression( value );
        if( !var_name->assignNormal( result ) )
            throw Py::NameError( c_name );

        return 0;
    }
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
    static void init_type()
    {
            behaviors().name("BemacsFunctions");
            behaviors().doc("Access all MLisp functions");
            behaviors().supportGetattr();
            behaviors().supportSetattr();
    }

// member methods
public:
    BemacsFunctions() {}
    virtual ~BemacsFunctions() {}

private:
    static PyObject *call_bemacs_function( PyObject *self, PyObject *_args )
    {
        try
        {
            Py::String py_fn_name( self );
            Py::Tuple args( _args );

            std::string std_fn_name( py_fn_name );

            EmacsString name( std_fn_name.c_str() );

            BoundName *fn_binding = BoundName::find( name );
            if( fn_binding == NULL || !fn_binding->isBound() )
                throw Py::NameError(std_fn_name);

            ProgramNodeNode prog_node( fn_binding, args.size() );

            for( unsigned int arg=0; arg<args.size(); arg++ )
            {
                Py::Object x( args[arg] );
                Expression expr( convertPyObjectToEmacsExpression( x ) );

                // must new the ProgramNodeExpression as its deleted via the NodeNode d'tor
                prog_node.pa_node[arg] = new ProgramNodeExpression( expr );
            }


            exec_prog( &prog_node );
            if( ml_err )
            {
                ml_err = 0;
                throw Py::RuntimeError( error_message_text.sdata() );
            }

            Py::Object result = convertEmacsExpressionToPyObject( ml_value );

            return Py::new_reference_to( result );

        }
        catch( Py::Exception & )
        {
            return 0;
        }
    }

    virtual Py::Object getattr( const char *c_name )
    {
        std::string std_fn_name( c_name );

        if( std_fn_name == "__members__" )
            return Py::List();

        if( std_fn_name == "__methods__" )
        {
            Py::List methods;

            int limit = BoundName::name_table.entries();
            for( int index=0; index    < limit; index++ )
            {
                const EmacsString *name = BoundName::name_table.key( index );
                std::string str_name( name->sdata() );

                // map all "-" in name to "_". This works well as Python uses "_" and MLisp uses "-".
                // thus mode-line-format becomes mode_line_format
                std::replace_if( str_name.begin(), str_name.end(), std::bind2nd(std::equal_to<char>(), '-'), '_' );

                methods.append( Py::String( str_name ) );
            }

            return methods;
        }

        // treat all __ names as reserved to python
        if( c_name[0] == '_' && c_name[1] == '_' )
            throw Py::NameError( c_name );

        // map all "_" in name to "-". This works well as Python uses "_" and MLisp uses "-".
        // thus mode_line_format becomes mode-line-format
        std::replace_if( std_fn_name.begin(), std_fn_name.end(), std::bind2nd(std::equal_to<char>(), '_'), '-' );

        EmacsString name( std_fn_name.c_str() );

        BoundName *fn_binding = BoundName::find( name );
        if( fn_binding == NULL || !fn_binding->isBound() )
            throw Py::NameError(std_fn_name);

        static PyMethodDef method_definition =
        {
            "call_bemacs_function",
            call_bemacs_function,
            METH_VARARGS,
            "call_bemacs_function"
        };
        PyObject *func = PyCFunction_New
                (
                &method_definition,
                Py::String( name.sdata() ).ptr()
                );

        return Py::Object( func, true );
    }

    virtual int setattr( const char *c_name, Py::Object /*value*/ )
    {
        throw Py::NameError( c_name );

    }
};

//================================================================================
//
//
//    BemacsBufferData type
//
//
//================================================================================
class BemacsBufferData : public Py::PythonExtension<BemacsBufferData>
{
// static methods
public:
    static void init_type()
    {
            behaviors().name("BemacsBuffer");
            behaviors().doc("bemacs buffer");
            behaviors().supportGetattr();
            behaviors().supportSetattr();
            behaviors().supportRepr();
        behaviors().supportSequenceType();
    }

// member methods
public:
    BemacsBufferData( EmacsBuffer *_buffer )
        : buffer( _buffer )
    {}
    virtual ~BemacsBufferData() {}

private:
    EmacsBufferRef buffer;

    virtual Py::Object repr()
    {
        EmacsString str;
        if( buffer.bufferValid() )
            str = FormatString( "<BEmacs buffer_data \"%s\">" ) << buffer->b_buf_name;
        else
            str = "<BEmacs buffer_data has deleted>";

        Py::String result( str.sdata() );

        return result;
    }

    virtual Py::Object getattr( const char *c_name )
    {
        return getattr_methods( c_name );
    }

    virtual int setattr( const char *c_name, Py::Object /*value*/ )
    {
        throw Py::NameError( c_name );
    }

    //--------------------------------------------------------------------------------
    //
    // Sequence methods
    //
    //--------------------------------------------------------------------------------
    virtual int sequence_length()
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        return Py::Int( buffer.buffer()->restrictedSize() );
    }

    virtual Py::Object sequence_concat( const Py::Object & )
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        throw Py::ValueError("cannot concat");
    }

    virtual Py::Object sequence_repeat( int )
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        throw Py::ValueError("cannot repeat");
    }

    virtual Py::Object sequence_item( int position )
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        if( position < 0 || position > buffer->restrictedSize() )
            throw Py::ValueError("index out of range");

        char ch[2];
        ch[0] = buffer->char_at( position + buffer->first_character() );
        ch[1] = '\0';

        return Py::String( ch );
    }

    virtual Py::Object sequence_slice( int start, int end )
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        if( start < 0 || start > buffer->restrictedSize() )
            throw Py::ValueError("start index out of range");
        if( end == INT_MAX )
            end = buffer->restrictedSize();
        if( end < 0 || end > buffer->restrictedSize() )
            throw Py::ValueError("end index out of range");

        if( end <= start )
            // return a null string
            return Py::String();

        start += buffer->first_character();
        end += buffer->first_character();
        buffer->gap_outside_of_range( start, end );

        return Py::String( (char *)buffer->ref_char_at( start ), end - start );
    }

    virtual int sequence_ass_item( int, const Py::Object & )
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        throw Py::ValueError("cannot ass_item");
    }

    virtual int sequence_ass_slice( int, int, const Py::Object & )
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        throw Py::ValueError("cannot ass_slice");
    }
};


//================================================================================
//
//
//    BemacsBufferSyntax type
//
//
//================================================================================
class BemacsBufferSyntax : public Py::PythonExtension<BemacsBufferSyntax>
{
// static methods
public:
    static void init_type()
    {
        behaviors().name("BemacsBuffer");
        behaviors().doc("bemacs buffer");
        behaviors().supportGetattr();
        behaviors().supportSetattr();
        behaviors().supportRepr();
        behaviors().supportSequenceType();
    }

// member methods
public:
    BemacsBufferSyntax( EmacsBuffer *_buffer )
    : buffer( _buffer )
    {}
    virtual ~BemacsBufferSyntax() {}

private:
    EmacsBufferRef buffer;

    virtual Py::Object repr()
    {
        EmacsString str( FormatString( "<BEmacs buffer_syntax \"%s\">" ) << buffer->b_buf_name );

        Py::String result( str.sdata() );

        return result;
    }

    virtual Py::Object getattr( const char *c_name )
    {
        return getattr_methods( c_name );
    }

    virtual int setattr( const char *c_name, Py::Object /*value*/ )
    {
        throw Py::NameError( c_name );
    }

    //--------------------------------------------------------------------------------
    //
    // Sequence methods
    //
    //--------------------------------------------------------------------------------
    virtual int sequence_length()
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        return Py::Int( buffer->restrictedSize() );
    }

    virtual Py::Object sequence_concat( const Py::Object & )
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        throw Py::ValueError("cannot concat");
    }

    virtual Py::Object sequence_repeat( int )
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        throw Py::ValueError("cannot repeat");
    }

    virtual Py::Object sequence_item( int position )
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        if( buffer->b_syntax.syntax_base == NULL )
            throw Py::ValueError("syntax is turned off in the buffer");

        if( position < 0 || position > buffer->restrictedSize() )
            throw Py::ValueError("index out of range");

        int syntax = buffer->syntax_at( position + buffer->first_character() );

        return Py::Int( syntax );
    }

    virtual Py::Object sequence_slice( int start, int end )
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        if( buffer->b_syntax.syntax_base == NULL )
            throw Py::ValueError("syntax is turned off in the buffer");

        if( start < 0 || start > buffer->restrictedSize() )
            throw Py::ValueError("start index out of range");
        if( end == INT_MAX )
            end = buffer->restrictedSize();
        if( end < 0 || end > buffer->restrictedSize() )
            throw Py::ValueError("end index out of range");

        start += buffer->first_character();
        end += buffer->first_character();

        if( end <= start )
            // return a null List
            return Py::List();

        buffer->syntax_fill_in_array( end+1 );

        Py::List slice( end-start );

        for( int pos=start; pos<end; pos++ )
            slice[pos-start] = Py::Int( buffer->syntax_at( pos ) );

        return slice;
    }

    virtual int sequence_ass_item( int, const Py::Object & )
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        throw Py::ValueError("cannot ass_item");
    }

    virtual int sequence_ass_slice( int, int, const Py::Object & )
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        throw Py::ValueError("cannot ass_slice");
    }
};


//================================================================================
//
//
//    BemacsBuffer type
//
//
//================================================================================
extern StringMap<buffer_types> buffer_types_map;

class BemacsBuffer : public Py::PythonExtension<BemacsBuffer>
{
// static methods
public:
    static void init_type()
    {
            behaviors().name("BemacsBuffer");
            behaviors().doc("bemacs buffer");
            behaviors().supportGetattr();
            behaviors().supportSetattr();
            behaviors().supportRepr();
    }

// member methods
public:
    BemacsBuffer( EmacsBuffer *_buffer )
        : buffer( _buffer )
    {}
    virtual ~BemacsBuffer()
    {}

private:
    EmacsBufferRef buffer;

    virtual Py::Object repr()
    {
        EmacsString str( FormatString( "<BEmacs buffer \"%s\">" ) << buffer->b_buf_name );

        Py::String result( str.sdata() );

        return result;
    }

    virtual Py::Object getattr( const char *c_name )
    {
        if( !buffer.bufferValid() )
            throw Py::ValueError("buffer has been deleted");

        std::string name( c_name );

        Py::Object result;

        if( name == "data" )
        {
            result = Py::Object( new BemacsBufferData( buffer ) );
        }
        else
        if( name == "syntax" )
        {
            result = Py::Object( new BemacsBufferSyntax( buffer ) );
        }
        else
        if( name == "extent" )
        {
            Py::Tuple extent(2);
            extent[0] = Py::Int( buffer->first_character() );
            extent[1] = Py::Int( buffer->num_characters() );
            result = extent;
        }
        else
        if( name == "type" )
        {
            EmacsString type_string = buffer_types_map.map( buffer->b_kind );
            result = Py::String( type_string.sdata() );
        }
        else
        if( name == "file_name" )
        {
            result = Py::String( buffer->b_fname );
        }
        else
        if( name == "file_time" )
        {
            result = Py::Int( buffer->b_file_time );
        }
        else
        if( name == "file_synchonise_time" )
        {
            result = Py::Int( buffer->b_synch_file_time );
        }
        else
        if( name == "file_permissions" )
        {
            result = Py::Int( buffer->b_file_access );
        }
        else
        if( name == "file_synchonise_permissions" )
        {
            result = Py::Int( buffer->b_synch_file_access );
        }
        else
        if( name == "__name__" )
        {
            result = Py::String( buffer->b_buf_name );
        }
        else
        if( name == "__members__" )
        {
            Py::List members;
            members.append( Py::String( "data" ) );
            members.append( Py::String( "syntax" ) );
            members.append( Py::String( "extent" ) );
            members.append( Py::String( "type" ) );
            members.append( Py::String( "file_name" ) );
            members.append( Py::String( "file_time" ) );
            members.append( Py::String( "file_synchonise_time" ) );
            members.append( Py::String( "file_permissions" ) );
            members.append( Py::String( "file_synchonise_permissions" ) );
            members.append( Py::String( "__name__" ) );

            return members;
        }
        else
            result = getattr_methods( c_name );

        return result;
    }

    virtual int setattr( const char *c_name, Py::Object /*value*/ )
    {
        throw Py::NameError( c_name );
    }

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
    static void init_type()
    {
            behaviors().name("BemacsBuffersDict");
            behaviors().doc("buffers dictionary");
            behaviors().supportGetattr();
            behaviors().supportMappingType();

        add_varargs_method( "keys", keys );
    }

// member methods
public:
    BemacsBuffersDict()
    {}
    virtual ~BemacsBuffersDict()
    {}

private:
    virtual Py::Object getattr( const char *c_name )
    {
        std::string str_name( c_name );

        // else return any methods we have defined
        return getattr_methods( c_name );
    }

    Py::Object keys( const Py::Tuple &args )
    {
        if( args.size() != 0 )
            throw Py::ValueError( "keys() takes no parameters" );

        Py::List names;

        for( int index=0; index<EmacsBuffer::name_table.entries(); index++ )
        {
            EmacsBuffer *buf = EmacsBuffer::name_table.value( index );

            Py::String name( buf->b_buf_name.sdata() );

            names.append( name );
        }

        return names;
    }

    // Mapping
    virtual int mapping_length()
    {
        return EmacsBuffer::name_table.entries();
    }

    virtual Py::Object mapping_subscript( const Py::Object &key )
    {
        EmacsBuffer *buf = NULL;

        if( key.isNumeric() )
        {
            Py::Int index( key );

            if( (int)index < 0 || (int)index >= EmacsBuffer::name_table.entries() )
                throw Py::ValueError("index out of range");

            buf = EmacsBuffer::name_table.value( index );
        }
        else
        if( key.isString() )
        {
            Py::String str( key );
            std::string _str( str );

            buf = EmacsBuffer::name_table.find( EmacsString( _str ) );
        }
        else
            throw Py::TypeError( "use int or string as subscript" );

        if( buf == NULL )
            throw Py::ValueError( "subscript unknown" );

        return Py::Object( new BemacsBuffer( buf ) );
    }

    virtual int mapping_ass_subscript( const Py::Object &, const Py::Object & )
    {
        throw Py::TypeError("cannot assign to buffer_names");
    }
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
    static void init_type()
    {
            behaviors().name("BemacsMarker");
            behaviors().doc("marker.position, marker.buffer");
            behaviors().supportGetattr();
            behaviors().supportSetattr();
            behaviors().supportRepr();

        add_varargs_method( "as_tuple", as_tuple );
    }

    Expression value()
    {
        return Expression( &marker );
    }

// member methods
public:
    BemacsMarker( Marker *_marker )
        : marker( *_marker )
    {}
    virtual ~BemacsMarker() {}

private:
    Marker marker;

    virtual Py::Object repr()
    {
        EmacsString str( FormatString( "<BEmacs Marker (\"%s\", %d)>" )
                << marker.m_buf->b_buf_name
                << marker.get_mark() );

        Py::String result( str.sdata() );

        return result;
    }

    virtual Py::Object getattr( const char *c_name )
    {
        std::string name( c_name );

        Py::Object result;

        if( name == "position" )
            result = Py::Int( marker.get_mark() );
        else
        if( name == "buffer_name" )
            result = Py::String( marker.m_buf->b_buf_name.sdata() );
        else
        if( name == "__members__" )
        {
            Py::List members;
            members.append( Py::String( "position" ) );
            members.append( Py::String( "buffer_name" ) );

            return members;
        }
        else
            result = getattr_methods( c_name );

        return result;
    }

    virtual int setattr( const char *c_name, Py::Object /*value*/ )
    {
        throw Py::NameError( c_name );
    }

    Py::Object as_tuple( const Py::Tuple &args )
    {
        args.verify_length( 0 );

        Py::Tuple result( 2 );

        result[0] = Py::String( marker.m_buf->b_buf_name.sdata() );
        result[1] = Py::Int( marker.get_mark() );

        return result;
    }
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
    static void init_type()
    {
            behaviors().name("BemacsWindowRing");
            behaviors().doc("bemacs window ring");
            behaviors().supportGetattr();
            behaviors().supportSetattr();
            behaviors().supportRepr();
    }

// member methods
public:
    BemacsWindowRing( const Expression &_window_ring )
        : window_ring( _window_ring )
    {}
    virtual ~BemacsWindowRing() {}

    const Expression &value() const
    {
        return window_ring;
    }

private:
    // use an express as it takes care of the reference counting of windows rings
    Expression window_ring;

    virtual Py::Object repr()
    {
        Py::String result( "<BEmacs Window Ring>" );

        return result;
    }

    virtual Py::Object getattr( const char *c_name )
    {
        throw Py::NameError( c_name );
    }

    virtual int setattr( const char *c_name, const Py::Object &/*value*/ )
    {
        throw Py::NameError( c_name );
    }
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
    static void init_type()
    {
            behaviors().name("BemacsArray");
            behaviors().doc("bemacs window ring");
            behaviors().supportGetattr();
            behaviors().supportSetattr();
            behaviors().supportRepr();
    }

// member methods
public:
    BemacsArray( const Expression &_array )
        : array( _array )
    {}
    virtual ~BemacsArray() {}

    const Expression &value() const
    {
        return array;
    }

private:
    // use an express as it takes care of the reference counting of windows rings
    Expression array;

    virtual Py::Object repr()
    {
        Py::String result( "<BEmacs Array>" );

        return result;
    }

    virtual Py::Object getattr( const char *c_name )
    {
        throw Py::NameError( c_name );
    }

    virtual int setattr( const char *c_name, const Py::Object &/*value*/ )
    {
        throw Py::NameError( c_name );
    }
};



//================================================================================
//
//
//    bemacs_module type
//
//
//================================================================================
class bemacs_module : public Py::ExtensionModule<bemacs_module>
{
public:
    bemacs_module()
        : Py::ExtensionModule<bemacs_module>( "bemacs" )
    {

        //
        // init types used by this module
        //
        BemacsVariables::init_type();
        BemacsFunctions::init_type();
        BemacsMarker::init_type();
        BemacsWindowRing::init_type();
        BemacsArray::init_type();
        BemacsBuffersDict::init_type();
        BemacsBuffer::init_type();
        BemacsBufferSyntax::init_type();
        BemacsBufferData::init_type();

        //
        //    Add the methods of this module
        //
        add_varargs_method( "call_function", call_bemacs_function );
        add_varargs_method( "report_error", report_error,
            "report_error\n"
            "\n"
            "report_error( error_message_string )\n"
            "Emacs will report the error_message_string as the\n"
            "result of the currently executing Python code"
        );


        //
        //    Initialise the module
        //
        initialize( "bemacs interface module" );

        //
        // add objects to modules dictionary as it exists now
        //
        Py::Dict module_dictionary( moduleDictionary() );

        module_dictionary["variable"] = Py::Object( new BemacsVariables );
        module_dictionary["function"] = Py::Object( new BemacsFunctions );
        module_dictionary["buffers"] = Py::Object( new BemacsBuffersDict );
    }

    Py::Object report_error( const Py::Tuple &args )
    {
        args.verify_length(1);
        Py::String error_message( args[0] );

        return Py::Object();
    }

    Py::Object call_bemacs_function( const Py::Tuple &args )
    {
        Py::String py_fn_name( args[0] );
        std::string std_fn_name( py_fn_name );
        EmacsString name( std_fn_name.c_str() );

        BoundName *fn_binding = BoundName::find( name );
        if( fn_binding == NULL || !fn_binding->isBound() )
            throw Py::NameError(std_fn_name);

        ProgramNodeNode prog_node( fn_binding, args.size() - 1 );

        for( unsigned int arg=1; arg<args.size(); arg++ )
        {
            Py::Object x( args[arg] );
            Expression expr( convertPyObjectToEmacsExpression( x ) );

            // must new the ProgramNodeExpression as its deleted via the NodeNode d'tor
            prog_node.pa_node[arg-1] = new ProgramNodeExpression( expr );
        }


        exec_prog( &prog_node );
        if( ml_err )
        {
            ml_err = 0;
            throw Py::RuntimeError( error_message_text.sdata() );
        }

        Py::Object result = convertEmacsExpressionToPyObject( ml_value );

        return result;
    }
};

static bemacs_module *bemacs_module_pointer;

void init_bemacs_python()
{
    //
    // build the module
    //
    bemacs_module_pointer = new bemacs_module;
}

//--------------------------------------------------------------------------------

Expression convertPyObjectToEmacsExpression( const Py::Object &obj )
{
    Expression expr;

    if( obj.isNumeric() )
    {
        expr = Expression( int( long( Py::Int( obj ) ) ) );
    }
    else
    if( obj.isString() )
    {
        Py::String str( obj );
        EmacsString string( EmacsString::copy, PyString_AsString( str.ptr() ), str.size() );
        expr = Expression( string );
    }
    else
    if( BemacsArray::check( obj ) )
    {
        BemacsArray *o = static_cast<BemacsArray *>( obj.ptr() );
        expr = o->value();
    }
    else
    if( BemacsWindowRing::check( obj ) )
    {
        BemacsWindowRing *o = static_cast<BemacsWindowRing *>( obj.ptr() );
        expr = o->value();
    }
    else
    if( BemacsMarker::check( obj ) )
    {
        BemacsMarker *o = static_cast<BemacsMarker *>( obj.ptr() );
        expr = o->value();
    }

    return expr;
}

Py::Object convertEmacsExpressionToPyObject( const Expression &expr )
{
    Py::Object obj;    // Inits to None

    switch( expr.exp_type() )
    {
    case ISINTEGER:
        obj = Py::Int( expr.asInt() );
        break;

    case ISSTRING:
        {
        EmacsString string( expr.asString() );

        obj = Py::String( string.sdata(), string.length() );
        }
        break;

    case ISVOID:
        // return None
        break;

    case ISMARKER:
        obj = Py::Object( new BemacsMarker( expr.asMarker() ), true );
        break;

    case ISWINDOWS:
        obj = Py::Object( new BemacsWindowRing( expr ), true );
        break;

    case ISARRAY:
        obj = Py::Object( new BemacsArray( expr ), true );
        break;

    default:
        // leave as None for now
        break;
    }

    return obj;
}
