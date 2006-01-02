//
//    vms_ext_func.cpp
//
//    copyright (c) 1996 Barry A. Scott
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#ifdef vms
#include <stsdef.h>
#include <lnmdef.h>
#endif

void init_opt( void );

#if defined(vms) && defined(CALL_BACK)
extern int lib$analyze_sdesc( struct dsc$descriptor *, unsigned short *, unsigned char **);
extern int lib$find_image_symbol( struct dsc$descriptor *, struct dsc$descriptor *,void * *);
extern int str$upcase(struct dsc$descriptor *,struct dsc$descriptor *);
extern int str$copy_r( struct dsc$descriptor *, unsigned short *, unsigned char * );
#endif

//
// external-defun functions.
//
// These include the function to activate the users code, and the function
// to process any call backs to EMACS that the user may want to make.
//
#if defined(vms) && defined(CALL_BACK)
#include <em_user.h>

static int vms_to_c( struct dsc$descriptor *str )
{
    unsigned short int len;
    unsigned char *addr;
    int resp;
    if( ! (resp = VMS_SUCCESS(lib$analyze_sdesc( str, &len, &addr ))) )
    {
        error( "Error `%s' during EMACS call back",
                fetch_os_error( resp ) );
        return resp;
    }
    if( call_back_len < len + 1 )
    {
        if( call_back_str != NULL )
            free( call_back_str );
        if( (call_back_str = malloc_ustr( len + 1)) == NULL )
        {
            call_back_len = 0;
            error( "Out of memory during EMACS call back" );
            return SS$_INSFMEM;
        }
        else
            call_back_len++;
    }
    memcpy( call_back_str, addr, len );
    call_back_str[len] = 0;
    return SS$_NORMAL;
}



static int activate_handler
    (
    struct chf$signal_array *sig,
    struct chf$mech_array *mech
    )
{
    int cond;
    cond = sig->chf$l_sig_name;

    // ignore info, success and warnings
    if( (cond&1)
    || (cond & STS$M_SEVERITY) == STS$K_WARNING )
        return SS$_CONTINUE;

#if defined(vax)
    mech->chf$l_mch_savr0 = cond;
#elif defined(alpha)
    mech->chf$q_mch_savr0 = cond;
#endif

    // Make the faulting routine return the error status
#if defined(vax)
    sys$unwind( &mech->chf$l_mch_depth, NULL );
#elif defined(__ALPHA)
    sys$unwind( (long int *)&mech->chf$q_mch_depth, NULL );
#else
    #error "what goes here?"
#endif

    return SS$_NORMAL;
}

