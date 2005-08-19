import sys
import win32api
import struct
import win32con
import os

def main(args):
	import os.path, string

	if len(args) >= 2 and args[0] == 'show':
		show_command( args[1:] )
	elif len(args) >= 2 and args[0] == 'make':
		make_command( args[1:] )
	elif len(args) >= 2 and args[0] == 'default':
		make_command( [args[1], 'main'] )
	else:
		print 'Usage:'
		print '%s show <bmeacs-exe>' % os.path.basename( sys.argv[0] )
		print '     - show the command paramaters in the <bemacs-exe>'
		print '%s default <bmeacs-exe>' % os.path.basename( sys.argv[0] )
		print '     - set the command paramters in <bemacs-exe> to the defaults'
		print '%s make <bemacs-exe> <cmd-prefix> <cmd-suffix>' % os.path.basename( sys.argv[0] )
		print '     - make a customised bemacs command.'

def show_command( args ):
	exe_filename = args[0]
	print "Info: Show BEmacs command in", exe_filename
	hsrc = win32api.LoadLibraryEx( exe_filename, 0, win32con.LOAD_LIBRARY_AS_DATAFILE )
	procname = win32api.LoadString( hsrc, 1 )
	cmd_prefix = win32api.LoadString( hsrc, 2 )
	cmd_suffix = win32api.LoadString( hsrc, 3 )
	win32api.FreeLibrary( hsrc )

	print 'Info:   Process name: "%s"'%procname
	print 'Info: Command prefix: "%s"'%cmd_prefix
	print 'Info: Command suffix: "%s"'%cmd_suffix

	print
	print 'Info: %s is bemacs.exe /name=%s %s <users-cmd-args> %s' % (os.path.basename( exe_filename ), procname, cmd_prefix, cmd_suffix)


def make_command( args ):
	exe_filename = args[0]
	print "Info: Make BEmacs command in", exe_filename
	
	procname = ''
	cmd_prefix = ''
	cmd_suffix = ''

	if len(args) > 1:
		procname = args[1]
	if len(args) > 2:
		cmd_prefix = args[2]
	if len(args) > 3:
		cmd_suffix = args[3]
	if procname == '':
		procname = 'main'
	if cmd_prefix == '':
		cmd_prefix = ' '
	if cmd_suffix == '':
		cmd_suffix = ' '

	print 'Info:   Process name: "%s"'%procname
	print 'Info: Command prefix: "%s"'%cmd_prefix
	print 'Info: Command suffix: "%s"'%cmd_suffix

	print
	print 'Info: %s is bemacs.exe /name=%s %s <users-cmd-args> %s' % (os.path.basename( exe_filename ), procname, cmd_prefix, cmd_suffix)

	data = [struct.pack( 'H', 0 )]
	data.append( struct.pack( 'H', len( procname ) ) )
	data.extend( [struct.pack( 'H', ord(c) ) for c in procname] )
	data.append( struct.pack( 'H', len( cmd_prefix ) ) )
	data.extend( [struct.pack( 'H', ord(c) ) for c in cmd_prefix] )
	data.append( struct.pack( 'H', len( cmd_suffix ) ) )
	data.extend( [struct.pack( 'H', ord(c) ) for c in cmd_suffix] )
	data.extend( [struct.pack( 'H', 0 ) for i in range(16-4)] )
	data = ''.join( data )

	hdst = win32api.BeginUpdateResource( exe_filename, 0 )
	win32api.UpdateResource( hdst, win32con.RT_STRING, 1, data )
	win32api.EndUpdateResource( hdst, 0 )


if __name__ == '__main__':
	main( sys.argv[1:] )
