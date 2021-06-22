//    Copyright (c) 1982-1995
//        Barry A. Scott
// Header file for dealing with values returned by mlisp functions


//
//    Classes defined in this header file
//

//    Class                           derived from
class Expression;
class  SystemExpression;                // public Expression
class ExpressionRepresentation;
class  ExpressionRepresentationInt;            // public ExpressionRepresentation
class  SystemExpressionRepresentationInt;        // public ExpressionRepresentation
class   SystemExpressionRepresentationIntReadOnly;    // public SystemExpressionRepresentationInt
class   SystemExpressionRepresentationIntPositive;    // public SystemExpressionRepresentationInt
class   SystemExpressionRepresentationIntBoolean;    // public SystemExpressionRepresentationInt
class  ExpressionRepresentationString;            // public ExpressionRepresentation
class  SystemExpressionRepresentationString;        // public ExpressionRepresentation
class   SystemExpressionRepresentationStringReadOnly;    // public ExpressionRepresentationString
class  ExpressionRepresentationMarker;            // public ExpressionRepresentation
class  ExpressionRepresentationArray;            // public ExpressionRepresentation
class  SystemExpressionRepresentationArray;        // public ExpressionRepresentation
class  ExpressionRepresentationWindowRing;        // public ExpressionRepresentation
class  SystemExpressionRepresentationWindowRing;    // public ExpressionRepresentation

class BoundName;


// The data types possible for MLisp values
enum ExpressionType
{
    ISVOID = 0,
    ISINTEGER,
    ISSTRING,
    ISMARKER,
    ISWINDOWS,
    ISARRAY
};

//
//    Holds a reference counted representation of the expression
//
class Expression : public EmacsObject
{
    // evaluation of a MLisp expression
public:
    Expression();
    Expression( const EmacsString &str );
    Expression( int i );
    Expression( const Expression &e );
    Expression( Marker *m );
    Expression( EmacsArray &arr );
    Expression( EmacsWindowRing *win );

    EMACS_OBJECT_FUNCTIONS( Expression )
#ifdef __has_array_new__
#if DBG_ALLOC_CHECK
    void *operator new [](size_t size, const char *fileName, int lineNumber);
    void operator delete [](void *p);
#else
    void *operator new [](size_t size);
    void operator delete [](void *p);
#endif
    // use array delete from EmacsObject
#endif
protected:
    // a value that can be returned from the
    Expression( ExpressionRepresentation *value );
public:
    virtual ~Expression();

    virtual Expression & operator=( const Expression &e );
    Expression & operator=( const EmacsString &str );
    Expression & operator=( int i );
    Expression & operator=( Marker *m );
    Expression & operator=( EmacsArray &arr );
    Expression & operator=( EmacsWindowRing *win );

    void release_expr(void);

    ExpressionType exp_type(void) const;

    int asInt(void) const;
    EmacsString asString(void) const;
    Marker *asMarker(void) const;
    EmacsArray &asArray(void) const;
    EmacsWindowRing *asWindows(void) const;

public:
    virtual ExpressionRepresentation *getRepresentation(void) const;
protected:
    Expression &assign_representation( ExpressionRepresentation *new_rep );

    ExpressionRepresentation *data;
};

//
//    The expression that a system expression describes
//    must be copied into and out. This is because the
//    underling storage changes outside of the control
//    of the MLisp subsystem.
//
class SystemExpression : public Expression
{
    friend class ExpressionRepresentation;
    friend class SystemExpressionRepresentation;
public:
    EMACS_OBJECT_FUNCTIONS( SystemExpression )
    // the only constructor will be given a ExpressionRepresentation that
    // describes a system variables data
    SystemExpression( ExpressionRepresentation *system_expression );
    virtual ~SystemExpression();

    // this version of operator= will update what system_data points at
    virtual Expression & operator=( const Expression &e );

public:
    // this routine will create a new p value with an up todate
    // copy of what system_data points to
    virtual ExpressionRepresentation *getRepresentation(void) const;
private:
    // This points to the system data that cannot be copied where as
    // p points to a ExpressionRepresentation that can be copied.
//    ExpressionRepresentation *system_data;
};

