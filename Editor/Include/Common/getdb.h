//
//
//	getdb.h
//
//

class DatabaseEntryNameTable : public EmacsStringTable
	{
public:
	EMACS_OBJECT_FUNCTIONS( DatabaseEntryNameTable )
	DatabaseEntryNameTable( DatabaseSearchList *dbs );
	virtual ~DatabaseEntryNameTable();
private:
	virtual void makeTable( EmacsString &prefix );

	static int indexDatabaseEntryCallback( const EmacsString &key, unsigned char * * );

	DatabaseSearchList *dbs;
	static DatabaseEntryNameTable *activeTable;
	};