static int call_back (int function, ... )
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
            if( ! (rv = vms_to_c( va_arg( args, struct dsc$descriptor * ) ) ) )
                return rv;
            SaveProgramNodePointer old_cur_exec( &cur_exec );

            cur_exec = NULL;

            message( call_back_str );
        }
        return SS$_NORMAL;
    }

    case EMACS__K_ERROR:
    //     message-text-desc
    {
        ml_err = 0;
        if( ! ( rv = vms_to_c( va_arg( args, struct dsc$descriptor * ) ) ) )
            return rv;
        error( call_back_str );
        return SS$_NORMAL;
    }
    case EMACS__K_SITFOR:
    //     tenths-of-seconds
    {
        sit_for_inner( va_arg( args, int ) );
        return SS$_NORMAL;
    }
    case EMACS__K_ARG_COUNT:
    //     address of int to receive arg count
    {
        int *p = va_arg( args, int * );

        *p = (( cur_exec != 0 ) ?  cur_exec->p_nargs : 0);
        return SS$_NORMAL;
    }
    case EMACS__K_CHECK_ARGS:
    //     min-args max-args
    {
        int min_val = va_arg( args, int );
        int max_val = va_arg( args, int );
        if( check_args( min_val, max_val ) )
            return SS$_BADPARAM;
        else
            return SS$_NORMAL;
    }

    case EMACS__K_STRING_ARG:
    //    argument-number desc-to-store-string-in int to
    //    receive string length
        if( string_arg( va_arg( args, int ) ) == 0 )
            return 0;
        else
        {
            struct dsc$descriptor *dst;
            unsigned short int len;
            unsigned char *addr;
            dst = va_arg( args, struct dsc$descriptor * );

            len = (unsigned short)ml_value.exp_int;
            if( ! (rv = VMS_SUCCESS(str$copy_r
                    (dst,
                    &len,
                    ml_value.exp_v.v_string ))) )
                error( "Error `%s' reported by EMACS call back",
                    fetch_os_error( rv ) );
            else
            {
                unsigned short int *p = va_arg( args, unsigned short int * );
                lib$analyze_sdesc( dst, &len, &addr );
                rv = SS$_NORMAL;

                *p = min( (unsigned short)ml_value.exp_int, len );
            }
            void_result();
            return rv;
        }

    case EMACS__K_NUMERIC_ARG:
    //     argument-number int to receive numeric value
    {
        rv = numeric_arg( va_arg( args, int ) );
        if( ml_err )
            return SS$_BADPARAM;
        else
        {
            int *p = va_arg( args, int * );
            *p = rv;
            void_result();
            return SS$_NORMAL;
        }
    }

    case EMACS__K_STRING_RESULT:
    //     desc-of-result
    {
        unsigned char *ptr;
        unsigned char *addr;
        unsigned short int len;
        if( ! ( rv = VMS_SUCCESS(lib$analyze_sdesc( va_arg( args, struct dsc$descriptor * ), &len, &addr )) ) )
        {
            error( "Error `%s' during EMACS call back",
                fetch_os_error( rv ) );
            return rv;
        }
        if( (ptr = malloc_ustr( len + 1 )) == NULL )
        {
            error( "Out of memory during EMACS call back" );
            return SS$_INSFMEM;
        }
        memcpy( ptr, addr, len );
        ptr[len] = 0;
        release_expr( call_back_result );
        call_back_result->exp_type = ISSTRING;
        call_back_result->exp_int = len;
        call_back_result->exp_v.v_string = ptr;
        call_back_result->exp_release = 1;
        return SS$_NORMAL;
    }

    case EMACS__K_NUMERIC_RESULT:
    //     result    : signed long
    {
        release_expr( call_back_result );
        call_back_result->exp_type = ISINTEGER;
        call_back_result->exp_int = va_arg( args, int );
        call_back_result->exp_v.v_string = 0;
        call_back_result->exp_release = 0;
        return SS$_NORMAL;
    }
    case EMACS__K_ALLOCATE:
    //     size-to-allocate int to hold buffer address
    {
        void *mem;
        int size = va_arg( args, int );
        void **p = va_arg( args, void ** );

        if( (mem = malloc_ustr( size )) == NULL )
        {
            *p = NULL;
            return SS$_INSFMEM;
        }
        else
        {
            *p = mem;
            return SS$_NORMAL;
        }
    }

    case EMACS__K_REALLOCATE:
    //     old-buffer-address new-size int to hold buffer
    //    address
{
    void *new_mem;
    void *old_mem = va_arg( args, void * );
    int size = va_arg( args, int );
    void **p = va_arg( args, void ** );
    if( (new_mem = realloc_ustr( old_mem, size )) == NULL )
    {
        *p = NULL;
        return SS$_INSFMEM;
    }
    else
    {
        *p = new_mem;
        return SS$_NORMAL;
    }
}


    case EMACS__K_FREE:
    //     buffer-to-free
    {
        free( va_arg( args, void * ) );
        return SS$_NORMAL;
    }

    case EMACS__K_BUFFER_SIZE:
    //     int to hold buffer size
    {
        int *p = va_arg( args, int * );
        *p = bf_cur->num_characters() + 1 - bf_cur->first_character();
        return SS$_NORMAL;
    }

    case EMACS__K_DOT:
    //     int to hold numeric-value of dot desc to return
    //    buffer-name in int to hold buffer-name length!
    {
        int *p_int = va_arg( args, int * );
        struct dsc$descriptor *p_str = va_arg( args, struct dsc$descriptor * );
        unsigned short int *p_size = va_arg( args, unsigned short int * );

        unsigned short int len;
        unsigned char * addr;
        unsigned short int maximum;
        *p_int = dot;
        len = _str_len( bf_cur->b_buf_name );
        if( ! (rv = VMS_SUCCESS(str$copy_r
                (
                p_str,
                &len,
                bf_cur->b_buf_name))) )
            return rv;
        lib$analyze_sdesc( p_str, &maximum, &addr );
        *p_size = min( len, maximum );
        return SS$_NORMAL;
    }

    case EMACS__K_MARK:
    //    int to hold numeric-value of dot desc to return
    //    buffer-name in int to hold buffer-name length
    {
        int *p_int = va_arg( args, int * );
        struct dsc$descriptor *p_str = va_arg( args, struct dsc$descriptor * );
        unsigned short int *p_size = va_arg( args, unsigned short int * );

        unsigned short int len;
        unsigned char *addr;
        int pos;
        unsigned short int maximum;
        Marker *m;
        m = bf_cur->b_mark;
        rv = SS$_NORMAL;
        if( m == NULL )
        {
            pos = 0;
            error( "No mark set in buffer \"%s\"!", bf_cur->b_buf_name );
            rv = SS$_IVSSRQ;
        }
        else
            pos = to_mark( m );
        *p_int = pos;
        len = _str_len( bf_cur->b_buf_name );
        if( ! (rv = VMS_SUCCESS(str$copy_r
                (
                p_str,
                &len,
                bf_cur->b_buf_name))) )
            return rv;
        lib$analyze_sdesc( p_str, &maximum, &addr );
        p_str->dsc$w_length = min( len, maximum );
        return rv;
    }

    case EMACS__K_SET_MARK:
    //     true - set, false - unset
    {
        if( va_arg( args, int ) )
        {
            if( bf_cur->b_mark == 0 )
                bf_cur->b_mark = EMACS_NEW Marker();
            set_mark( bf_cur->b_mark, bf_cur, dot, 0 );
            cant_1line_opt = 1;
        }
        else
            bf_cur->set_mark();
        return SS$_NORMAL;
    }

    case EMACS__K_USE_BUFFER:
    //     buffer-name-desc
    {
        if( ! (rv = vms_to_c( va_arg( args, struct dsc$descriptor * ) )) )
            return rv;
        set_bfn( call_back_str );
        return SS$_NORMAL;
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
        return SS$_NORMAL;
    }

    case EMACS__K_INSERT_CHARACTER:
    //     char to insert
    {
        unsigned char c = va_arg( args, unsigned char );
        ins_cstr( &c, 1 );
        return SS$_NORMAL;
    }

    case EMACS__K_INSERT_STRING:
    //     desc of string to insert
    {
        unsigned short int len;
        unsigned char *addr;
        if( ! ( rv = VMS_SUCCESS(lib$analyze_sdesc( va_arg( args, struct dsc$descriptor * ), &len, &addr ))) )
        {
            error( "Error `%s' during EMACS call back", fetch_os_error( rv ) );
            return rv;
        }
        ins_cstr( addr, len );
        return SS$_NORMAL;
    }

    case EMACS__K_DELETE_CHARACTERS:
    //     number of chars to delete, -iv means backwards
    {
        int cnt;
        int repeat;
        int forw;
        forw = 1;
        cnt = va_arg( args, int );
        if( cnt < 0 )
        {
            cnt = -cnt;
            forw = 0;
        }
        if( (repeat = del_chars_in_buffer( dot, cnt, forw )) != 0 )
            if( forw )
                dot_right( repeat );
        if( ! forw )
            dot_left( cnt );
        return SS$_NORMAL;
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

        return SS$_NORMAL;
    }

    case EMACS__K_RETURN_STRING_TO_PARAM:
    //     argument-number string-desc
    {
        unsigned short int len;
        unsigned char *addr;
        ProgramNode *p;
        int argnum;
        argnum = va_arg( args, int ) - 1;

        if( ! (rv = VMS_SUCCESS(lib$analyze_sdesc( va_arg( args, struct dsc$descriptor * ), &len, &addr ))) )
        {
            error( "Error `%s' during EMACS call back", fetch_os_error (rv) );
            return rv;
        }
        if( cur_exec == 0
        || argnum < 0
        || argnum >= cur_exec->p_nargs )
        {
            error( "Error, Argument number %d out of range", argnum );
            return SS$_BADPARAM;
        }
        p = cur_exec->p_args[argnum].pa_node;
        if( p->p_proc != &bound_variable_node )
        {
            error( "Emacs Call back expects argument %d to be a variable name.", argnum+1 );
            return SS$_BADPARAM;
        }
        if( len == 0 )
            addr = u_str("");
        Expression val( EmacsString( EmacsString::copy, addr, len ) );
        perform_set( p->p_args[0].pa_name, val, 0 );
        return SS$_NORMAL;
    }

    case EMACS__K_RETURN_NUMBER_TO_PARAM:
    //     argument-number integer
    {
        ProgramNode *p;
        int argnum;
        argnum = va_arg( args, int ) - 1;

        if( cur_exec == 0
        || argnum < 0
        || argnum >= cur_exec->p_nargs )
        {
            error( "Error, Argument number %d out of range", argnum+1 );
            return SS$_BADPARAM;
        }
        p = cur_exec->p_args[argnum].pa_node;
        if( p->p_proc != &bound_variable_node )
        {
            error( "Emacs Call back expects argument %d to be a variable name.", argnum+1 );
            return SS$_BADPARAM;
        }
        Expression val( va_arg( args, int ) );
        perform_set( p->p_args[0].pa_name, val, 0,  );
        return SS$_NORMAL;
    }

    case EMACS__K_BUFFER_EXTENT: // s1, p1, s2, p2
        *va_arg( args, int *) = bf_s1;
        *va_arg( args, unsigned char * *) = bf_p1;
        *va_arg( args, int *) = bf_s2;
        *va_arg( args, unsigned char * *) = bf_p2;
        break;

    case EMACS__K_GAP_TO: // position
        gap_to( va_arg( args, int ) );
        break;

    case EMACS__K_GAP_ROOM: // size
        gap_room( va_arg( args, int ) );
        break;

    default:
        return SS$_BADPARAM;
}

    return SS$_NORMAL;
}