//
//
//    EmacsArray
//
//
class EmacsArray : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( EmacsArray )
    enum { ARRAY_MAX_DIMENSION = 10 };
private:
    class Array : public EmacsObject
    {
        friend class EmacsArray;
    private:
        Array();
        virtual ~Array();

        EMACS_OBJECT_FUNCTIONS( Array )

        int ref_count;                          // number of referances to this structure
        int dimensions;                         // number of dimensions
        int lower_bound[ ARRAY_MAX_DIMENSION ]; // value of the lower bound
        int size[ ARRAY_MAX_DIMENSION ];        // number of elements in this dimension
        int total_size;                         // total number of elements in the array

        Expression *values;
    };

public:
    EmacsArray();
    EmacsArray( int low1, int high1 );
    EmacsArray( int low1, int high1, int low2, int high2 );
    EmacsArray( EmacsArray & );
    virtual ~EmacsArray();

    EmacsArray &operator=( EmacsArray & );


    void create();                              // create the array now
    void addDimension( int low, int high );     // add a dimension
    int dimensions() const;                     // return number of dimensions
    Expression &operator()(int a);              // one dim array - range checked
    Expression &operator()(int a, int b);       // two dim array - range checked

    int array_index( unsigned int arg );

    Expression &getValue( int index );
    void setValue( int index, Expression &value );

    //
    //    MLisp command functions
    //
    static int bounds_of_array_command( void );

private:
    Array *array;
};


//
//
//    Representations
//
//
class ExpressionRepresentation : public EmacsObject
{
    friend class Expression;
    friend class SystemExpression;
public:
    EMACS_OBJECT_FUNCTIONS( ExpressionRepresentation )
    ExpressionRepresentation();
    virtual ~ExpressionRepresentation();

    void add_ref(void);
    int remove_ref(void);        // return TRUE if the ExpressionRepresentation can be deleted

    virtual ExpressionType exp_type(void) const = 0;

    virtual int asInt(void) const;
    virtual EmacsString asString() const;
    virtual Marker *asMarker() const;
    virtual EmacsArray &asArray();
    virtual EmacsWindowRing *asWindows() const;

    //
    //    This function assigns a value to the expression
    //
    virtual void assign_value( ExpressionRepresentation *new_value );

    //
    //    This function updates the value in system_data to its
    //    current value
    //
    virtual void fetch_value(void);

private:
    int ref_count;
};

class ExpressionRepresentationInt : public ExpressionRepresentation
{
public:
    EMACS_OBJECT_FUNCTIONS( ExpressionRepresentationInt )
    ExpressionRepresentationInt( int i );
    virtual ~ExpressionRepresentationInt();

    virtual ExpressionType exp_type(void) const { return ISINTEGER; }

    virtual int asInt(void) const { return exp_int; }
    virtual EmacsString asString() const;

protected:
    int exp_int;
};

#define SYS_EXPR_STRING_OPERATOR_ASSIGN( cls ) \
    public: \
    virtual EmacsString asString() const { return exp_string; } \
    SystemExpressionRepresentation##cls &operator=( const char *new_value ) { exp_string = new_value; return *this; } \
    SystemExpressionRepresentation##cls &operator=( const unsigned char *new_value ) { exp_string = new_value; return *this; } \
    SystemExpressionRepresentation##cls &operator=( const EmacsString &new_value ) { exp_string = new_value; return *this; }

//    operator ++( int ) { return exp_int++; }
#define SYS_EXPR_INT_OPERATOR_ASSIGN( cls ) \
public: \
    operator int() const { return exp_int; } \
    SystemExpressionRepresentation##cls &operator=( int value ) { exp_int = value; return *this; }


class SystemExpressionRepresentationInt : public ExpressionRepresentation
{
public:
    EMACS_OBJECT_FUNCTIONS( SystemExpressionRepresentationInt )
    SystemExpressionRepresentationInt( int i=0 );
    virtual ~SystemExpressionRepresentationInt();

    virtual ExpressionType exp_type(void) const { return ISINTEGER; }

