//
//    win_ext_func.cpp
//
#include <emacs.h>

#include <windows.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

class EmacsExternImageOsInfo_ForWindows: public EmacsExternImageOsInfo
{
public:
    EmacsExternImageOsInfo_ForWindows();
    ~EmacsExternImageOsInfo_ForWindows();

    bool isLoaded();
    bool load( const EmacsString &file );
    bool unload();
    void *symbolValue( const EmacsString &symbol );

    HINSTANCE    module;
    EmacsString    file;
};

extern EmacsExternImageOsInfo *make_EmacsExternImageOsInfo()
{
    return new EmacsExternImageOsInfo_ForWindows();
}


EmacsCallBackStatus Emacs_UserCallBackFunctions( EmacsCallBackFunctions function, ... );

extern BoundName *active_boundname;

int BoundNameExternalFunction::activate_external_function()
{
    //
    //    Load the module if necessary
    //
    if( !b_func->ef_image->ef_os_info->isLoaded() )
        if( !b_func->ef_image->ef_os_info->load( b_func->ef_image->ef_filename ) )
            return 0;

    //
    //    Load the routine address
    //
    if( b_func->ef_c_function == NULL && b_func->ef_cpp_function == NULL )
    {
        b_func->is_c_plus_plus = false;
        // see if its a extern "C" function
        b_func->ef_c_function = (EmacsCallBackStatus (*)(EmacsCallBackStatus (*)(EmacsCallBackFunctions,...)))b_func->ef_image->ef_os_info->symbolValue( b_func->ef_funcname );
        if( b_func->ef_c_function == NULL )
        {
            // try from C++ name with VC++ V4 mangled name
            EmacsString cxx_name;
            cxx_name = "?";
            cxx_name.append( b_func->ef_funcname );
            cxx_name.append( "@@YA?AW4EmacsCallBackStatus@@AAVEmacsCallBackInterface@@@Z" );
            b_func->ef_cpp_function = (EmacsCallBackStatus (*)(class EmacsCallBackInterface &))b_func->ef_image->ef_os_info->symbolValue( cxx_name );
            if( b_func->ef_cpp_function == NULL )
            {
                error( FormatString("Unable to find %s in %s") << b_func->ef_funcname << b_func->ef_image->ef_filename );
                return 0;
            }

            b_func->is_c_plus_plus = true;
        }
    }

    EmacsString proc_name( active_boundname->b_proc_name );
    EmacsCallBackStatus rv;
    if( b_func->is_c_plus_plus )
    {
        // call as C++
        EmacsCallBackInterface iface( Emacs_UserCallBackFunctions );
        rv = b_func->ef_cpp_function( iface );
    }
    else
    {
        // Call as C
        rv = b_func->ef_c_function( Emacs_UserCallBackFunctions );
    }
    if( rv != EMACS__K_SUCCESS )
    {
        error( FormatString("Function %s failed with status %d") << proc_name << rv );
        void_result();
    }

    return 0;
}

EmacsExternImageOsInfo_ForWindows::EmacsExternImageOsInfo_ForWindows()
    : module(NULL)
{
}

EmacsExternImageOsInfo_ForWindows::~EmacsExternImageOsInfo_ForWindows()
{
}

bool EmacsExternImageOsInfo_ForWindows::isLoaded()
{
    return module != NULL;
}

bool EmacsExternImageOsInfo_ForWindows::load( const EmacsString &_file )
{
    file = _file;
    module = LoadLibrary( file );
    if( module == NULL )
    {
        error( FormatString("Unable to load external function from %s") << file );
        return false;
    }

    return true;
}

bool EmacsExternImageOsInfo_ForWindows::unload()
{
    if( module != NULL )
        if( !FreeLibrary( module ) )
        {
            error( FormatString("Unable to unload external functions from %s") << file );
            return false;
        }
    return true;
}

void *EmacsExternImageOsInfo_ForWindows::symbolValue( const EmacsString &symbol )
{
    void *value = GetProcAddress( module, symbol );
    if( value == NULL )
    {
        return NULL;
    }

    return value;
}

char * save_emacs_string( const EmacsString &str )
{
    char *ret = s_str(malloc_ustr( str.length() + 1 ));
    _str_cpy( ret, str.data() );
    return ret;
}

