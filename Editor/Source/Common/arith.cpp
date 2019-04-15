//module arith
//    (
//    ident    = 'V5.0 Emacs',
//    addressing_mode( nonexternal=long_relative, external=general )
//    ) =
//begin
//    Copyright (c) 1982, 1983, 1984, 1985
//        Barry A. Scott and nick Emery

// functions to handle MLisp arithmetic

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


int check_args( int min, int max );
int eval_arg( int n );
EmacsWindowRing * window_arg( int n );
int numeric_arg( int n );
int string_arg( int n );

int add_command( void );
int not_command( void );
int subtract_command( void );
int times_command( void );
int divide_command( void );
int mod_command( void );
int shift_left_command( void );
int shift_right_command( void );
int and_command( void );
int or_command( void );
int xor_command( void );
int equal_command( void );
int not_equal_command( void );
int less_command( void );
int less_equal_command( void );
int greater_command( void );
int greater_equal_command( void );

// check that we were given at least min and at most max arguments.
// Returns true iff there was an error.

int check_args( int min, int max )
{
    ProgramNode *p;

    p = cur_exec;
    if( ml_err )
        return 1;
    if( p == 0 )
    {
        if( min != 0 || max != 0 )
        {
            error( "No arguments provided to MLisp function!");
            return 1;
        }
        else
        {
            return 0;
        }
    }
    if( p->p_nargs < min || (p->p_nargs > max && min <= max) )
    {
        error( FormatString("Too %s arguments to \"%s\"") <<
            (p->p_nargs < min ? "few" : "many") <<
            p->p_proc->b_proc_name );
        return 1;
    }
    return 0;
}

// Evaluate the n th argument. Returns true if the evaluation was successful
int eval_arg( int n )
{
    if( ml_err )
        return 0;
    ProgramNode *p = cur_exec;
    if( p == 0 || p->p_nargs < n )
    {
        error( FormatString("Missing argument %d to %s") << n <<
            (p != NULL ? p->p_proc->b_proc_name : EmacsString("MLisp function")) );
        return 0;
    }
    ProgramNode *arg = p->arg( n );
    exec_prog( arg );
    if( ml_err )
        return 0;
    if( ml_value.exp_type() == ISVOID )
    {
        error( FormatString("\"%s\" did not return a value; \"%s\" was expecting it to.") <<
            arg->p_proc->b_proc_name <<
            p->p_proc->b_proc_name );
        return 0;
    }
    return 1;
}

// Evaluate and return the n th window argument
EmacsWindowRing * window_arg( int n )
{
    if( !eval_arg( n ) )
        return 0;
    try
    {
        return ml_value.asWindows();
    }
    catch( EmacsExceptionExpressionNotWindows & )
    {
        error( "windows argument expected.");
        return 0;
    }
}

// Evaluate and return the n th numeric argument
int numeric_arg( int n )
{
    if( !eval_arg( n ) )
        return 0;

    try
    {
        int value = ml_value.asInt();
        ml_value = value;
        return value;
    }
    catch( EmacsExceptionExpressionNotInteger & )
    {
        error( "Numeric argument expected." );
    }

    return 0;
}

//
// Evaluate and return the n th string argument in ml_value (returns
// true if all is well)
//
int string_arg( int n )
{
    if( !eval_arg( n ) )
        return 0;

    try
    {
        ml_value = ml_value.asString();
        return 1;
    }
    catch( EmacsExceptionExpressionNotString & )
    {
        error( "String argument expected." );
    }

    return 0;
}

class arith_op
{
public:
    virtual ~arith_op() {};
    virtual int action( int a, int b ) = 0;
};

static int arith_command( arith_op &op )
{
    if( check_args( 1, 0 ) )
        return 0;

    int result = numeric_arg( 1 );
    if( ml_err )
        return 0;

    for( int i=2; i<=cur_exec->p_nargs; i++ )
        if( ml_err )
            break;
        else
        {
            int arg = numeric_arg( i );
            result = op.action( result, arg );
        }
    ml_value = result;
    return 0;
}

class op_plus : public arith_op { public: int action( int a, int b ) { return a + b; } };
class op_minus : public arith_op { public: int action( int a, int b ) { return a - b; } };
class op_times : public arith_op { public: int action( int a, int b ) { return a * b; } };
class op_divide : public arith_op { public: int action( int a, int b )
{
    if( b == 0 )
    {
        error( "Division by zero");return 0;
    }
    else
        return a / b;
}};
class op_modulas : public arith_op { public: int action( int a, int b )
{ if( b == 0 )
{error( "Modulas by zero");return 0;}
    else
        return a % b; }};
