#
#   be_user
#

def be_init_user( bemacs_ ):
    global bemacs
    bemacs = bemacs_

def be_import( library ):
    import importlib
    importlib.import_module( library )
    return None

def be_call( function_name, args ):
    import sys
    function_path = function_name.split('.')

    function = sys.modules['be_user']
    for mod_name in function_name:
        function = module.getattr( module )

    return function( *args )

def be_exec( arg ):
    return exec( arg )

def be_eval( arg ):
    return eval
