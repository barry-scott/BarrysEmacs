#
#   be_user
#
bemacs = None

def be_init_user( bemacs_ ):
    global bemacs
    bemacs = bemacs_

    import sys
    import be_platform_specific
    sys.path.insert( 0, str(be_platform_specific.getUserDir()) )
    bemacs.log.info( repr(sys.path) )

def be_init_for_unittest():
    global bemacs
    class FakeLog:
        def __init__( self ):
            pass
        def info( self, msg ):
            print( 'bemacs.log.info: %s' % (msg,) )

    class FakeBemacs:
        def __init__( self ):
            self.log = FakeLog()

    bemacs = FakeBemacs()

def be_import( library ):
    module = __import__( library )
    globals()[ module.__name__ ] = module
    return None

def be_call( function_name, *args ):
    import sys
    function_path = function_name.split('.')

    function = sys.modules['be_user']

    for name in function_path:
        function = getattr( function, name )

    return function( *args )

def be_exec( arg ):
    return exec( arg )

def be_eval( arg ):
    return eval( arg )