EmacsCallBackStatus Emacs_UserCallBackFunctions( EmacsCallBackFunctions function, ... )
{
    int rv;
    va_list args;

    va_start( args, function );

    switch( function)
    {
    case EMACS__K_MESSAGE:
    //     message-text-desc
    {
        if( ! ml_err )
        {
            const char *str = va_arg( args, const char * );
            EmacsString e_str( str );

            Save<ProgramNode *> lcur_exec( &cur_exec );
            cur_exec = NULL;
            message( e_str );
        }
        return EMACS__K_SUCCESS;
    }

    case EMACS__K_ERROR:
    //     message-text-desc
    {
        const char *str = va_arg( args, const char * );
        EmacsString e_str( str );
        ml_err = 0;
        error( e_str );
        return EMACS__K_SUCCESS;
    }
    case EMACS__K_SITFOR:
    //     tenths-of-seconds
    {
        sit_for_inner( va_arg( args, int ) );
        return EMACS__K_SUCCESS;
    }
    case EMACS__K_ARG_COUNT:
    //     address of int to receive arg count
    {
        int *p = va_arg( args, int * );

        *p = cur_exec != 0 ? cur_exec->p_nargs : 0;
        return EMACS__K_SUCCESS;
    }
    case EMACS__K_CHECK_ARGS:
    //     min-args max-args
    {
        int min_val = va_arg( args, int );
        int max_val = va_arg( args, int );
        if( check_args( min_val, max_val ) )
            return EMACS__K_FAILURE;
        else
            return EMACS__K_SUCCESS;
    }

    case EMACS__K_STRING_ARG:
    //     argument-number pointer-to-pointer-to-store-string-in
    {
        if( string_arg( va_arg( args, int ) ) == 0 )
            return EMACS__K_FAILURE;

        char **dst = va_arg( args, char ** );

        *dst = save_emacs_string( ml_value.asString() );

        void_result();
        return EMACS__K_SUCCESS;
    }

    case EMACS__K_NUMERIC_ARG:
    //     argument-number int to receive numeric value
    {
        rv = numeric_arg( va_arg( args, int ) );
        if( ml_err )
            return EMACS__K_FAILURE;
        else
        {
            int *p = va_arg( args, int * );
            *p = rv;
            void_result();
            return EMACS__K_SUCCESS;
        }
    }

    case EMACS__K_STRING_RESULT:
    //     desc-of-result
    {
        const char *str = va_arg( args, const char * );
        EmacsString e_str( EmacsString::copy, str );
        ml_value = e_str;
        return EMACS__K_SUCCESS;
    }

    case EMACS__K_NUMERIC_RESULT:
    //     result    : signed long
    {
        ml_value = va_arg( args, int );
        return EMACS__K_SUCCESS;
    }
    case EMACS__K_BUFFER_SIZE:
    //     int to hold buffer size
    {
        int *p = va_arg( args, int * );
        *p = bf_cur->num_characters() + 1 - bf_cur->first_character();
        return EMACS__K_SUCCESS;
    }

    case EMACS__K_DOT:
    //     int to hold numeric-value of dot desc to return
    //    buffer-name in int to hold buffer-name length!
    {
        int *p_int = va_arg( args, int * );

        *p_int = dot;

        return EMACS__K_SUCCESS;
    }

    case EMACS__K_MARK:
    //     int to hold numeric-value of dot desc to return
    //    buffer-name in int to hold buffer-name length
    {
        int *p_int = va_arg( args, int * );
        //char **p_str = va_arg( args, char ** );

        if( !bf_cur->b_mark.isSet() )
        {
            error( FormatString("No mark set in buffer \"%s\"!") << bf_cur->b_buf_name );
            *p_int = 0;
            return EMACS__K_FAILURE;
        }

        *p_int = bf_cur->b_mark.to_mark();
        return EMACS__K_SUCCESS;
    }

    case EMACS__K_SET_MARK:
    //     true - set, false - unset
    {
        if( va_arg( args, int ) )
        {
            bf_cur->set_mark( dot, 0, false );
            cant_1line_opt = 1;
        }
        else
            bf_cur->unset_mark();
        return EMACS__K_SUCCESS;
    }

    case EMACS__K_BUFFER_NAME:
    //    buffer-name in int to hold buffer-name length
    {
        char **p_str = va_arg( args, char ** );
        *p_str = save_emacs_string( bf_cur->b_buf_name );
        if( *p_str == NULL )
            return EMACS__K_FAILURE;
        return EMACS__K_SUCCESS;
    }

    case EMACS__K_USE_BUFFER:
    //     buffer-name-desc
    {
        const char *str = va_arg( args, const char * );
        EmacsString e_str( str );
        EmacsBuffer::set_bfn( e_str );
        return EMACS__K_SUCCESS;
    }

    case EMACS__K_GOTO_CHARACTER:
    //     position-to-goto
    {
        int pos;
        pos = va_arg( args, int );
        if( pos < 1 )
            pos = 1;
        if( pos > bf_cur->num_characters() )
            pos = bf_cur->num_characters() + 1;
        set_dot( pos );
        return EMACS__K_SUCCESS;
    }

    case EMACS__K_INSERT_CHARACTER:
    //     char to insert
    {
        unsigned char c = va_arg( args, unsigned char );
        bf_cur->ins_cstr( &c, 1 );
        return EMACS__K_SUCCESS;
    }

    case EMACS__K_INSERT_STRING:
    //     desc of string to insert
    {
        const unsigned char *str = va_arg( args, const unsigned char * );
        bf_cur->ins_cstr( str, _str_len( str ) );
        return EMACS__K_SUCCESS;
    }

    case EMACS__K_DELETE_CHARACTERS:
    //     number of chars to delete, -iv means backwards
    {
        int forw = 1;
        int cnt = va_arg( args, int );
        if( cnt < 0 )
        {
            cnt = -cnt;
            forw = 0;
        }
        int repeat;
        if( (repeat = del_chars_in_buffer( dot, cnt, forw )) != 0 )
            if( forw )
                dot_right( repeat );
        if( ! forw )
            dot_left( cnt );
        return EMACS__K_SUCCESS;
    }

    case EMACS__K_FETCH_CHARACTER:
    //     position of required char int to hold char at dot
    {
        int pos = va_arg( args, int );
        int *p = va_arg( args, int * );

        if( pos < bf_cur->num_characters() && pos > 0 )
            *p = bf_cur->char_at( pos );
        else
            *p = -1;

        return EMACS__K_SUCCESS;
    }

    case EMACS__K_RETURN_STRING_TO_PARAM:
    //     argument-number string-desc
    {
        int argnum = va_arg( args, int );
        const unsigned char *p_str = va_arg( args, const unsigned char * );

        if( cur_exec == 0
        || argnum < 1
        || argnum > cur_exec->p_nargs )
        {
            error( FormatString("Error, Argument number %d out of range") << argnum );
            return EMACS__K_FAILURE;
        }

        ProgramNode *p = cur_exec->arg( argnum );
        if( p->p_proc != &bound_variable_node )
        {
            error( FormatString("Emacs Call back expects argument %d to be a variable name.") << argnum );
            return EMACS__K_FAILURE;
        }

        Expression val( EmacsString( EmacsString::copy, p_str, _str_len( p_str ) ) );
        perform_set( p->name(), val, 0 );

        return EMACS__K_SUCCESS;
    }

    case EMACS__K_RETURN_NUMBER_TO_PARAM:
    //     argument-number integer
    {
        ProgramNode *p;
        int argnum;
        argnum = va_arg( args, int );

        if( cur_exec == 0
        || argnum < 1
        || argnum > cur_exec->p_nargs )
        {
            error( FormatString("Error, Argument number %d out of range") << argnum );
            return EMACS__K_FAILURE;
        }
        p = cur_exec->arg( argnum );
        if( p->p_proc != &bound_variable_node )
        {
            error( FormatString("Emacs Call back expects argument %d to be a variable name.") << argnum );
            return EMACS__K_FAILURE;
        }
        Expression val( va_arg( args, int ) );
        perform_set( p->name(), val, 0 );
        return EMACS__K_SUCCESS;
    }

    case EMACS__K_BUFFER_EXTENT: // s1, p1, s2, p2
    {
        unsigned char *p1;
        unsigned char *p2;
        int s1;
        int s2;
        bf_cur->bufferExtent( p1, s1, p2, s2 );
        *va_arg( args, int *) = s1;
        *va_arg( args, unsigned char * *) = p1;
        *va_arg( args, int *) = s2;
        *va_arg( args, unsigned char * *) = p2;
    }
        break;

    case EMACS__K_GAP_TO: // position
        bf_cur->gap_to( va_arg( args, int ) );
        break;

    case EMACS__K_GAP_ROOM: // size
        bf_cur->gap_room( va_arg( args, int ) );
        break;

    case EMACS__K_ALLOCATE_STRING:
    //     size-to-allocate int to hold buffer address
    {
        void *mem;
        int size = va_arg( args, int );
        void **p = va_arg( args, void ** );

        if( (mem = malloc_ustr( size )) == NULL )
        {
            *p = NULL;
            return EMACS__K_FAILURE;
        }
        else
        {
            *p = mem;
            return EMACS__K_SUCCESS;
        }
    }

    case EMACS__K_REALLOCATE_STRING:
    //     old-buffer-address new-size int to hold buffer
    //    address
    {
        void *old_mem = va_arg( args, void * );
        int size = va_arg( args, int );
        void **p = va_arg( args, void ** );

        void *new_mem = (void *)realloc_ustr( old_mem, size );
        if( new_mem == NULL )
        {
            *p = NULL;
            return EMACS__K_FAILURE;
        }
        else
        {
            *p = new_mem;
            return EMACS__K_SUCCESS;
        }
    }


    case EMACS__K_FREE_STRING:
    //     buffer-to-free
    {
        EMACS_FREE( va_arg( args, void * ) );
        return EMACS__K_SUCCESS;
    }

    default:
        return EMACS__K_FAILURE;
    }

    return EMACS__K_SUCCESS;
}
