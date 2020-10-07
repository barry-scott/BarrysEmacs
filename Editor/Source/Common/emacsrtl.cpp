//
//    Emacs_rtl.c
//
//        Copyright (c) 1992 Barry A. Scott
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <ctype.h>

int _dbg_fn_trace::callDepth()
{
    return s_call_depth;
}

int _dbg_fn_trace::s_call_depth(0);

_dbg_fn_trace::_dbg_fn_trace( const EmacsString &fn_name, bool enabled )
: m_enabled( enabled )
, m_fn_name( fn_name )
, m_result()
, m_call_depth( s_call_depth )
{
    if( m_enabled )
    {
        s_call_depth++;

        _dbg_msg( FormatString("%*sEnter[%d]: %s") << (m_call_depth*2) << EmacsString::null << m_call_depth << m_fn_name );
    }
}

_dbg_fn_trace::~_dbg_fn_trace()
{
    if( m_enabled )
    {
        if( m_result.isNull() )
        {
            _dbg_msg( FormatString("%*sLeave[%d]: %s") << (m_call_depth*2) << EmacsString::null << m_call_depth << m_fn_name );
        }
        else
        {
            _dbg_msg( FormatString("%*sLeave[%d]: %s -> %s") << (m_call_depth*2) << EmacsString::null << m_call_depth << m_fn_name << m_result );
        }

        s_call_depth = m_call_depth;
    }
}

void _dbg_fn_trace::_msg( const EmacsString &msg )
{
    if( m_enabled )
    {
       _dbg_msg( FormatString("%*sTrace[%d]: %s") << (m_call_depth*2) << EmacsString::null << m_call_depth << msg );
    }
}

void _dbg_fn_trace::_result( const EmacsString &result )
{
    m_result = result;
}
