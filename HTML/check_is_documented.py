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

        all_functions = self.loadDefinedFunctions( self.builder_top_dir / 'Editor/Source/Common/function.cpp' )
        self.checkFunctionsAreDocumented( all_functions )

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
                    print( 'Variable "%s" not documented' % (varname,) )

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
                    #print( 'qqq 1 %r' % (definition_line,) )
                    #print( 'qqq 2 %r' % (line,) )
                    if line.startswith( definition_line ):
                        found = True
                        break

                if not found:
                    print( 'Function "%s" not documented' % (funcname,) )



if __name__ == '__main__':
    sys.exit( CheckIsDocumented( sys.argv ).main() )
