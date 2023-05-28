#
#    create_library.py
#
#    %1 - file set [common|vms|motif|unix|windows]
#    %2 - library name
#    %3 - path to tools
#
import sys
import os
import subprocess

def main( argv ):
    file_sets = argv[1].split( ',' )
    lib_name = argv[2]
    tool_path = argv[3]

    print( 'Create MLisp Library %(lib_name)s with file set %(file_sets)s using tools from %(tool_path)s' % locals() )

    try:
        if tool_path == '--sqlite':
            dbtools = BemacsSqliteTools()
        else:
            dbtools = BemacsDatabaseTools( tool_path )

        createLibrary( file_sets, lib_name, dbtools )

    except ValueError as e:
        print( 'Error: %s' % (e,) )
        return 1

    return 0

def createLibrary( file_sets, lib_name, dbtools ):
    dbtools.create( lib_name )

    lib_folder = os.path.dirname( __file__ )

    for set_name, filename in library_files:
        if set_name in file_sets:
            dbtools.add( lib_name, os.path.join( lib_folder, filename ) )

    dbtools.close()

class BemacsSqliteTools:
    def __init__( self ):
        self.db = None

    def create( self, lib_name ):
        import sqlite3
        self.db = sqlite3.connect( lib_name+'.db' )
        c = self.db.cursor()
        c.execute( '''CREATE TABLE key_value (key TEXT PRIMARY KEY, value TEXT)''' )

    def add( self, lib_name, filename ):
        basename = os.path.basename( filename )
        c = self.db.cursor()
        with open( filename, 'rb' ) as f:
            c.execute( '''INSERT INTO key_value VALUES( ?, ? )''', (basename, f.read().decode('utf-8')) )

    def close( self ):
        self.db.commit()
        self.db.close()

class BemacsDatabaseTools:
    def __init__(self, tool_path):
        exe_suffix = ''
        if sys.platform == 'win32':
            exe_suffix = '.exe'
        self.tool_path = tool_path
        self.__dbcreate = os.path.join( tool_path, 'dbcreate' + exe_suffix )
        if not os.path.exists( self.__dbcreate ):
            self.__dbcreate = os.path.join( tool_path, 'bemacs-dbcreate' + exe_suffix )
            if not os.path.exists( self.__dbcreate ):
                raise ValueError( 'Missing tool %s' % self.__dbcreate )

        self.__dbadd = os.path.join( tool_path, 'dbadd' + exe_suffix )
        if not os.path.exists( self.__dbadd ):
            self.__dbadd = os.path.join( tool_path, 'bemacs-dbadd' + exe_suffix )
            if not os.path.exists( self.__dbadd ):
                raise ValueError( 'Missing tool %s' % self.__dbadd )

    def _run_command( self, cmd, filename=None ):
        if filename is None:
            stdin = subprocess.PIPE
        else:
            stdin = open( filename, 'r' )

        proc = subprocess.Popen(
                    cmd,
                    shell=False,
                    stdin=stdin,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                    )

        output = proc.stdout.read()
        rc = proc.wait()
        if output != '':
            sys.stdout.write( output.decode( 'utf-8' ) )

    def create( self, lib_name ):
        self._run_command( [self.__dbcreate, lib_name, '-c'] )

    def add( self, lib_name, filename ):
        basename = os.path.basename( filename )
        self._run_command( [self.__dbadd, lib_name, basename], filename )

    def close( self ):
        pass

