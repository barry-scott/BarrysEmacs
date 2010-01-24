//
//    key_names.h
//
//    Copyright (c) 1997-2010 Barry A. Scott
//
typedef std::map< EmacsString, EmacsString >    KeysMapping_t;

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
    key_name( key_name_entry *_key_names, int sizeof_key_names );
    key_name();
    virtual ~key_name();

    void addMapping( const EmacsString &name, const EmacsString &keys );

    // return the value of a named key
    EmacsString valueOfKeyName( const EmacsString &keyname );

    // return the name of the key that best matches the chars in keyname
    // and return the number of chars matched
    int keyNameOfValue( const EmacsString &chars, EmacsString &keyname);

private:
    void buildCompressedMapping();

    KeysMapping_t name_to_keys;
    KeysMapping_t keys_to_name;

    KeysMapping_t name_to_compressed_keys;
    KeysMapping_t compressed_keys_to_name;
};

#if defined( EMACS_LK201_KEYBOARD_SUPPORT )
extern key_name LK201_key_names;
#endif
extern key_name PC_key_names;
