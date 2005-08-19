//
//	key_names.h
//
//	Copyright (c) 1997 Barry A. Scott
//
class key_name_entry
	{
public:
	key_name_entry( const char *n, const char *v );
	key_name_entry( const key_name_entry &from );
	EmacsString name;
	EmacsString value;
	EmacsString compressed_value;
	};

class key_name
	{
public:
	key_name( key_name_entry *_key_names, int sizeof_key_names )
		: key_names( _key_names )
		, num_key_names( sizeof_key_names/sizeof( key_name_entry ) )
		, key_compressed_built( -1 )
		{ }
	~key_name()
		{ }

	// return the value of a named key
	EmacsString valueOfKeyName( const EmacsString &keyname );
	// return the name of the key that best matches the chars in keyname
	// and return the number of chars matched
	int keyNameOfValue( const EmacsString &chars, EmacsString &keyname);

private:
	void build_key_values();

	key_name_entry *key_names;
	const int num_key_names;
	int key_compressed_built;
	};

extern key_name LK201_key_names;
extern key_name PC_key_names;
