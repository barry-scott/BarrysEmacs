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

_dbg_fn_trace::_dbg_fn_trace( const EmacsString &fn_name )
    : m_fn_name( fn_name )
    , m_call_depth( s_call_depth )
{
    s_call_depth++;

    _dbg_msg( FormatString("Enter: %s()") << m_fn_name );
}

_dbg_fn_trace::~_dbg_fn_trace()
{
    _dbg_msg( FormatString("Leave: %s()") << m_fn_name );

    s_call_depth = m_call_depth;
}
