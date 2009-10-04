//
//    string_map.h
//
//    Copyright (c) 2000 Barry A. Scott
//
//
class StringMapBase
{
protected:
    StringMapBase( const char **_strings, int *_values );
    virtual ~StringMapBase();

    bool map( const EmacsString &key, int &value );
    EmacsString map( int value );
private:
    const char **strings;
    int *values;
};

template <class T>
class StringMap : public StringMapBase
{
public:
    StringMap( const char **_strings,  T *_values )
        : StringMapBase( _strings, (int *)_values )
    { }
    virtual ~StringMap() { }

    bool map( const EmacsString &key, T &value )
    { return StringMapBase::map( key, (int &)value ); }
    EmacsString map( T value )
    { return StringMapBase::map( value ); }
};
