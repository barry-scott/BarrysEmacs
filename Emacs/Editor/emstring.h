//
//	Copyright (c) 1995-2000 Barry A. Scott
//

//
//
//	EmacsString
//
//
#include <string>

class EmacsString;
class EmacsStringRepresentation;

#ifdef _MSC_VER
class CString;
#endif

class EmacsString : public EmacsObject
	{
	friend class EmacsInitialisation;
public:
	EMACS_OBJECT_FUNCTIONS( EmacsString )
	enum string_type
		{
		copy,		// make a copy of the string and free on delete
		free,		// free the malloc'ed string
		keep		// do not free the string
		};

	static const EmacsString null;	// always the empty string

	EmacsString( void );
	EmacsString( const char *string );
	EmacsString( const unsigned char *string );
	EmacsString( char *string );
	EmacsString( unsigned char *string );
	EmacsString( const wchar_t *string );
	EmacsString( const wchar_t *string, int length );
	EmacsString( enum string_type type );
	EmacsString( enum string_type type, char *string );
	EmacsString( enum string_type type, char *string, int length );
	EmacsString( enum string_type type, const char *string );
	EmacsString( enum string_type type, const char *string, int length );
	EmacsString( enum string_type type, unsigned char *string );
	EmacsString( enum string_type type, unsigned char *string, int length );
	EmacsString( enum string_type type, const unsigned char *string );
	EmacsString( enum string_type type, const unsigned char *string, int length );
	EmacsString( const EmacsString &string );
#ifdef _MSC_VER
	EmacsString( const CString &string );
#endif
	EmacsString( const std::string &string );
	EmacsString( std::string &string );

	EmacsString( EmacsStringRepresentation *rep );

	virtual ~EmacsString();

	EmacsString &operator=( const EmacsString &string );
	EmacsString &operator=( const char *string );
	EmacsString &operator=( const unsigned char *string );
#ifdef _MSC_VER
	EmacsString &operator=( const CString &string );
#endif
	//
	//	relational operators
	//
	bool operator==( const EmacsString &str2 ) const { return compare( str2 ) == 0; }
	bool operator==( const char *str2 ) const { return compare( str2 ) == 0; }
	bool operator==( const unsigned char *str2 ) const { return compare( str2 ) == 0; }
	bool operator!=( const EmacsString &str2 ) const { return compare( str2 ) != 0; }
	bool operator!=( const char *str2 ) const { return compare( str2 ) != 0; }
	bool operator!=( const unsigned char *str2 ) const { return compare( str2 ) != 0; }
	bool operator>( const EmacsString &str2 ) const { return compare( str2 ) > 0; }
	bool operator>( const char *str2 ) const { return compare( str2 ) > 0; }
	bool operator>( const unsigned char *str2 ) const { return compare( str2 ) > 0; }
	bool operator>=( const EmacsString &str2 ) const { return compare( str2 ) >= 0; }
	bool operator>=( const char *str2 ) const { return compare( str2 ) >= 0; }
	bool operator>=( const unsigned char *str2 ) const { return compare( str2 ) >= 0; }
	bool operator<( const EmacsString &str2 ) const { return compare( str2 ) < 0; }
	bool operator<( const char *str2 ) const { return compare( str2 ) < 0; }
	bool operator<( const unsigned char *str2 ) const { return compare( str2 ) < 0; }
	bool operator<=( const EmacsString &str2 ) const { return compare( str2 ) <= 0; }
	bool operator<=( const char *str2 ) const { return compare( str2 ) <= 0; }
	bool operator<=( const unsigned char *str2 ) const { return compare( str2 ) <= 0; }


	//
	//	casts
	//
	operator const unsigned char *() const;
	operator const char *() const;
#ifdef _MSC_VER
	operator const CString() const;
#endif

	//
	//	description of the data
	//
	const unsigned char *data() const;		// unsigned char data
	const char *sdata() const;			// signed char data
	// these two function give unsafe access to the inside of representation
	unsigned char *dataHack() const;		// unsigned char data
	char *sdataHack() const;			// signed char data
	int length() const;
	int isNull() const { return length() == 0; }

	//
	//	searching primitives
	//
	int first( char ch ) const;			// index of first ch in string
	int first( unsigned char ch ) const;		// index of first ch in string
	int last( char ch ) const;			// index of last ch in string
	int last( unsigned char ch ) const;		// index of last ch in string

	int index( char ch, int start_pos=0 ) const;			// find the first ch starting at pos
	int index( unsigned char ch, int start_pos=0 ) const;		// find the first ch starting at pos
	int index( const EmacsString &str, int start_pos=0 ) const;	// find the first str starting at pos

	int commonPrefix( const EmacsString &str ) const;		// length of common prefix case sensitive
	int caseBlindCommonPrefix( const EmacsString &str ) const;	// length of common prefix case blind

	//
	//	string modifiers
	//
	void remove( int position, int length );	// remove the chars from position for length chars
	void remove( int position );			// remove from position to the end of the string

	enum { string_growth_room=32 };			// amount of space to allow for growth 
	EmacsString &append( char ch ) { return append( 1, (const unsigned char *)&ch ); }
	EmacsString &append( const char *str ) { return append( strlen( str ), (const unsigned char *)str ); }
	EmacsString &append( unsigned char ch ) { return append( 1, &ch ); }
	EmacsString &append( const unsigned char *str ) { return append( strlen( (const char *)str ), str ); }
	EmacsString &append( const EmacsString &str ) { return append( str.length(), str.data() ); }
	EmacsString &append( int ch ) { return append( (char)ch ); }

	EmacsString &append( int length, const unsigned char *data );

	EmacsString &insert( int pos, char ch ) { return insert( pos, 1, (const unsigned char *)&ch ); }
	EmacsString &insert( int pos, const char *str ) { return insert( pos, strlen( str ), (const unsigned char *)str ); }
	EmacsString &insert( int pos, unsigned char ch ) { return insert( pos, 1, &ch ); }
	EmacsString &insert( int pos, const unsigned char *str ) { return insert( pos, strlen( (const char *)str ), str ); }
	EmacsString &insert( int pos, const EmacsString &str ) { return insert( pos, str.length(), str.data() ); }
	EmacsString &insert( int pos, int ch ) { return insert( pos, (char)ch ); }

	EmacsString &insert( int pos, int length, const unsigned char *data );

	EmacsString operator ()( int first, int last ) const;	// return a substring

	EmacsString &toLower();
	EmacsString &toUpper();

	inline unsigned char operator[]( int index ) const;	// read only
	inline unsigned char &operator[]( int index );		// read write

	int compare( const EmacsString &str2 ) const;
	int compare( const char *str2 ) const;
	int compare( const unsigned char *str2 ) const;
	int caseBlindCompare( const EmacsString &str2 ) const;
	int caseBlindCompare( const char *str2 ) const;
	int caseBlindCompare( const unsigned char *str2 ) const;
private:
	static void init();

	void copy_on_write();

	EmacsStringRepresentation *_rep;

	static EmacsStringRepresentation *empty_string;

	static EmacsStringRepresentation *bad_value;
	inline void check_for_bad_value( EmacsStringRepresentation *rep );
	};