    virtual int asInt(void) const { return exp_int; }
    virtual EmacsString asString() const;
    virtual void assign_value( ExpressionRepresentation *new_value );

    SYS_EXPR_INT_OPERATOR_ASSIGN( Int )
protected:
    int exp_int;
    SystemExpressionRepresentationInt &operator=( SystemExpressionRepresentationInt & );
};

class SystemExpressionRepresentationIntReadOnly : public SystemExpressionRepresentationInt
{
public:
    EMACS_OBJECT_FUNCTIONS( SystemExpressionRepresentationIntReadOnly )
    SystemExpressionRepresentationIntReadOnly( int i=0 );
    virtual ~SystemExpressionRepresentationIntReadOnly();
    virtual void assign_value( ExpressionRepresentation *new_value );
    SYS_EXPR_INT_OPERATOR_ASSIGN( IntReadOnly )
private:
    SystemExpressionRepresentationIntReadOnly &operator=( SystemExpressionRepresentationIntReadOnly & );
};

class SystemExpressionRepresentationIntPositive : public SystemExpressionRepresentationInt
{
public:
    EMACS_OBJECT_FUNCTIONS( SystemExpressionRepresentationIntPositive )
    SystemExpressionRepresentationIntPositive( int i=0 );
    virtual ~SystemExpressionRepresentationIntPositive();
    virtual void assign_value( ExpressionRepresentation *new_value );
    SYS_EXPR_INT_OPERATOR_ASSIGN( IntPositive )

private:
    SystemExpressionRepresentationIntPositive &operator=( SystemExpressionRepresentationIntPositive );
};

class SystemExpressionRepresentationIntBoolean : public SystemExpressionRepresentationInt
{
public:
    EMACS_OBJECT_FUNCTIONS( SystemExpressionRepresentationIntBoolean )
    SystemExpressionRepresentationIntBoolean( int i=0 );
    virtual ~SystemExpressionRepresentationIntBoolean();
    virtual void assign_value( ExpressionRepresentation *new_value );
    SYS_EXPR_INT_OPERATOR_ASSIGN( IntBoolean )
private:
    SystemExpressionRepresentationIntBoolean &operator=( SystemExpressionRepresentationIntBoolean );
};

class SystemExpressionRepresentationDisplayBoolean : public SystemExpressionRepresentationIntBoolean
{
public:
    EMACS_OBJECT_FUNCTIONS( SystemExpressionRepresentationDisplayBoolean )
    SystemExpressionRepresentationDisplayBoolean( int i=0 );
    virtual ~SystemExpressionRepresentationDisplayBoolean();

    virtual void assign_value( ExpressionRepresentation *new_value );
    SYS_EXPR_INT_OPERATOR_ASSIGN( DisplayBoolean )

private:
    SystemExpressionRepresentationDisplayBoolean &operator=( SystemExpressionRepresentationDisplayBoolean & );
};

class ExpressionRepresentationString : public ExpressionRepresentation
{
public:
    EMACS_OBJECT_FUNCTIONS( ExpressionRepresentationString )
    ExpressionRepresentationString( void );
    ExpressionRepresentationString( const EmacsString &string );
    virtual ~ExpressionRepresentationString();

    virtual ExpressionType exp_type(void) const { return ISSTRING; }

    virtual EmacsString asString() const { return exp_string; }
    virtual int asInt(void) const;

protected:
    EmacsString exp_string;
};

class SystemExpressionRepresentationString : public ExpressionRepresentation
{
public:
    EMACS_OBJECT_FUNCTIONS( SystemExpressionRepresentationString )
    SystemExpressionRepresentationString();
    SystemExpressionRepresentationString( const EmacsString &rep );
    virtual ~SystemExpressionRepresentationString();

    virtual ExpressionType exp_type(void) const { return ISSTRING; }

    virtual void assign_value( ExpressionRepresentation *new_value );

//    virtual const EmacsString &asString() const;
    virtual int asInt(void) const;