static int activate_image
    (
    unsigned char *image,
    unsigned char *symbol,
    int (**reference)(int (*call_back)(int,...), void *, int *),
    unsigned char *file
    )
{
    int rv;
    struct dsc$descriptor imagename;
    struct dsc$descriptor symbolname;
    struct dsc$descriptor filename;
    struct vms_itm_3 set_logical[2];
    unsigned int attr =LNM$M_CONFINE;

    lib$establish( activate_handler );

    DSC_SZ( imagename, image );
    DSC_SZ( symbolname, symbol );
    DSC_SZ( filename, file );

    // If the image is different from the filename define a logical * name
    // to point at the file!
    if( imagename.dsc$w_length != filename.dsc$w_length )
    {
        struct dsc$descriptor idesc;
        struct dsc$descriptor fdesc;
        struct dsc$descriptor tabnam = { 11,0,0,"LNM$PROCESS" };
        char istring[255];
        char fstring[255];

        idesc.dsc$w_length = imagename.dsc$w_length;
        idesc.dsc$b_dtype = 0;
        idesc.dsc$b_class = 0;
        idesc.dsc$a_pointer = istring;

        fdesc.dsc$w_length = filename.dsc$w_length;
        fdesc.dsc$b_dtype = 0;
        fdesc.dsc$b_class = 0;
        fdesc.dsc$a_pointer = fstring;

        // upcase the strings to keep VMS happy
        str$upcase( &fdesc, &filename );
        str$upcase( &idesc, &imagename );

        memset( set_logical, 0, sizeof( set_logical ) );
        set_logical[0].w_item_code = LNM$_STRING;
        set_logical[0].a_buf_addr = u_str(fstring);
        set_logical[0].w_length = fdesc.dsc$w_length;

        if( !VMS_SUCCESS(sys$crelnm
            (
            &attr,
            &tabnam,
            &idesc,
            NULL,
            set_logical
            )) )
        {
            error( "Error defining logical name %s as %s", image, file );
            return 0;
        }
    }
    rv = lib$find_image_symbol( &imagename, &symbolname, (void **)reference );
    if( ! VMS_SUCCESS(rv) )
    {
        error( "Error while defining %s in image %s, %s",
                symbol, image, fetch_os_error( rv ) );
        return 0;
    }
    return 1;
}

