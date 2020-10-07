#!/usr/bin/env python
import sys
import sqlite3

def main( argv ):
    mll_file = argv[1]
    db_file = argv[2]

    createDatabaseFromMll( mll_file, db_file )
    return 0

def createDatabaseFromMll( mll_file, db_file ):
    assert db_file.endswith( '.db' )

    db = sqlite3.connect( db_file )
    c = db.cursor()
    c.execute( '''CREATE TABLE key_value (key TEXT PRIMARY KEY, value TEXT)''' )

    with open( mll_file, 'rb' ) as f:
        key = None
        all_body = None
        for line in f:
            line = line.decode('utf-8').replace( '\r\n', '\n' )
            if line[0] == '[' and line[-2:] == ']\n':
                if all_body is not None:
                    value = ''.join(all_body)
                    c.execute( '''INSERT INTO key_value VALUES( ?, ? )''',
                                (key, value) )
                key = line[1:-2]
                all_body = []
                continue

            all_body.append( line )

        # and output the last entry
        value = ''.join(all_body)
        c.execute( '''INSERT INTO key_value VALUES( ?, ? )''',
                    (key, value) )
        all_body = []

    db.commit()
    db.close()

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
