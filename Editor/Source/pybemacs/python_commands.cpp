//
//  Copyright (c) 2019-2019 Barry A. Scott
//
//
//  python_commands.cpp
//
#include <emacs.h>

#include "bemacs_python.hpp"

int python_import_module(void)
{
    EmacsString module_name;

    if( cur_exec == NULL )
    {
        EmacsString prompt( ": Python-import " );

        module_name = get_string_interactive( prompt );
    }
    else
    {
        if( check_args( 1, 1 ) )
            return 0;

        module_name = get_string_mlisp();
    }
}

int python_call_function(void)
{
    if( check_args( 1, 0 ) )
        return 0;

    if( !string_arg( 1 ) )
        return 0;

    Py::String function = ml_value.asString().asPyString();

    const int first_arg = 2;
    Py::Tuple args( cur_exec->p_nargs - first_arg );
    for( int arg=first_arg; !ml_err && arg<=cur_exec->p_nargs; arg++ )
    {
        if( !eval_arg( arg ) )
            return 0;

        switch( ml_value.exp_type() )
        {
        case ISINTEGER:
        case ISSTRING:
            args[ arg - first_arg ] = convertEmacsExpressionToPyObject( ml_value );
            break;

        case ISVOID:
        case ISMARKER:
        case ISWINDOWS:
        case ISARRAY:
            error( FormatString("Python-call - unsupported expression type for arg %d") << arg );
            return 0;
        }
    }

    try
    {
        Py::Module module( "be_user" );
        Py::Dict dict( module.getDict() );

        Py::Callable be_call( dict[ "be_call" ] );
        Py::Object result = be_call.apply( args );
        ml_value = convertPyObjectToEmacsExpression( result );
        return 0;
    }
    catch( Py::Exception &e )
    {
        Py::Object err( PyErr_Occurred(), false );
        e.clear();
        Py::String str( err.str() );

        error( FormatString("Python-call error %s") << str.as_std_string() );
        return 0;
    }
}

int python_exec_string(void)
{
    EmacsString expression;

    if( cur_exec == NULL )
    {
        EmacsString prompt( ": Python-exec " );

        expression = get_string_interactive( prompt );
    }
    else
    {
        if( check_args( 1, 1 ) )
            return 0;

        expression = get_string_mlisp();
    }

    try
    {
        Py::Module module( "be_user" );
        Py::Dict dict( module.getDict() );

        Py::Callable be_exec( dict[ "be_exec" ] );
        Py::TupleN args( convertEmacsExpressionToPyObject( expression ) );

        Py::Object result = be_exec.apply( args );
        ml_value = convertPyObjectToEmacsExpression( result );
        return 0;
    }
    catch( Py::Exception &e )
    {
        Py::Object err( PyErr_Occurred(), false );
        e.clear();
        Py::String str( err.str() );

        error( FormatString("Python-exec error %s") << str.as_std_string() );
        return 0;
    }
}

int python_eval_string(void)
{
    EmacsString expression;

    if( cur_exec == NULL )
    {
        EmacsString prompt( ": Python-eval " );

        expression = get_string_interactive( prompt );
    }
    else
    {
        if( check_args( 1, 1 ) )
            return 0;

        expression = get_string_mlisp();
    }

    try
    {
        Py::Module module( "be_user" );
        Py::Dict dict( module.getDict() );

        Py::Callable be_eval( dict[ "be_eval" ] );
        Py::TupleN args( convertEmacsExpressionToPyObject( expression ) );

        Py::Object result = be_eval.apply( args );
        ml_value = convertPyObjectToEmacsExpression( result );
        return 0;
    }
    catch( Py::Exception &e )
    {
        Py::Object err( PyErr_Occurred(), false );
        e.clear();
        Py::String str( err.str() );

        error( FormatString("Python-eval error %s") << str.as_std_string() );
        return 0;
    }
}
