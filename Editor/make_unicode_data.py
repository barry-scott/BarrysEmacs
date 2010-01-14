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

    to_upper = {}
    to_lower = {}
    to_title = {}

    for line in ucd:
        parts = line.strip().split(';')
        code = parts[0]
        lower = parts[12]
        upper = parts[13]
        title = parts[14]
        if len(upper) > 0:
            to_upper[ code ] = upper
        if len(lower) > 0:
            to_lower[ code ] = lower
        if len(title) > 0:
            to_title[ code ] = title

    print 'to_upper',len(to_upper)
    print 'to_lower',len(to_lower)
    print 'to_title',len(to_title)

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
struct unicode_data
{
    unsigned int code_point;
    unsigned int replacement;
};

''' % (argv[0], time.strftime( '%Y-%d-%m %H:%M:%S' )) )

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
