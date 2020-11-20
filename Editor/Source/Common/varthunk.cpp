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
    int index = 1;
    // first put the file buffers - same as list-buffers order
    for( int name = 0; name<EmacsBuffer::name_table.entries(); name++ )
    {
        if( EmacsBuffer::name_table.value( name )->b_kind == FILEBUFFER )
        {
            a(index) = *EmacsBuffer::name_table.key( name );
            ++index;
        }
    }
    // then put the none file buffers
    for( int name = 0; name<EmacsBuffer::name_table.entries(); name++ )
    {
        if( EmacsBuffer::name_table.value( name )->b_kind != FILEBUFFER )
        {
            a(index) = *EmacsBuffer::name_table.key( name );
            ++index;
        }
    }

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
