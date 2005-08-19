//
//
//	emacs string table
//		Copyright 1995 Barry A. Scott
//
//
class EmacsStringTable : public EmacsObject
	{
	// the protected need overriding for type safty
public:
	EMACS_OBJECT_FUNCTIONS( EmacsStringTable )
	EmacsStringTable( int initial_size=256, int growth=256 );
	virtual ~EmacsStringTable();

	// add a key and value to the table
	void add( const EmacsString &key, void *value );
	// remove a value from the table
	void *remove( const EmacsString &key );
	// find the value associated with the key
	void *find( const EmacsString &key );

	// these public routines are independent of the value type
public:
	// delete all the keys, the value is left dangling
	void emptyTable();

	// return the number of entries in the table that match prefix
	// retrun in matchingString the longest common prefix of the matching
	// entries. This can be longer or shorter then prefix.
	int match( const EmacsString &prefix, EmacsString &matchingString );

	// return the next entry that contains str in the key
	// start with index as zero
	// no more matches once a NULL has been returned
	const EmacsString *apropos( const EmacsString &str, int &index );

	// return the key at slot index
	const EmacsString *key( int index );

	// return the value at slot index
	void *value( int index );

	// return the number of entries in the table
	int entries(void) { return num_entries; }

	// return one of the keys in the table otherwise NULL
	EmacsString &get_word_interactive( const EmacsString &prompt, EmacsString &result )
		{ return get_word_interactive( prompt, EmacsString::null, result ); }
	EmacsString &get_word_interactive( const EmacsString &prompt, const EmacsString &default_value, EmacsString &result );
	EmacsString &get_word_mlisp( EmacsString &result );

	// return one of the keys, a new value otherwise NULL
	EmacsString &get_esc_word_interactive( const EmacsString &prompt, const EmacsString &default_value, EmacsString &result );
	EmacsString &get_esc_word_interactive( const EmacsString &prompt, EmacsString &result )
		{ return get_esc_word_interactive( prompt, EmacsString::null, result ); }
	EmacsString &get_esc_word_mlisp( EmacsString &result );

	// these routines assist getword and getnsword to do theere job
protected:
	// override to do things like file string tables
	virtual void makeTable( EmacsString &prefix );

	// override to do things like case blind compare
	virtual int compareKeys( const EmacsString &string1, const EmacsString &string2 );
	virtual int commonPrefix( const EmacsString &string1, const EmacsString &string2 );

	// return true is the entry allows the get to finish
	// for example if entry is a directory don't finish
	// but if its a file dso finish
	virtual bool terminalEntry( const EmacsString &entry );

	// fill the help buffer with using information about
	// the permitted values
	void fillHelpBuffer( const EmacsString &prefix, int nfound );

	// hide the details of the implementation
private:
	int findIndex( const EmacsString &key );
	
	int num_entries;
	int allocated_entries;
	int grow_by;
	EmacsString **keys;
	void **values;
	};


#define getword( obj, prompt ) (cur_exec == NULL ? obj get_word_interactive( prompt ) : obj get_word_mlisp())
#define getescword( obj, prompt, result ) (cur_exec == NULL ? obj get_esc_word_interactive( prompt, result ) : obj get_esc_word_mlisp( result))