//
// Activate the required bit of code.
//
// check_ to see when the code was last merge activiated[ This ]is coded in the
// extern_func block which is used to contain information about the external
// function. Offset ef_restore_count is set to the value that was the
// value of the is_restored variable when LIB$FIND_IMAGE_SYMBOL was last
// called. If ef_restore_count isnt correct, then its time to re-activate
// the code.
//
// The function is called, and the result accepted as a standrad VAX/VMS
// return status code. For success, EMACS continues to execute. Otherwise
// the return status is used to indicate that EMACS is to exit.
//

int BoundNameExternalFunction::activate_external_function( BoundName *b )
{
    int rv;
    struct extern_func *ef;
    int isrest;
    Expression *old_result;
    Expression result;

    old_result = call_back_result;
    ef = b->b_bound.b_func;

    lib$establish( activate_handler );

    call_back_result = &result;
    result.exp_type = ISVOID;
    result.exp_int = 0;
    result.exp_v.v_string = NULL;
    result.exp_release = 0;
    if( ef->ef_restore_count != is_restored )
        if( ! (activate_image
            (
            images[ ef->ef_image_index ],
            ef->ef_funcname,
            &ef->ef_function,
            image_filenames[ ef->ef_image_index ])) )
                return 0;
        else
            ef->ef_restore_count = is_restored;
    if( image_context_lost )
        ef->ef_context = 0;
    isrest = is_restored;
    rv = ef->ef_function( call_back, ef->ef_context, &isrest );
    if( ! VMS_SUCCESS(rv) )
    {
        error( "Function %s failed with status %s",
                b->b_proc_name, fetch_os_error( rv ) );
        void_result();
    }
    else
    {
        ml_value = result;
        call_back_result = old_result;
    }
    return 0;
}
#endif
