#include <cstdlib>
#include <iostream>
#include <CXX/Objects.hxx>
#include <emacs.h>

#define outputSizeOf( type ) "sizeof( " #type " ) " << sizeof( type )

template<typename T> void _describeType( const char *T_name )
{
    T cp( -1 );
    if( cp < 0 )
    {
        std::cout << T_name << " signed " << sizeof( T ) << std::endl;
    }
    else
    {
        std::cout << T_name << " unsigned " << sizeof( T ) << std::endl;
    }
}
#define describeType( type ) _describeType<type>( #type )

int main( int argc, char **argv )
{
    std::cout << "python types" << std::endl;

    describeType( int );
    describeType( unsigned int );
    describeType( wchar_t );
    describeType( Py_UNICODE );
    describeType( Py_UCS1 );
    describeType( Py_UCS2 );
    describeType( Py_UCS4 );
    describeType( EmacsChar_t );
    describeType( DisplayBody_t );
    describeType( PY_UNICODE_TYPE );
    describeType( Py_UNICODE );
    return 0;
}

