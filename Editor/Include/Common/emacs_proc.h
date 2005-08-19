//
//	emacs_proc.h
//		copyright (c) 1995-1996 Barry A. Scott
//
class EmacsProcess;
class EmacsProcessNameTable : public EmacsStringTable
	{
public:
	EMACS_OBJECT_FUNCTIONS( EmacsProcessNameTable )
	EmacsProcessNameTable( int init_size, int grow_amount )
		: EmacsStringTable( init_size, grow_amount )
		{ }
	virtual ~EmacsProcessNameTable()
		{ }

	void add( const EmacsString &key, EmacsProcess *value )
		{ EmacsStringTable::add( key, value ); }
	EmacsProcess *remove( const EmacsString &key )
		{ return (EmacsProcess *)EmacsStringTable::remove( key ); }
	EmacsProcess *find( const EmacsString &key )
		{ return (EmacsProcess *)EmacsStringTable::find( key ); }
	EmacsProcess *value( int index )
		{ return (EmacsProcess *)EmacsStringTable::value( index ); }
	};

//
// Process structure base class
//
class EmacsProcessCommon : public EmacsObject
	{
public:
	EMACS_OBJECT_FUNCTIONS( EmacsProcessCommon )
protected:
	EmacsProcessCommon( const EmacsString &name );
	virtual ~EmacsProcessCommon();
public:
	EmacsString proc_name;			// Local Name

	static EmacsProcessNameTable name_table;
	};

class EmacsThreadCommon
	{
public:
	EmacsThreadCommon();
	virtual ~EmacsThreadCommon();

	virtual unsigned run() = 0;
	};