    const unsigned char *utf8_data() { return exp_string.utf8_data(); }
    const char *sdata() { return exp_string.sdata(); }
    int length() const { return exp_string.length(); }
    int isNull() const { return exp_string.isNull(); }
#if defined( WIN32 )
    int utf16_data_length() const { return exp_string.utf16_data_length(); }
    // this function give unsafe access to the inside of representation
    const wchar_t *utf16_data() const { return exp_string.utf16_data(); }
#endif

    SYS_EXPR_STRING_OPERATOR_ASSIGN( String )
protected:
    EmacsString exp_string;
};

class SystemExpressionRepresentationStringReadOnly : public SystemExpressionRepresentationString
{
public:
    EMACS_OBJECT_FUNCTIONS( SystemExpressionRepresentationStringReadOnly )
    SystemExpressionRepresentationStringReadOnly();
    virtual ~SystemExpressionRepresentationStringReadOnly();
    SystemExpressionRepresentationStringReadOnly( const EmacsString &rep );
    virtual void assign_value( ExpressionRepresentation *new_value );

    SYS_EXPR_STRING_OPERATOR_ASSIGN( StringReadOnly )
};

class ExpressionRepresentationMarker : public ExpressionRepresentation
{
public:
    EMACS_OBJECT_FUNCTIONS( ExpressionRepresentationMarker )
    ExpressionRepresentationMarker( Marker *m );
    virtual ~ExpressionRepresentationMarker();
    virtual ExpressionType exp_type(void) const { return ISMARKER; }

    virtual int asInt(void) const;
    virtual EmacsString asString() const;
    virtual Marker *asMarker() const;

protected:
    Marker *exp_marker;
};

// no system markers

class ExpressionRepresentationArray : public ExpressionRepresentation
{
public:
    EMACS_OBJECT_FUNCTIONS( ExpressionRepresentationArray )
    ExpressionRepresentationArray( EmacsArray & );
    virtual ~ExpressionRepresentationArray();
    virtual ExpressionType exp_type(void) const { return ISARRAY; }

    virtual EmacsArray &asArray();

protected:
    EmacsArray exp_array;
};

class SystemExpressionRepresentationArray : public ExpressionRepresentation
{
public:
    EMACS_OBJECT_FUNCTIONS( SystemExpressionRepresentationArray )
    SystemExpressionRepresentationArray();
    virtual ~SystemExpressionRepresentationArray();
    virtual ExpressionType exp_type(void) const { return ISARRAY; }

    virtual EmacsArray &asArray();

    void replace( EmacsArray &a );
protected:
    EmacsArray exp_array;
};

class ExpressionRepresentationWindowRing : public ExpressionRepresentation
{
public:
    EMACS_OBJECT_FUNCTIONS( ExpressionRepresentationWindowRing )
    ExpressionRepresentationWindowRing( EmacsWindowRing *w );
    virtual ~ExpressionRepresentationWindowRing();
    virtual ExpressionType exp_type(void) const { return ISWINDOWS; }

    virtual EmacsWindowRing *asWindows() const { return exp_windows; }
protected:
    EmacsWindowRing *exp_windows;
};

class SystemExpressionRepresentationWindowRing : public ExpressionRepresentation
{
public:
    EMACS_OBJECT_FUNCTIONS( SystemExpressionRepresentationWindowRing )
    SystemExpressionRepresentationWindowRing();
    SystemExpressionRepresentationWindowRing( EmacsWindowRing *w );
    virtual ~SystemExpressionRepresentationWindowRing();
    virtual ExpressionType exp_type(void) const { return ISWINDOWS; }

    virtual EmacsWindowRing *asWindows() const;
protected:
    EmacsWindowRing *exp_windows;
};

//
//
//    Class that implement specially checked variables
//
//
#if DBG_SER
extern void debug_SER(void);
#define _debug_SER_ debug_SER();
#else
#define _debug_SER_
#endif

