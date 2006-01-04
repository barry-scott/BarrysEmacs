'''

    bemacs_stdio.py


    initialise the python world for Barry's Emacs use

'''
import sys
import bemacs

class BEmacs_stdout:
    buffer_name = 'Python Console'
    def __init__(self):
        pass

    def write( self, text ):
        bemacs.function.temp_use_buffer( BEmacs_stdout.buffer_name )
        bemacs.function.end_of_file()
        bemacs.function.insert_string( text )

    def flush( self ):
        pass


class BEmacs_stdin:
    def __init__(self):
        pass

    def read( self, size=0 ):
        raise IOError, 'No stdin available'

    def readline( self ):
        return bemacs.function.get_tty_string( 'Python stdin: ' ) + '\n'

    def readlines( self ):
        raise IOError, 'No stdin available'

    def flush( self ):
        pass

def init():
    sys.__stdin__ = BEmacs_stdin()
    sys.__stderr__ = BEmacs_stdout()
    sys.__stdout__ = sys.__stderr__

    sys.stdin = sys.__stdin__
    sys.stdout = sys.__stdout__
    sys.stderr = sys.__stderr__

    # this should appear in the buffer
    print 'bemacs_stdio.py done'


init()
