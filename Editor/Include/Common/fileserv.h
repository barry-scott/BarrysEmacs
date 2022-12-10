//
//    fileserv.h    - file services
//

//
//    Classes to compare file names
//
class FileNameCompare
{
public:
    virtual ~FileNameCompare();
    virtual bool isEqual( const EmacsString &string1, const EmacsString &string2 ) = 0;
    virtual int compare( const EmacsString &string1, const EmacsString &string2 ) = 0;
    virtual int commonPrefix( const EmacsString &string1, const EmacsString &string2 ) = 0;
};

class FileNameCompareCaseSensitive : public FileNameCompare
{
public:
    virtual ~FileNameCompareCaseSensitive();
private:
    virtual bool isEqual( const EmacsString &string1, const EmacsString &string2 );
    virtual int compare( const EmacsString &string1, const EmacsString &string2 );
    virtual int commonPrefix( const EmacsString &string1, const EmacsString &string2 );
};

class FileNameCompareCaseBlind : public FileNameCompare
{
public:
    virtual ~FileNameCompareCaseBlind();
private:
    virtual bool isEqual( const EmacsString &string1, const EmacsString &string2 );
    virtual int compare( const EmacsString &string1, const EmacsString &string2 );
    virtual int commonPrefix( const EmacsString &string1, const EmacsString &string2 );
};


extern FileNameCompare *file_name_compare;
extern FileNameCompareCaseSensitive file_name_compare_case_sensitive;
extern FileNameCompareCaseBlind file_name_compare_case_blind;

int match_wild( const EmacsString &, const EmacsString & );

// virtual base class for doing wild card file finding
class FileFindInternal
{
protected:
    FileFindInternal( bool _return_all_directories )
    : return_all_directories( _return_all_directories )
    { }
    bool return_all_directories;
public:
    virtual ~FileFindInternal() { }
    virtual EmacsString next() = 0;
};

class EmacsFile;

class FileFind
{
public:
    FileFind( const EmacsFile &files, bool return_all_directories=false );
    virtual ~FileFind();

    virtual EmacsString next();

private:
    FileFindInternal *impl;
};
