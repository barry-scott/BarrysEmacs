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
class EmacsFile;

class FileFindImplementation
{
protected:
    FileFindImplementation( EmacsFile &files, bool return_all_directories );

    bool m_return_all_directories;

    EmacsFile &m_files;
    enum { first_time, next_time, all_done } m_state;
    EmacsString m_root_path;
    EmacsString m_match_pattern;
    EmacsString m_full_filename;

public:
    virtual ~FileFindImplementation() { }
    virtual EmacsString next() = 0;
    virtual EmacsString repr() = 0;
    virtual const EmacsString &matchPattern() const;
};

class EmacsFile;

class FileFind
{
public:
    FileFind( EmacsFile &files, bool return_all_directories=false );
    virtual ~FileFind();

    virtual EmacsString next();
    virtual EmacsString repr();
    virtual const EmacsString &matchPattern() const;

private:
    FileFindImplementation *m_impl;
};
