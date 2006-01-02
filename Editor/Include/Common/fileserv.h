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


//
//    On DOS a file is made up of 4 parts,
//    diskname, path, filename and filetype
//    Viz:-
//        DISK:\PATH\FILE.TYPE
//
class FileParse : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( FileParse )
    FileParse();
    virtual ~FileParse();

    bool sys_parse( const EmacsString &, const EmacsString & );
    bool sys_search( const EmacsString &, const EmacsString & );

    EmacsString disk;        // disk:
    EmacsString path;        // /path/
    EmacsString filename;        // name
    EmacsString filetype;        // .type
    EmacsString result_spec;    // full file spec with all fields filled in
    int    wild;            // true if any field is wild
    int    filename_maxlen;    // how long filename can be
    int    filetype_maxlen;    // how long filetype can be
    int    file_case_sensitive;    // true if case is important
protected:
    void init();
    int analyse_filespec( const EmacsString &filespec );
};

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

class FileFind
{
public:
    FileFind( const EmacsString &files, bool return_all_directories=false );
    virtual ~FileFind();

    virtual EmacsString next();
private:
    FileFindInternal *implementation;
};

extern int file_is_directory( const EmacsString &file );
extern int file_is_regular( const EmacsString &file );
