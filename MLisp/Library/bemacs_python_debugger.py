#
#	bemacs_python_debugger.py
#
import sys
import pdb
import os
import repr

class MyPdb(pdb.Pdb):
	def __init__(self):
		pdb.Pdb.__init__(self)
		self.prompt = '\020'		# C-P prompt
		self.use_rawinput = 0

		repr.aRepr.maxstring = 100

	def interaction(self, frame, traceback, feedback=None):
		entry_stdin = sys.stdin
		entry_stdout = sys.stdout
		entry_stderr = sys.stderr

		sys.stdin = sys.__stdin__
		sys.stdout = sys.__stdout__
		sys.stderr = sys.__stdout__

		sys.stdout.write( '\002\n' )	# C-B begin
		if feedback is not None:
			sys.stdout.write( feedback )
			sys.stdout.write( '\n' )
		pdb.Pdb.interaction( self, frame, traceback )
		sys.stdout.write( '\005' )	# C-E end
		sys.stdout.flush()

		sys.stdin = entry_stdin
		sys.stdout = entry_stdout
		sys.stderr = entry_stderr

	def user_return(self, frame, return_value):
		"""This function is called when a return trap is set here."""
		frame.f_locals['__return__'] = return_value
		self.interaction(frame, None, '--- Return ---')

# Simplified interface

def run(statement, globals=None, locals=None):
    MyPdb().run(statement, globals, locals)

# When invoked as main program, invoke the debugger on a script
if __name__=='__main__':
    if not sys.argv[1:]:
        print( "usage: pdb.py scriptfile [arg] ..." )
        sys.exit(2)

    mainpyfile = filename = sys.argv[1]     # Get script filename
    if not os.path.exists(filename):
        print( 'Error:', `filename`, 'does not exist' )
        sys.exit(1)
    mainmodule = os.path.basename(filename)
    del sys.argv[0]         # Hide "pdb.py" from argument list

    # Insert script directory in front of module search path
    sys.path.insert(0, os.path.dirname(filename))

    run('execfile(' + `filename` + ')')
