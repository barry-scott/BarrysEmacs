//
//	CommandLine.h
//
//	Copyright (c) 1997 Barry A. Scott
//

class EmacsCommandLine;

class EmacsArgument
	{
	friend class EmacsCommandLine;
private:
	EmacsArgument( bool _is_qualifier, const EmacsString &_value)
		: is_qualifier( _is_qualifier )
		, arg_value( _value )
		{}
public:
	bool isQualifier() const { return is_qualifier; }
	const EmacsString &value() const { return arg_value; }
private:
	bool is_qualifier;
	EmacsString arg_value;
	};

class EmacsCommandLine
	{
public:
	EmacsCommandLine();
	void setArguments( int argc, char **argv );
	void setArguments( const EmacsString &command_line );

	EmacsCommandLine &operator=( EmacsCommandLine &new_command_line );

	void deleteArgument( int n );
	void setArgument( int n, const EmacsString &new_value, bool is_qual=false );

	int argumentCount() const;
	const EmacsArgument &argument( int n ) const;
private:
	void deleteArguments();
	void moveArguments( EmacsCommandLine &new_command_line );

	void addArgument( EmacsString &value );

	enum {MAX_ARGUMENTS = 1024};
	int count;
	EmacsArgument *arguments[MAX_ARGUMENTS];
	bool no_more_qualifers;
	};

class EmacsCommandLineServerWorkItem : public EmacsWorkItem
	{
public:
	EmacsCommandLineServerWorkItem()
		: EmacsWorkItem()
		, command_current_directory()
		, command_line()
		{}
	virtual ~EmacsCommandLineServerWorkItem()
		{}

	void newCommandLine( const EmacsString &_current_directory, const EmacsString &_command_line );

protected:
	virtual void workAction(void);

	EmacsString command_current_directory;
	EmacsString command_line;
	};
