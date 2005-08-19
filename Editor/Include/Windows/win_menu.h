//
//	win_menu.h
//
//	Copyright (c) 2000 Barry A. Scott
//

class EmacsMenu;


class EmacsMenuNameTable : public EmacsStringTable
	{
public:
	EmacsMenuNameTable( int init_size, int grow_amount )
		: EmacsStringTable( init_size, grow_amount )
		{ }
	virtual ~EmacsMenuNameTable()
		{ }

	void add( const EmacsString &key, EmacsMenu *value )
		{ EmacsStringTable::add( key, value ); }
	EmacsMenu *remove( const EmacsString &key )
		{ return (EmacsMenu *)EmacsStringTable::remove( key ); }
	EmacsMenu *find( const EmacsString &key )
		{ return (EmacsMenu *)EmacsStringTable::find( key ); }
	EmacsMenu *value( int index )
		{ return (EmacsMenu *)EmacsStringTable::value( index ); }
	};

