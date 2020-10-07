//
//
//    getfile.h
//
//
class EmacsFileTable : public EmacsStringTable
{
public:
    EMACS_OBJECT_FUNCTIONS( EmacsFileTable )
    EmacsFileTable();
    virtual ~EmacsFileTable();
private:
    virtual void makeTable( EmacsString &prefix );
    // return true is the entry allows the get to finish
    // for example if entry is a directory don't finish
    // but if its a file do finish
    virtual bool terminalEntry( const EmacsString &entry );

    // override to use file_name_compare case blind compare
    virtual int compareKeys( const EmacsString &string1, const EmacsString &string2 );
    virtual int commonPrefix( const EmacsString &string1, const EmacsString &string2 );
};