class EmacsStringRepresentation : public EmacsObject
	{
public:
	EMACS_OBJECT_FUNCTIONS( EmacsStringRepresentation )
	EmacsStringRepresentation
		( 
		enum EmacsString::string_type _type,
		int _alloc_length,
		int _length,
		unsigned char *_data
		);
	EmacsStringRepresentation
		( 
		enum EmacsString::string_type _type,
		int _alloc_length,
		int _length,
		const unsigned char *_data
		);
	EmacsStringRepresentation
		( 
		enum EmacsString::string_type _type,
		int _alloc_length,
		int _length,
		const wchar_t *_data
		);
	virtual ~EmacsStringRepresentation();

	friend class EmacsString;
private:
	int ref_count;
	enum EmacsString::string_type type;
	int alloc_length;
	int length;
	unsigned char *data;
	};

inline void EmacsString::check_for_bad_value( EmacsStringRepresentation *rep )
	{
	if( rep->ref_count < 1 )
		debug_invoke();
	if( rep == bad_value )
		debug_invoke();
	}

inline unsigned char EmacsString::operator[]( int index ) const
	{
	// allow negative index to be relative to the end of the string
	if( index < 0 )
		index += _rep->length;
	emacs_assert( index >= 0 );
	emacs_assert( index < _rep->length );
	return _rep->data[index];
	}

inline unsigned char &EmacsString::operator[]( int index )
	{
	// allow negative index to be relative to the end of the string
	if( index < 0 )
		index += _rep->length;
	emacs_assert( index >= 0 );
	emacs_assert( index < _rep->length );
	return _rep->data[index];
	}

class FormatString : public EmacsObject
	{
public:
	EMACS_OBJECT_FUNCTIONS( FormatString )
	FormatString( EmacsString _format );
	~FormatString() {}

	operator const EmacsString() const;

	FormatString &operator <<( int v );
	FormatString &operator <<( const EmacsString & );
	FormatString &operator <<( const EmacsString * );
	FormatString &operator <<( const char * );
	FormatString &operator <<( const unsigned char * );

private:
	enum arg_type { argNone, argString, argInt };

	void process_format();
	int next_format_char();

	void put( int ch );
	void put( const unsigned char *chars, unsigned int len );

	void print_decimal( long int );
	void print_hexadecimal( long int );
	void print_octal( long int );
	void print_string( const EmacsString &str );

	EmacsString format;
	EmacsString result;

	arg_type next_arg_type;
	arg_type next_width_type;
	arg_type next_precision_type;
	char format_char;
	char pad_char;

	int width;
	int precision;
	int left_justify;

	int next_format_char_index;

	int intArg;
	EmacsString stringArg;
	// keep some compilers for complaining (C Set++)
	FormatString( const FormatString &other );
	};

class EmacsStringIterator
	{
public:
	EmacsStringIterator( const EmacsString &string, int index=0 );
	virtual ~EmacsStringIterator();

	bool next( char &ch );
private:
	const EmacsString string;
	int index;
	int remaining;
	};
