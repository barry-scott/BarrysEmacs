#!/usr/bin/env python3
#
#   make_unicode_data.py
#
#   Copyright (c) 2010-2017  Barry A. Scott
#
import sys
import os
import time

def main( argv ):
    # uncode data
    if sys.version_info[0] == 3:
        with open( argv[1], 'r', encoding='utf-8' ) as f:
            ucd = f.read()
    else:
        with open( argv[1], 'rb' ) as f:
            ucd = f.read().decode('utf-8')

    alphabetic = set()
    numeric = set()
    to_upper = {}
    to_lower = {}
    to_title = {}
    is_upper = set()
    is_lower = set()
    is_title = set()

    last_code_point = None

    for line in ucd.split('\n'):
        if line == '':
            continue
        parts = line.strip().split(';')
        code = parts[0]
        category = parts[2]
        upper = parts[12]
        lower = parts[13]
        title = parts[14]

        last_code_point = int( code, 16 )

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

    print( 'alphabetic', len(alphabetic) )
    print( 'numeric', len(numeric) )
    print( 'to_upper', len(to_upper) )
    print( 'to_lower', len(to_lower) )
    print( 'to_title', len(to_title) )
    print( 'is_upper', len(is_upper) )
    print( 'is_lower', len(is_lower) )
    print( 'is_title', len(is_title) )
    print( 'last code point', last_code_point )

    # case folding
    if sys.version_info[0] == 3:
        with open( argv[2], 'r', encoding='utf-8' ) as f:
            ucd = f.read()
    else:
        with open( argv[2], 'rb' ) as f:
            ucd = f.read().decode('utf-8')

    casefold = {}

    for line in ucd.split('\n'):
        line = line.strip()
        if len(line) == 0:
            continue

        line, comment = line.split( '#', 1 )
        line = line.strip()
        if len(line) == 0:
            continue

        code, status, mapping = [v.strip() for v in line.split(';')][0:3]
        # choose common C and simple S not full F or turkic T
        if status in ('C','S'):
            casefold[ code ] = mapping

    print( 'casefold', len(casefold) )

    cxx = [
u'''//
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

''' % (argv[0], time.strftime( '%Y-%d-%m %H:%M:%S' ))]

    cxx.append( u'const int unicode_max_code_point( 0x%x );\n' % (last_code_point,) )

    cxx.append( u'struct unicode_category unicode_init_alphabetic[ %d ] = {\n' % (len(alphabetic)+1,))
    for code in sorted( alphabetic ):
        cxx.append( u'    {0x%s},\n' % (code,) )

    cxx.append( u'    {0x0000}\n' )
    cxx.append( u'};\n\n' )

    cxx.append( u'struct unicode_category unicode_init_numeric[ %d ] = {\n' % (len(numeric)+1,))
    for code in sorted( numeric ):
        cxx.append( u'    {0x%s},\n' % (code,) )

    cxx.append( u'    {0x0000}\n' )
    cxx.append( u'};\n\n' )

    cxx.append( u'struct unicode_category unicode_init_is_upper[ %d ] = {\n' % (len(is_upper)+1,))
    for code in sorted( is_upper ):
        cxx.append( u'    {0x%s},\n' % (code,) )

    cxx.append( u'    {0x0000}\n' )
    cxx.append( u'};\n\n' )

    cxx.append( u'struct unicode_category unicode_init_is_lower[ %d ] = {\n' % (len(is_lower)+1,))
    for code in sorted( is_lower ):
        cxx.append( u'    {0x%s},\n' % (code,) )

    cxx.append( u'    {0x0000}\n' )
    cxx.append( u'};\n\n' )

    cxx.append( u'struct unicode_category unicode_init_is_title[ %d ] = {\n' % (len(is_title)+1,))
    for code in sorted( is_title ):
        cxx.append( u'    {0x%s},\n' % (code,) )

    cxx.append( u'    {0x0000}\n' )
    cxx.append( u'};\n\n' )

    cxx.append( u'struct unicode_data unicode_init_to_upper[ %d ] = {\n' % (len(to_upper)+1,))
    for code, value in sorted( to_upper.items() ):
        cxx.append( u'    {0x%s, 0x%s},\n' % (code, value) )

    cxx.append( u'    {0x0000, 0x0000}\n' )
    cxx.append( u'};\n\n' )

    cxx.append( u'struct unicode_data unicode_init_to_lower[ %d ] = {\n' % (len(to_lower)+1,))
    for code, value in sorted( to_lower.items() ):
        cxx.append( u'    {0x%s, 0x%s},\n' % (code, value) )

    cxx.append( u'    {0x0000, 0x0000}\n' )
    cxx.append( u'};\n\n' )

    cxx.append( u'struct unicode_data unicode_init_to_title[ %d ] = {\n' % (len(to_title)+1,))
    for code, value in sorted( to_title.items() ):
        cxx.append( u'    {0x%s, 0x%s},\n' % (code, value) )

    cxx.append( u'    {0x0000, 0x0000}\n' )
    cxx.append( u'};\n\n' )

    cxx.append( u'struct unicode_data unicode_init_casefold[ %d ] = {\n' % (len(casefold)+1,))
    for code, value in sorted( casefold.items() ):
        cxx.append( u'    {0x%s, 0x%s},\n' % (code, value) )

    cxx.append( u'    {0x0000, 0x0000}\n' )
    cxx.append( u'};\n\n' )

    cxx.append( u'// end of file\n' )

    if sys.version_info[0] == 3:
        with open( argv[3], 'w', encoding='utf-8' ) as f:
            f.write( ''.join( cxx ) )
    else:
        with open( argv[3], 'wb' ) as f:
            f.write( ''.join( cxx ).encode('utf-8') )

    return 0

sys.exit( main( sys.argv ) )
