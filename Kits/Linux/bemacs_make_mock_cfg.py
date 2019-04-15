#!/bin/python3
import sys
import os

def main( argv ):
    mock_orig_version_filename = argv[1]
    mock_copr_repo_filename = argv[2]
    output_filename = argv[3]

    with open( mock_orig_version_filename, 'r' ) as f:
        mock_cfg = compile( f.read(), 'mock_cfg', 'exec' )
        config_opts = {}
        exec( mock_cfg )

    with open( mock_copr_repo_filename, 'r' ) as f:
        repo = f.read()

        config_opts['yum.conf'] += '\n'
        config_opts['yum.conf'] += repo
        config_opts['root'] = os.path.splitext( os.path.basename( output_filename ) )[0]

    with open( output_filename, 'w' ) as f:
        for k in config_opts:
            if k == 'yum.conf':
                print( 'config_opts[\'yum.conf\'] = """', end='', file=f )
                print( config_opts['yum.conf'], file=f )
                print( '"""', file=f )

            else:
                print( 'config_opts[%r] = %r' % (k, config_opts[k]), file=f )

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
