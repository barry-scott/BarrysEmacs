//
//    CommandLine.h
//
//    Copyright (c) 1997 Barry A. Scott
//

class EmacsCommandLine;

class EmacsArgument
{
    friend class EmacsCommandLine;
private:
    EmacsArgument( bool is_qualifier, const EmacsString &value)
    : m_is_qualifier( is_qualifier )
    , m_arg_value( value )
    {}

    EmacsArgument( const EmacsArgument &other )
    : m_is_qualifier( other.m_is_qualifier )
    , m_arg_value( other.m_arg_value )
    {}

    EmacsArgument &operator=( const EmacsArgument &other )
    {
        m_is_qualifier = other.m_is_qualifier;
        m_arg_value = other.m_arg_value;

        return *this;
    }

public:
    bool isQualifier() const { return m_is_qualifier; }
    const EmacsString &value() const { return m_arg_value; }

private:
    bool m_is_qualifier;
    EmacsString m_arg_value;
};

class EmacsCommandLine
{
public:
    EmacsCommandLine();
    EmacsCommandLine( const EmacsCommandLine &other );
    void setArguments( int argc, char **argv );
    void setArguments( const EmacsString &command_line );

    EmacsCommandLine &operator=( const EmacsCommandLine &new_command_line );

    void deleteArgument( int n );
    void setArgument( int n, const EmacsString &new_value, bool is_qual=false );

    void addArgument( const EmacsString &value );

    int argumentCount() const;
    const EmacsArgument &argument( int n ) const;

private:
    void deleteArguments();
    void moveArguments( const EmacsCommandLine &new_command_line );


    enum {MAX_ARGUMENTS = 1024};
    int             m_count;
    EmacsArgument  *m_arguments[MAX_ARGUMENTS];
    bool            m_no_more_qualifers;
};

class EmacsCommandLineServerWorkItem : public EmacsWorkItem
{
public:
    EmacsCommandLineServerWorkItem()
    : EmacsWorkItem()
    , m_command_line()
    {}
    virtual ~EmacsCommandLineServerWorkItem()
    {}

    void newCommandLine( const EmacsString &current_directory, const EmacsCommandLine &new_command_line );

protected:
    virtual void workAction(void);

    EmacsString         m_command_current_directory;
    EmacsCommandLine    m_command_line;
};
