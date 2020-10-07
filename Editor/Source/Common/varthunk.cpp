//module varthunk
//    (
//    ident    = 'V5.0 Emacs',
//    addressing_mode(  nonexternal=long_relative, external=general  )
//    ) =
//begin
//     Copyright(c ) 1982, 1983, 1984, 1985
//        Barry A. Scott and nick Emery

//
//    Copyright 1985 Barry A. Scott & Nick Emery
//
//    This module contains check and thunk routines for
//    system variables.
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


void SystemExpressionRepresentationBufferNames::assign_value( ExpressionRepresentation * )
{
    throw EmacsExceptionVariableReadOnly();
}

//
//    Need to allocate the array and fill it in with
//    the buffer names
//
void SystemExpressionRepresentationBufferNames::fetch_value()
{
    int n_buffers = EmacsBuffer::name_table.entries();

    // create a new array of one dimension
    EmacsArray a( 0, n_buffers );

    // index 0 is the number of entries
    a(0) = n_buffers;
    // index 1 to n is the name of the buffer
    for( int index = 0; index<EmacsBuffer::name_table.entries(); index++ )
        a(index+1) = *EmacsBuffer::name_table.key( index );

    // copy the array into the representation storage
    exp_array = a;
}

void SystemExpressionRepresentationTermOutputSize::assign_value( ExpressionRepresentation *e )
{
    int new_buffer_size = e->asInt();
    if( new_buffer_size < 0 )
        throw EmacsExceptionVariablePositive();
    exp_int = new_buffer_size;
}
void SystemExpressionRepresentationGraphicRendition::fetch_value()
{ }
void SystemExpressionRepresentationTermOutputSize::fetch_value()
{ }
