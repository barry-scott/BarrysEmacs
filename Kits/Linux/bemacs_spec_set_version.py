import sys

with open( 'bemacs.spec.template', 'r', encoding='utf-8' ) as f:
    contents = f.read()

contents = contents.replace( 'BEMACS-VERSION', sys.argv[1] )

with open( 'bemacs.spec', 'w', encoding='utf-8' ) as f:
    f.write( contents )