library_files =    [
    ('common',    'Library/abbrev.key'),
    ('common',    'Library/abbrev.ml'),
    ('common',    'Library/addlog.ml'),
    ('common',    'Library/addlog.mlp'),
    ('common',    'Library/area.ml'),
    ('common',    'Library/autoarg.ml'),
    ('common',    'Library/blissmode.key'),
    ('common',    'Library/blissmode.ml'),
    ('common',    'Library/bookmark.ml'),
    ('common',    'Library/bufdired.ml'),
    ('common',    'Library/buff.ml'),
    ('common',    'Library/buffer-commands.ml'),
    ('common',    'Library/case-convert.ml'),
    ('common',    'Library/centerlin.ml'),
    ('common',    'Library/cmode.key'),
    ('common',    'Library/cmode.ml'),
    ('common',    'Library/console-ui.ml'),
    ('common',    'Library/dabbrev.ml'),
    ('common',    'Library/debug.key'),
    ('common',    'Library/debug.ml'),
    ('common',    'Library/default-emacs-init.ml'),
    ('common',    'Library/dent.ml'),
    ('common',    'Library/describe-in-buffer.ml'),
    ('common',    'Library/describe_word_list.database'),
    ('common',    'Library/describex.ml'),
    ('common',    'Library/desword.ml'),
    ('common',    'Library/diff.ml'),
    ('common',    'Library/diff.key'),
    ('common',    'Library/dired.ml'),
    ('common',    'Library/dired.mlp'),
    ('common',    'Library/edit-variable.ml'),
    ('common',    'Library/edtsim.ml'),
    ('common',    'Library/electricc.key'),
    ('common',    'Library/electricc.ml'),
    ('common',    'Library/emacs.mlp'),
    ('common',    'Library/emacs_control_strings.ml'),
    ('common',    'Library/emacs_default_autoloads.ml'),
    ('common',    'Library/emacs_default_keybindings.ml'),
    ('common',    'Library/emacs_file_reload.ml'),
    ('common',    'Library/errmsgprs.ml'),
    ('common',    'Library/expand-word-in-buffer.ml'),
    ('common',    'Library/expandx.ml'),
    ('common',    'Library/flasher.ml'),
    ('common',    'Library/form.ml'),
    ('common',    'Library/fundmode.ml'),
    ('common',    'Library/git-commands.ml'),
    ('common',    'Library/globdel.ml'),
    ('common',    'Library/goto.ml'),
    ('common',    'Library/grep-in-buffers.key'),
    ('common',    'Library/grep.key'),
    ('common',    'Library/grep.ml'),
    ('common',    'Library/gtags.key'),
    ('common',    'Library/gtags.ml'),
    ('common',    'Library/helpchar.key'),
    ('common',    'Library/helpchar.ml'),
    ('common',    'Library/helpchar_apropos.ml'),
    ('common',    'Library/helpchar_compose.ml'),
    ('common',    'Library/helpchar_describe.ml'),
    ('common',    'Library/helpchar_describe_bindings.ml'),
    ('common',    'Library/helpchar_describe_key.ml'),
    ('common',    'Library/helpchar_help.ml'),
    ('common',    'Library/helpchar_help_i.ml'),
    ('common',    'Library/helpchar_info.ml'),
    ('common',    'Library/helpchar_news.ml'),
    ('common',    'Library/helpchar_variable.ml'),
    ('common',    'Library/helpchar_view_help.ml'),
    ('common',    'Library/helpchar_where_is.ml'),
    ('common',    'Library/html-mode.key'),
    ('common',    'Library/html-mode.ml'),
    ('common',    'Library/incsearch.key'),
    ('common',    'Library/incsearch.ml'),
    ('common',    'Library/info.ml'),
    ('common',    'Library/info.mlp'),
    ('common',    'Library/info_at.ml'),
    ('common',    'Library/info_ctrl_n.ml'),
    ('common',    'Library/info_ctrl_r.ml'),
    ('common',    'Library/info_d.ml'),
    ('common',    'Library/info_g.ml'),
    ('common',    'Library/info_h.ml'),
    ('common',    'Library/info_hlp.ml'),
    ('common',    'Library/info_l.ml'),
    ('common',    'Library/info_m.ml'),
    ('common',    'Library/info_n.ml'),
    ('common',    'Library/info_o.ml'),
    ('common',    'Library/info_p.ml'),
    ('common',    'Library/info_save.ml'),
    ('common',    'Library/info_u.ml'),
    ('common',    'Library/inno-mode.ml'),
    ('common',    'Library/inno-mode.key'),
    ('common',    'Library/install-shield-mode.ml'),
    ('common',    'Library/install-shield-mode.key'),
    ('common',    'Library/javamode.key'),
    ('common',    'Library/javamode.ml'),
    ('common',    'Library/javascriptmode.key'),
    ('common',    'Library/javascriptmode.ml'),
    ('common',    'Library/json-mode.key'),
    ('common',    'Library/json-mode.ml'),
    ('common',    'Library/justify.ml'),
    ('common',    'Library/justipara.ml'),
    ('common',    'Library/killer.key'),
    ('common',    'Library/killer.ml'),
    ('common',    'Library/language_support.ml'),
    ('common',    'Library/latexmode.key'),
    ('common',    'Library/latexmode.ml'),
    ('common',    'Library/lispmode.key'),
    ('common',    'Library/lispmode.ml'),
    ('common',    'Library/lua-mode.key'),
    ('common',    'Library/lua-mode.ml'),
    #('common',    'Library/lk201.ml'),
    #('common',    'Library/lk201_compose_chart'),
    #('common',    'Library/lk201_minibuf.ml'),
    ('common',    'Library/makebox.ml'),
    ('common',    'Library/makefile-mode.ml'),
    ('common',    'Library/minibuf-recall.key'),
    ('common',    'Library/minibuf-recall.ml'),
    ('common',    'Library/minibuf.ml'),
    ('common',    'Library/misc.ml'),
    ('common',    'Library/mouse-rectangular.ml'),
    ('common',    'Library/mouse.ml'),
    ('common',    'Library/newcompil.ml'),
    ('common',    'Library/normalmod.ml'),
    ('common',    'Library/ntcmdmode.key'),
    ('common',    'Library/ntcmdmode.ml'),
    ('common',    'Library/number.ml'),
    ('common',    'Library/p4-change-mode.key'),
    ('common',    'Library/p4-change-mode.ml'),
    ('common',    'Library/p4-clientspec-mode.key'),
    ('common',    'Library/p4-clientspec-mode.ml'),
    ('common',    'Library/p4-commands.ml'),
    ('common',    'Library/page.ml'),
    ('common',    'Library/paragraph.ml'),
    ('common',    'Library/pascal.ml'),
    ('common',    'Library/puppet-mode.ml'),
    ('common',    'Library/puppet-mode.key'),
    ('common',    'Library/pc-keyboard.ml'),
    ('common',    'Library/pc-mouse.ml'),
    ('common',    'Library/process.key'),
    ('common',    'Library/process.ml'),
    ('common',    'Library/pybemacs-ui.ml'),
    ('common',    'Library/python-console.ml'),
    ('common',    'Library/python-mode.key'),
    ('common',    'Library/python-mode.ml'),
    ('common',    'Library/query-replace.ml'),
    ('common',    'Library/rectangular.key'),
    ('common',    'Library/rectangular.ml'),
    ('common',    'Library/rust-mode.ml'),
    ('common',    'Library/rust-mode.key'),
    ('common',    'Library/rpm-specfile-mode.ml'),
    ('common',    'Library/scribe.ml'),
    ('common',    'Library/sentence.ml'),
    ('common',    'Library/sh-mode.key'),
    ('common',    'Library/sh-mode.ml'),
    ('common',    'Library/shell.mlp'),
    ('common',    'Library/show-buffer.ml'),
    ('common',    'Library/smart-auto-execute.ml'),
    ('common',    'Library/spell.key'),
    ('common',    'Library/spell.ml'),
    ('common',    'Library/spawn.ml'),
    ('common',    'Library/sql-mode.key'),
    ('common',    'Library/sql-mode.ml'),
    ('common',    'Library/sys_literals.ml'),
    ('common',    'Library/tabs.ml'),
    ('common',    'Library/tags.key'),
    ('common',    'Library/tags.ml'),
    ('common',    'Library/teach.mlp'),
    ('common',    'Library/template_driver.ml'),
    ('common',    'Library/textmode.key'),
    ('common',    'Library/textmode.ml'),
    ('common',    'Library/time.ml'),
    ('common',    'Library/trace.ml'),
    ('common',    'Library/undo.ml'),
    ('common',    'Library/view.ml'),
    ('common',    'Library/vbscript-mode.ml'),
    ('common',    'Library/vbscript-mode.key'),
    ('common',    'Library/whatcurs.ml'),
    ('common',    'Library/whereis.ml'),
    ('common',    'Library/white-space-mini-mode.ml'),
    ('common',    'Library/windiff.ml'),
    ('common',    'Library/windows.ml'),
    ('common',    'Library/writereg.ml'),
    ('common',    'Library/xhtml-mode.key'),
    ('common',    'Library/xhtml-mode.ml'),
    ('common',    'Library/xml-mode.key'),
    ('common',    'Library/xml-mode.ml'),

    #
    #    motif files
    #
    ('motif',    'Library/Motif/emacs_motif.key'),
    ('motif',    'Library/Motif/emacs_motif.ml'),
    ('motif',    'Library/Motif/motif-def.ml'),

    #
    #    windows files
    #
    #('windows',    'Library/Windows/emacs_windows.key'),
    #('windows',    'Library/Windows/emacs_windows.ml'),
    ('windows',    'Library/Windows/windows-process.ml'),
    #('windows',    'Library/Windows/shell-chdir-here.mlp'),
    #('windows',    'Library/windows/vss-view.mlp'),

    #
    #    vms files
    #
    ('vms',        'Library/VMS/dvi_def.ml'),
    ('vms',        'Library/VMS/dvidef.ml'),
    ('vms',        'Library/VMS/emacs_spell.ml'),
    ('vms',        'Library/VMS/emacs_sys.ml'),
    ('vms',        'Library/VMS/cli.ml'),
    ('vms',        'Library/VMS/cms.ml'),
    ('vms',        'Library/VMS/dclmode.ml'),
    ('vms',        'Library/VMS/debug32.mlp'),
    ('vms',        'Library/VMS/fscn_def.ml'),
    ('vms',        'Library/VMS/fscndef.ml'),
    ('vms',        'Library/VMS/help.ml'),
    ('vms',        'Library/VMS/jpi_def.ml'),
    ('vms',        'Library/VMS/jpidef.ml'),

    ('vms',        'Library/VMS/mail.commands'),
    ('vms',        'Library/VMS/mail.key'),
    ('vms',        'Library/VMS/mail.ml'),
    ('vms',        'Library/VMS/mail_commands.ml'),
    ('vms',        'Library/VMS/mail_def.ml'),
    ('vms',        'Library/VMS/mail_routines.ml'),

    ('vms',        'Library/VMS/messages.key'),
    ('vms',        'Library/VMS/messages.ml'),

    ('vms',        'Library/VMS/notes.key'),
    ('vms',        'Library/VMS/notes.ml'),
    ('vms',        'Library/VMS/notes.mlp'),
    ('vms',        'Library/VMS/notes_commands.ml'),
    ('vms',        'Library/VMS/notes_def.ml'),
    ('vms',        'Library/VMS/notes_routines.ml'),

    ('vms',        'Library/VMS/queues.ml'),
    ('vms',        'Library/VMS/qui_def.ml'),
    ('vms',        'Library/VMS/quidef.ml'),
    ('vms',        'Library/VMS/show-system.ml'),
    ('vms',        'Library/VMS/sjc_def.ml'),
    ('vms',        'Library/VMS/sjcdef.ml'),
    ('vms',        'Library/VMS/spell.key'),
    ('vms',        'Library/VMS/spell.ml'),
    ('vms',        'Library/VMS/syi_def.ml'),
    ('vms',        'Library/VMS/syidef.ml'),
    ('vms',        'Library/VMS/teco.ml'),
    ('vms',        'Library/VMS/thesaurus.ml'),
    ('vms',        'Library/VMS/uai_def.ml'),
    ('vms',        'Library/VMS/uaidef.ml'),
    ('vms',        'Library/VMS/vms_mail.key'),
    ('vms',        'Library/VMS/vms_mail.mlp'),
    ('vms',        'Library/VMS/vms-process.ml'),


    ('unix',    'Library/Unix/unix-process.ml'),
    ('unix',    'Library/Unix/unix-process.key'),
    ('unix',    'Library/Unix/unix-buffer.ml'),
    ('unix',    'Library/Unix/unix-server.ml'),
    ('unix',    'Library/Unix/unix-man.ml')
    ]

if __name__ == "__main__":
    sys.exit( main( sys.argv ) )
