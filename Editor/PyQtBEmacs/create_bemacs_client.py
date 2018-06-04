#!/usr/bin/env python3
import sys
import os

def main( argv ):
    src_dir = argv[1]
    cli_client_path = argv[2]

    path = '%s/be_client.py' % (src_dir,)
    with open( path, 'r', encoding='utf-8' ) as f:
        print( 'Info: Reading %s' % (path,) )
        __text = f.read()

    __text = __text.replace( 'org.barrys-emacs.bemacs-devel', 'org.barrys-emacs.bemacs' )
    __text = __text.replace( '#!/usr/bin/env python3\n', '#!/usr/bin/python\n' )

    with open( cli_client_path, 'w', encoding='utf-8' )  as f:
        print( 'Info: Writing %s' % (cli_client_path,) )
        f.write( __text )

    print( 'Info: chmod 555 %s' % (cli_client_path,) )
    os.chmod( cli_client_path, 0o555 )
    return 0

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )


