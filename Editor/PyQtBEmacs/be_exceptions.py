'''
 ====================================================================
 Copyright (c) 2003-2015 Barry A Scott.  All rights reserved.

 This software is licensed as described in the file LICENSE.txt,
 which you should have received as part of this distribution.

 ====================================================================

    be_exceptions.py

    Based on pysvn wb_exceptions.py

'''
class BemacsError(Exception):
    def __init__( self, msg ):
        Exception.__init__( self, msg )

class InternalError(BemacsError):
    def __init__( self, msg ):
        BemacsError.__init__( self, msg )

#
#    Helper class to cut down code bloat.
#
#    in __init__ add:
#        self.try_wrapper = wb_exceptions.TryWrapperFactory( log )
#
#    where binding a Qt slot  as:
#
#        widget.connect( self.try_wrapper( self.OnSize ) )
#
class TryWrapperFactory:
    def __init__( self, log ):
        self.log = log

    def __call__( self, function ):
        return TryWrapper( self.log, function )

class TryWrapper:
    def __init__( self, log, function ):
        self.log = log
        self.function = function

    def __call__( self, *params, **keywords ):
        try:
            result = self.function( *params, **keywords )
            return result

        except Exception:
            self.log.exception( 'TryWrapper<%s.%s>\n' %
                (self.function.__module__, self.function.__name__ ) )

            return None
