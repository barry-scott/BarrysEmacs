//
//    Emacs_python_interface.h
//
//    Copyright (c) 1999-2002 Barry A. Scott
//
class EmacsPythonCommand
{
public:
    EmacsPythonCommand();
    virtual ~EmacsPythonCommand();

    bool executeCommand();

    bool failed() const;
    const EmacsString &failureReason() const;
    const Expression &getResult() const;

    void commandSucceeded( const Expression &result );
    void commandFailed( const EmacsString &reason );

protected:
    virtual void executeCommandImpl() = 0;

    void runPythonStringInsideTryExcept( const EmacsString &command );

private:
    bool command_failed;
    EmacsString failure_reason;
    Expression result;
};
