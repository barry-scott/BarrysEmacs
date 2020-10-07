//
//    Copyright (c) 1995-2014 Barry A. Scott
//
//    emacs exceptions header
//

extern void debug_exception(void);

class EmacsException
{
public:
    EmacsException() : _code(0)
    {
        debug_exception();
    }
    virtual ~EmacsException() { }
protected:
    int _code;
};

class EmacsInternalError
{
public:
    EmacsInternalError( const char *reason ) : error_reason( reason ) { }
    virtual ~EmacsInternalError() { }
    const char *error_reason;
};

class EmacsExceptionTextError : public EmacsException
{
public:
    EmacsExceptionTextError( const char *text )
        : error_text( text )
    { }

    const char *error_text;
};

//
//    Expression handling expressions
//
class EmacsExceptionExpression : public EmacsException
{
public:
    EmacsExceptionExpression() : EmacsException() { }
};

class EmacsExceptionExpressionNotInteger : public EmacsExceptionExpression
{
public:
    EmacsExceptionExpressionNotInteger() : EmacsExceptionExpression() { }
};

class EmacsExceptionExpressionNotString : public EmacsExceptionExpression
{
public:
    EmacsExceptionExpressionNotString() { }
};

class EmacsExceptionExpressionNotMarker : public EmacsExceptionExpression
{
public:
    EmacsExceptionExpressionNotMarker() { };
};

class EmacsExceptionExpressionNotArray : public EmacsExceptionExpression
{
public:
    EmacsExceptionExpressionNotArray() { };
};

class EmacsExceptionExpressionNotWindows : public EmacsExceptionExpression
{
public:
    EmacsExceptionExpressionNotWindows() { };
};

//
//    Variable handling exceptions
//
class EmacsExceptionVariable : public EmacsException
{
public:
    EmacsExceptionVariable() { }
};

class EmacsExceptionVariableReadOnly : public EmacsExceptionVariable
{
public:
    EmacsExceptionVariableReadOnly() { }
};

class EmacsExceptionVariablePositive : public EmacsExceptionVariable
{
public:
    EmacsExceptionVariablePositive() { }
};

class EmacsExceptionVariableBoolean : public EmacsExceptionVariable
{
public:
    EmacsExceptionVariableBoolean() { }
};

class EmacsExceptionVariableStringTooLong : public EmacsExceptionVariable
{
public:
    EmacsExceptionVariableStringTooLong() { }
};

class EmacsExceptionVariableTabOutOfRange : public EmacsExceptionVariable
{
public:
    EmacsExceptionVariableTabOutOfRange() { }
};

class EmacsExceptionVariableGreaterThanRange : public EmacsExceptionVariable
{
public:
    EmacsExceptionVariableGreaterThanRange( int _range ) : range( _range ) { }
    int range;
};

class EmacsExceptionVariableLessThanRange : public EmacsExceptionVariable
{
public:
    EmacsExceptionVariableLessThanRange( int _range ) : range( _range ) { }
    int range;
};

class EmacsExceptionUserInputAbort : public EmacsException
{
public:
    EmacsExceptionUserInputAbort() { }
};
