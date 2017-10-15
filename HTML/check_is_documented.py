#!/usr/bin/python3
#
#   check_is_documented.py
#
#   check that the variables and functions builtin to bemacs
#   all have documentation.
#
import sys
import os
import pathlib
import html

class CheckIsDocumented:
    def __init__( self, argv ):
        self.argv = argv
        self.builder_top_dir = pathlib.Path( os.environ["BUILDER_TOP_DIR"] )

    def main( self ):
        all_variables = self.loadDefinedVariables( self.builder_top_dir / 'Editor/Source/Common/variable.cpp' )
        self.checkVariableAreDocumented( all_variables )
        self.checkVariableAreIndexed( all_variables )

        all_functions = self.loadDefinedFunctions( self.builder_top_dir / 'Editor/Source/Common/function.cpp' )
        self.checkFunctionsAreDocumented( all_functions )
        self.checkFunctionsAreIndexed( all_functions )

        return 0

    def loadDefinedVariables( self, src ):
        all_variables = []
        with open( str(src), 'r' ) as f:
            for line in f:
                if '("' in line:
                    all_variables.append( line.split('"')[1] )
        return all_variables

    def loadDefinedFunctions( self, src ):
        all_functions = []
        with open( str(src), 'r' ) as f:
            for line in f:
                if '( "' in line:
                    all_functions.append( line.split('"')[1] )
        return all_functions

    def checkVariableAreDocumented( self, all_variables ):
        html_folder = self.builder_top_dir / 'HTML'

        all_variables.sort()
        for varname in all_variables:
            definition_line = '<p><a name="%(varname)s"></a><b>%(varname)s</b>' % {'varname': varname}

            with open( html_folder / ('var_%s.html' % (varname[0].lower(),)), 'r' ) as f:
                found = False
                for line in f:
                    if line.startswith( definition_line ):
                        found = True
                        break

                if not found:
                    print( 'No documentation for variable "%s"' % (varname,) )

    anchor_name_map = {
        'c=': 'cequals',
        '+': 'plus',
        '-': 'minus',
        '*': 'multiply',
        '/': 'divide',
        '%': 'modulus',
        '^': 'or',
        '&': 'and',
        '|': 'logicalor',
        '!': 'complement',
        '=': 'equal',
        '!=': 'notequal',
        '<': 'lessthan',
        '<=': 'lessorequal',
        '>': 'greater',
        '>=': 'greaterorequal',
        '>>': 'rightshift',
        '<<': 'shiftleft',
        }

    def checkFunctionsAreDocumented( self, all_functions ):
        html_folder = self.builder_top_dir / 'HTML'

        all_functions.sort()
        for funcname in all_functions:
            definition_line = ('<p><a name="%(aname)s">(<b>%(funcname)s</b>' %
                                {'funcname': html.escape( funcname, quote=True )
                                ,'aname': self.anchor_name_map.get( funcname, funcname )})
            if 'a' <= funcname[0].lower() <= 'z':
                suffix = funcname[0].lower()
            else:
                suffix = 'shriek'

            with open( html_folder / ('fn_%s.html' % (suffix,)), 'r' ) as f:
                found = False
                for line in f:
                    if line.startswith( definition_line ):
                        found = True
                        break

                if not found:
                    print( 'No documentation for function "%s"' % (funcname,) )

    def checkVariableAreIndexed( self, all_variables ):
        index = self.builder_top_dir / 'HTML/var_list.html'
        all_indexed = set()

        with index.open() as f:
            for line in f:
                if '<a href="var_' in line:
                    parts = line.strip().split( '>' )
                    varname = parts[-2].split('<')[0].strip()
                    varname = html.unescape( varname )
                    all_indexed.add( varname )

        missing = set(all_variables) - all_indexed
        for varname in sorted(missing):
            print( 'No index for variable "%s"' % (varname,) )

    def checkFunctionsAreIndexed( self, all_functions ):
        index = self.builder_top_dir / 'HTML/fn_list.html'
        all_indexed = set()

        with index.open() as f:
            for line in f:
                if '<a href="fn_' in line:
                    parts = line.strip().split( 'target="body">' )
                    funcname = parts[-1].split()[0].split('<')[0]
                    funcname = html.unescape( funcname )
                    all_indexed.add( funcname )

        missing = set(all_functions) - all_indexed
        for funcname in sorted(missing):
            print( 'No index for function "%s"' % (funcname,) )

if __name__ == '__main__':
    sys.exit( CheckIsDocumented( sys.argv ).main() )
