#
#	unix_build.py
#
import sys
import os
import string


def main():
	app_name = os.path.basename(sys.argv[0])
	if len(sys.argv) != 5:
		print 'Usage:',app_name,'os-type CFG build-host build-dir'
		return 1

	os_type = sys.argv[1]
	cfg = sys.argv[2]
	build_host = sys.argv[3]
	build_dir_win = sys.argv[4]

	if string.lower( build_dir_win )[:3] != 'x:\\':
		print app_name,'build-dir must be on X: -',build_dir_win
		return 1

	build_dir_unix = string.join( string.split( '$HOME/'+build_dir_win[3:], '\\' ), '/' )

	command = 'rsh %(build_host)s -l barry "cd %(build_dir_unix)s/_build;gmake -f unix.mak %(os_type)s BUILD_CFG=%(cfg)s BUILD_OS=%(os_type)s BUILD_DIR=%(build_dir_unix)s' % locals()
	print 'Info:',command
	sys.stdout.flush()
	os.system( command )


if __name__ == "__main__":
	main()
