#
#   make_unicode_data.py
#
#   Copyright (c) 2010  Barry A. Scott
#
import sys
import os
import time

def main( argv ):

    # uncode data
    ucd = open( argv[1], 'r' )

    alphabetic = set()
    numeric = set()
    to_upper = {}
    to_lower = {}
    to_title = {}
    is_upper = set()
    is_lower = set()
    is_title = set()

    for line in ucd:
        parts = line.strip().split(';')
        code = parts[0]
        category = parts[2]
        upper = parts[12]
        lower = parts[13]
        title = parts[14]

        if category.startswith( 'Lu' ):
            alphabetic.add( code )
            is_upper.add( code )

        if category.startswith( 'Ll' ):
            alphabetic.add( code )
            is_lower.add( code )

        if category.startswith( 'Lt' ):
            alphabetic.add( code )
            is_title.add( code )

        if category.startswith( 'N' ):
            numeric.add( code )

        if len(upper) > 0:
            to_upper[ code ] = upper
        if len(lower) > 0:
            to_lower[ code ] = lower
        if len(title) > 0:
            to_title[ code ] = title

    print 'alphabetic',len(alphabetic)
    print 'numeric',len(numeric)
    print 'to_upper',len(to_upper)
    print 'to_lower',len(to_lower)
    print 'to_title',len(to_title)
    print 'is_upper',len(is_upper)
    print 'is_lower',len(is_lower)
    print 'is_title',len(is_title)

    ucd.close()

    # case folding
    ucd = open( argv[2], 'r' )

    casefold = {}

    for line in ucd:
        line, comment = line.split( '#', 1 )
        if line.strip() == '':
            continue

        code, status, mapping = [v.strip() for v in line.strip().split(';')][0:3]
        # choose common C and simple S not full F or turkic T
        if status in ('C','S'):
            casefold[ code ] = mapping

    ucd.close()

    print 'casefold', len(casefold)

    cxx = open( argv[3], 'w' )

    cxx.write( 
'''//
//  written by %s on %s
//
struct unicode_category
{
    unsigned int code_point;
};

struct unicode_data
{
    unsigned int code_point;
    unsigned int replacement;
};

''' % (argv[0], time.strftime( '%Y-%d-%m %H:%M:%S' )) )

    cxx.write( 'struct unicode_category unicode_init_alphabetic[ %d ] = {\n' % (len(alphabetic)+1,))
    for code in sorted( alphabetic ):
        cxx.write( '    {0x%s},\n' % (code,) )

    cxx.write( '    {0x0000}\n' )
    cxx.write( '};\n\n' )

    cxx.write( 'struct unicode_category unicode_init_numeric[ %d ] = {\n' % (len(numeric)+1,))
    for code in sorted( numeric ):
        cxx.write( '    {0x%s},\n' % (code,) )

    cxx.write( '    {0x0000}\n' )
    cxx.write( '};\n\n' )

    cxx.write( 'struct unicode_category unicode_init_is_upper[ %d ] = {\n' % (len(is_upper)+1,))
    for code in sorted( is_upper ):
        cxx.write( '    {0x%s},\n' % (code,) )

    cxx.write( '    {0x0000}\n' )
    cxx.write( '};\n\n' )

    cxx.write( 'struct unicode_category unicode_init_is_lower[ %d ] = {\n' % (len(is_lower)+1,))
    for code in sorted( is_lower ):
        cxx.write( '    {0x%s},\n' % (code,) )

    cxx.write( '    {0x0000}\n' )
    cxx.write( '};\n\n' )

    cxx.write( 'struct unicode_category unicode_init_is_title[ %d ] = {\n' % (len(is_title)+1,))
    for code in sorted( is_title ):
        cxx.write( '    {0x%s},\n' % (code,) )

    cxx.write( '    {0x0000}\n' )
    cxx.write( '};\n\n' )

    cxx.write( 'struct unicode_data unicode_init_to_upper[ %d ] = {\n' % (len(to_upper)+1,))
    for code, value in sorted( to_upper.items() ):
        cxx.write( '    {0x%s, 0x%s},\n' % (code, value) )

    cxx.write( '    {0x0000, 0x0000}\n' )
    cxx.write( '};\n\n' )

    cxx.write( 'struct unicode_data unicode_init_to_lower[ %d ] = {\n' % (len(to_lower)+1,))
    for code, value in sorted( to_lower.items() ):
        cxx.write( '    {0x%s, 0x%s},\n' % (code, value) )

    cxx.write( '    {0x0000, 0x0000}\n' )
    cxx.write( '};\n\n' )

    cxx.write( 'struct unicode_data unicode_init_to_title[ %d ] = {\n' % (len(to_title)+1,))
    for code, value in sorted( to_title.items() ):
        cxx.write( '    {0x%s, 0x%s},\n' % (code, value) )

    cxx.write( '    {0x0000, 0x0000}\n' )
    cxx.write( '};\n\n' )

    cxx.write( 'struct unicode_data unicode_init_casefold[ %d ] = {\n' % (len(casefold)+1,))
    for code, value in sorted( casefold.items() ):
        cxx.write( '    {0x%s, 0x%s},\n' % (code, value) )

    cxx.write( '    {0x0000, 0x0000}\n' )
    cxx.write( '};\n\n' )


    cxx.write( '// end of file\n' )
    cxx.close()
    return 0

sys.exit( main( sys.argv ) )
