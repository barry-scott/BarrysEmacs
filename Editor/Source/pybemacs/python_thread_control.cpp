//
//  Copyright (c) 2010 Barry A. Scott
//
//
//  python_thread_control.cpp
//
#include <emacs.h>

#include "bemacs_python.hpp"

//--------------------------------------------------------------------------------
BemacsEditorAccessControl::BemacsEditorAccessControl()
: m_saved_thread_state( NULL )
{}

BemacsEditorAccessControl::~BemacsEditorAccessControl()
{
    emacs_assert( m_saved_thread_state == NULL );
}

void BemacsEditorAccessControl::allowOtherThreads()
{
    emacs_assert( m_saved_thread_state == NULL );

    m_saved_thread_state = PyEval_SaveThread();

    emacs_assert( m_saved_thread_state != NULL );
}

void BemacsEditorAccessControl::allowThisThread()
{
    emacs_assert( m_saved_thread_state != NULL );

    PyEval_RestoreThread( m_saved_thread_state );

    m_saved_thread_state = NULL;
}

//--------------------------------------------------------------------------------
PythonAllowThreads::PythonAllowThreads( BemacsEditorAccessControl &control )
: m_control( control )
{
    m_control.allowOtherThreads();
}

PythonAllowThreads::~PythonAllowThreads()
{
    m_control.allowThisThread();
}

//--------------------------------------------------------------------------------
PythonDisallowThreads::PythonDisallowThreads( BemacsEditorAccessControl &control )
: m_control( control )
{
    m_control.allowThisThread();
}

PythonDisallowThreads::~PythonDisallowThreads()
{
    m_control.allowOtherThreads();
}