class op_shift_left : public arith_op { public: int action( int a, int b ) { return a << b; } };
class op_shift_right : public arith_op { public: int action( int a, int b ) { return a >> b; } };
class op_bit_and : public arith_op { public: int action( int a, int b ) { return a & b; } };
class op_bit_or : public arith_op { public: int action( int a, int b ) { return a | b; } };
class op_bit_xor : public arith_op { public: int action( int a, int b ) { return a ^ b; } };


int add_command( void ) { op_plus op; return arith_command( op ); }
int times_command( void ) { op_times op; return arith_command( op ); }
int divide_command( void ) { op_divide op; return arith_command( op ); }
int mod_command( void ) { op_modulas op; return arith_command( op ); }
int shift_left_command( void ) { op_shift_left op; return arith_command( op ); }
int shift_right_command( void ) { op_shift_right op; return arith_command( op ); }
int and_command( void ) { op_bit_and op; return arith_command( op ); }
int or_command( void ) { op_bit_or op; return arith_command( op ); }
int xor_command( void ) { op_bit_xor op; return arith_command( op ); }
int subtract_command( void )
{
    // monadic operator
    if( cur_exec->p_nargs == 1 )
    {
        ml_value = - numeric_arg( 1 );
        return 0;
    }

    // dyadic and above
    op_minus op; return arith_command( op );
}

int not_command( void )
{
    if( check_args( 1, 1 ) )
        return 0;

    ml_value = numeric_arg( 1 ) == 0;
    return 0;
}


class op_compare
{
public:
    virtual ~op_compare() {};
    virtual bool compare( int left, int right ) = 0;
    virtual bool compare( const EmacsString &left, const EmacsString &right ) = 0;
};

static int compare_command( op_compare &op )
{
    if( check_args( 2, 2 ) )
        return 0;
    if( !eval_arg( 1 ) )
        return 0;

    EmacsString left_str;   // left string operand to a comparison operator
    int left_int = 0;       // left integer operand to a comparison operator

    ExpressionType left_type = ml_value.exp_type();
    switch( left_type )
    {
    case ISMARKER:
    case ISINTEGER:
        left_int = ml_value.asInt();
        left_type = ISINTEGER;
        break;

    case ISSTRING:
        left_str = ml_value.asString();
        break;

    default:
        error( "Illegal operand to comparison operator");
        return 0;
    }

    if( !eval_arg( 2 ) )
        return 0;

    ExpressionType right_type = ml_value.exp_type();

    int right_int = 0;
    EmacsString right_str;

    switch( right_type )
    {
    case ISINTEGER:
        if( left_type == ISSTRING )
        {
            left_int = atoi( left_str );
            left_type = ISINTEGER;
        }
        else
        {
            right_int = ml_value.asInt();
        }
        break;

    case ISSTRING:
        if( left_type != ISSTRING )
        {
            right_int = ml_value.asInt();
        }
        else
        {
            right_str = ml_value.asString();
        }
        break;

    case ISMARKER:
        if( left_type == ISSTRING )
        {
            left_int = atoi( left_str );
            left_type = ISINTEGER;
        }
        else
        {
            right_int = ml_value.asInt();
        }
        break;

    default:
        break;
    }

    switch( left_type )
    {
    case ISINTEGER:
        ml_value = op.compare( left_int, right_int );
        break;

    case ISSTRING:
        ml_value = op.compare( left_str, right_str );
        break;

    default:
        break;
    }

    return 0;
}

#define    DEFINE_COMPARE_COMMAND( __name__, __op__ ) \
    class op_compare_##__name__ : public op_compare \
{ public: \
        virtual bool compare( int left, int right ) \
{ return left __op__ right; } \
        virtual bool compare( const EmacsString &left, const EmacsString &right ) \
{ return left __op__ right; } }; \
    int __name__##_command( void ) { op_compare_##__name__ op; return compare_command( op ); }

DEFINE_COMPARE_COMMAND( equal, == )
DEFINE_COMPARE_COMMAND( not_equal, != )
DEFINE_COMPARE_COMMAND( less, < )
DEFINE_COMPARE_COMMAND( less_equal, <= )
DEFINE_COMPARE_COMMAND( greater, > )
DEFINE_COMPARE_COMMAND( greater_equal, >= )