#define SYS_VAR_END  };
#define SYS_VAR( Class, Base ) \
    class SystemExpressionRepresentation##Class : public SystemExpressionRepresentation##Base \
    { \
    public: \
        SystemExpressionRepresentation##Class() : SystemExpressionRepresentation##Base() { _debug_SER_ } \
    protected: \
        virtual void assign_value( ExpressionRepresentation *new_value ); \
        virtual void fetch_value(void);

#define SYS_VAR_ARG( Class, Base, Type, Arg ) \
    class SystemExpressionRepresentation##Class : public SystemExpressionRepresentation##Base \
    { \
    public: \
        SystemExpressionRepresentation##Class( Type Arg ) \
            : SystemExpressionRepresentation##Base( Arg ) \
        { _debug_SER_ } \
    protected: \
        virtual void assign_value( ExpressionRepresentation *new_value ); \
        virtual void fetch_value(void); \
        SystemExpressionRepresentation##Class &operator=( SystemExpressionRepresentation##Class & );

#define SYS_VAR_PARM( Class, Base, Type, Parm ) \
    class SystemExpressionRepresentation##Class : public SystemExpressionRepresentation##Base \
    { \
    public: \
        SystemExpressionRepresentation##Class( Type _##Parm ) \
            : SystemExpressionRepresentation##Base() \
            , Parm( _##Parm ) \
        { _debug_SER_ } \
    protected: \
        Type Parm; \
        virtual void assign_value( ExpressionRepresentation *new_value ); \
        virtual void fetch_value(void); \
        SystemExpressionRepresentation##Class &operator=( SystemExpressionRepresentation##Class & );

class SystemExpressionRepresentationEndOfLineStyle;
class SystemExpressionRepresentationBufferEndOfLineStyle;

SYS_VAR_PARM( EndOfLineStyle, String, FIO_EOL_Attribute, exp_eol_attr )
    public:
        SystemExpressionRepresentationEndOfLineStyle() : SystemExpressionRepresentationString(), exp_eol_attr( FIO_EOL__None ) { }
        operator FIO_EOL_Attribute() const { return exp_eol_attr; }
        SystemExpressionRepresentationEndOfLineStyle &operator=( FIO_EOL_Attribute value ) { exp_eol_attr = value; return *this; }
SYS_VAR_END

SYS_VAR( BufferNames, Array ) SYS_VAR_END
SYS_VAR( AbbrevTable, String ) SYS_EXPR_STRING_OPERATOR_ASSIGN( AbbrevTable ) SYS_VAR_END
SYS_VAR( BufferEndOfLineStyle, EndOfLineStyle )
    operator FIO_EOL_Attribute() const { return exp_eol_attr; }
    SystemExpressionRepresentationBufferEndOfLineStyle &operator=( FIO_EOL_Attribute value ) { exp_eol_attr = value; return *this; }
SYS_VAR_END

SYS_VAR_PARM( BoundName, String, BoundName **, procedure ) SYS_VAR_END
SYS_VAR( ProcessKeyHook, String ) SYS_VAR_END
SYS_VAR( AutoFillHook, String ) SYS_VAR_END
SYS_VAR_ARG( GraphicRendition, String, const char *, init_value ) SYS_EXPR_STRING_OPERATOR_ASSIGN( GraphicRendition ) SYS_VAR_END
SYS_VAR_PARM( BackupFileMode, String, int , exp_int ) SYS_EXPR_INT_OPERATOR_ASSIGN( BackupFileMode ) SYS_VAR_END
SYS_VAR_PARM( ControlString, String, int, mask ) SYS_VAR_END
SYS_VAR( IntBufferAllocSize, Int ) SYS_EXPR_INT_OPERATOR_ASSIGN( IntBufferAllocSize ) SYS_VAR_END
SYS_VAR( BufferCheckpointable, Int ) SYS_EXPR_INT_OPERATOR_ASSIGN( BufferCheckpointable ) SYS_VAR_END
SYS_VAR( JournalFrequency, Int ) SYS_EXPR_INT_OPERATOR_ASSIGN( JournalFrequency ) SYS_VAR_END
SYS_VAR( BufferFilename, String ) SYS_VAR_END
SYS_VAR( BufferName, String ) SYS_VAR_END
SYS_VAR( BufferJournalled, IntBoolean ) SYS_EXPR_INT_OPERATOR_ASSIGN( BufferJournalled ) SYS_VAR_END
SYS_VAR( BufferMacroName, String ) SYS_VAR_END
SYS_VAR( BufferSyntaxTable, StringReadOnly ) SYS_VAR_END
SYS_VAR( BufferType, String ) SYS_VAR_END
SYS_VAR( BufferKeymap, String ) SYS_VAR_END
SYS_VAR( CurrentWindows, WindowRing ) SYS_VAR_END
SYS_VAR( LeftMargin, Int ) SYS_VAR_END
SYS_VAR( RightMargin, Int ) SYS_VAR_END
SYS_VAR( DefaultLeftMargin, Int ) SYS_EXPR_INT_OPERATOR_ASSIGN( DefaultLeftMargin ) SYS_VAR_END
SYS_VAR( DefaultRightMargin, Int ) SYS_EXPR_INT_OPERATOR_ASSIGN( DefaultRightMargin ) SYS_VAR_END
SYS_VAR( SyntaxColouring, IntBoolean ) SYS_VAR_END
SYS_VAR( SyntaxArray, IntBoolean ) SYS_VAR_END
SYS_VAR( DefaultTabSize, Int ) SYS_EXPR_INT_OPERATOR_ASSIGN( DefaultTabSize ) SYS_VAR_END
SYS_VAR( TabSize, Int ) SYS_VAR_END
SYS_VAR( StackMaxDepth, Int ) SYS_EXPR_INT_OPERATOR_ASSIGN( StackMaxDepth ) SYS_VAR_END
SYS_VAR( BufferModified, Int ) SYS_VAR_END
SYS_VAR( CtrlXSwap, Int ) SYS_VAR_END
SYS_VAR( ScreenLength, IntPositive ) SYS_EXPR_INT_OPERATOR_ASSIGN( ScreenLength ) SYS_VAR_END
SYS_VAR( ScreenWidth, IntPositive ) SYS_EXPR_INT_OPERATOR_ASSIGN( ScreenWidth ) SYS_VAR_END
SYS_VAR( TermOutputSize, Int ) SYS_EXPR_INT_OPERATOR_ASSIGN( TermOutputSize ) SYS_VAR_END
SYS_VAR( WindowColumn, IntPositive ) SYS_EXPR_INT_OPERATOR_ASSIGN( WindowColumn ) SYS_VAR_END
SYS_VAR( WindowSize, IntPositive ) SYS_EXPR_INT_OPERATOR_ASSIGN( WindowSize ) SYS_VAR_END
SYS_VAR( WindowWidth, IntPositive ) SYS_EXPR_INT_OPERATOR_ASSIGN( WindowWidth ) SYS_VAR_END
SYS_VAR_ARG( TermProtocolMode, IntBoolean, int, value ) SYS_EXPR_INT_OPERATOR_ASSIGN( TermProtocolMode ) SYS_VAR_END
SYS_VAR( DisplayBooleanHighlight, DisplayBoolean ) SYS_VAR_END
SYS_VAR( DisplayBooleanNonPrinting, DisplayBoolean ) SYS_VAR_END
SYS_VAR( DisplayBooleanEOF, DisplayBoolean ) SYS_VAR_END
SYS_VAR( DisplayBooleanC1, DisplayBoolean ) SYS_VAR_END
SYS_VAR( CommentColumn, IntPositive ) SYS_VAR_END
SYS_VAR( AbbrevOn, IntBoolean ) SYS_VAR_END
SYS_VAR( FoldCase, IntBoolean ) SYS_VAR_END
SYS_VAR( IndentUseTab, IntBoolean ) SYS_VAR_END
SYS_VAR( ReadOnly, IntBoolean ) SYS_VAR_END
SYS_VAR( ReplaceMode, IntBoolean ) SYS_VAR_END
SYS_VAR( WrapLines, IntBoolean ) SYS_VAR_END
SYS_VAR( ModeString, String ) SYS_VAR_END
SYS_VAR( ModeFormat, String ) SYS_VAR_END
SYS_VAR( PrefixString, String ) SYS_VAR_END
SYS_VAR( PrefixArgument, Int ) SYS_VAR_END
SYS_VAR( PrefixArgumentProvided, Int ) SYS_VAR_END
SYS_VAR( ElapseTime, IntReadOnly ) SYS_VAR_END
