//
//    Copyright (c) 1995-2010 Barry A. Scott
//

//
//    This module contains the implementation of classes
//        Expression, ExpressionRepresentation
//        VariableName and Binding
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


//
//
//    VariableName implementation
//
//
VariableName::VariableName( const EmacsString &name, Binding *binding )
: v_name( name )
, v_binding( binding )
{
    define();
}

VariableName::~VariableName()
{ }

bool VariableName::declareGlobal()
{
    if( v_binding == NULL )
    {
        pushBinding();
        return true;
    }
    if( v_binding->b_buffer_specific )
    {
        error( FormatString( "%s is a buffer specific variable" ) << v_name );
        return false;
    }

    return true;
}

bool VariableName::declareBufferSpecific()
{
    if( v_binding == NULL )
    {
        pushBinding();
        v_binding->b_buffer_specific = true;
        v_binding->b_is_default = true;

        return true;
    }
    if( !v_binding->b_buffer_specific )
    {
        error( FormatString( "%s is a global variable" ) << v_name );
        return false;
    }

    return true;
}

bool VariableName::normalValue( Expression &value ) const
{
    Binding *b = v_binding;
    if( b == NULL )
        return false;

    if( b->b_buffer_specific )
    {
        b = resolve_buffer_specific();
        if( b == NULL )
            return false;
    }

    value = *b->b_exp;
    return true;
}

bool VariableName::assignNormal( const Expression &new_value )
{
    Binding *b = v_binding;
    if( b == NULL )
        return false;

    if( b->b_buffer_specific )
    {
        b = resolve_buffer_specific();
        if( b == NULL )
            return false;

        if( b->b_is_default )
        {
            b = EMACS_NEW Binding( new_value, current_buffer_for_mlisp() );
            b->b_inner = v_binding;
            v_binding = b;
            return true;
        }
    }

    *b->b_exp = new_value;

    return true;
}

bool VariableName::defaultValue( Expression &value ) const
{
    // find the default
    for( Binding *b = v_binding; b != NULL; b = b->b_inner )
    {
        if( b->b_is_default )
        {
            value = *b->b_exp;
            return true;
        }
    }

    return false;
}

bool VariableName::assignDefault( const Expression &new_value ) const
{
    // find the default
    for( Binding *b = v_binding; b != NULL; b = b->b_inner )
    {
        if( b->b_is_default )
        {
            *b->b_exp = new_value;
            return true;
        }
    }

    return false;
}

//
//    Returns:
//        v_binding if not buffer specific
//        binding that is specific to the current buffer
//        the default binding
//        NULL if any error
//
Binding *VariableName::resolve_buffer_specific() const
{
    Binding *b = v_binding;

    if( b != NULL && b->b_buffer_specific )
    {
        EmacsBuffer *buf = current_buffer_for_mlisp();

        while( b != NULL
        && !b->b_is_default
        && b->b_buffer_specific
        && b->b_local_to != buf )
            b = b->b_inner;

        if( b == NULL || !b->b_buffer_specific )
        {
            error( "error resolving buffer-specific variable (internal error)" );
            return NULL;    // This should never happen!
        }
    }

    return b;
}

void VariableName::pushBinding()
{
    Binding *b = EMACS_NEW Binding( Expression( int(0) ) );
    b->b_inner = v_binding;
    v_binding = b;
}

void VariableName::pushBinding( const Expression &value )
{
    Binding *b = EMACS_NEW Binding( value );
    b->b_inner = v_binding;
    v_binding = b;
}

void VariableName::popBinding()
{
    Binding *b = v_binding;
    v_binding = b->b_inner;
    delete b;
}


//
//
//    Binding implementation
//
//
Binding::Binding( const Expression &exp )
: b_inner( NULL )
, b_exp( EMACS_NEW Expression( exp ) )
, b_local_to( NULL )
, b_buffer_specific( 0 )
, b_is_default( 0 )
{ }

Binding::Binding( const Expression &exp, EmacsBuffer *buf )
: b_inner( NULL )
, b_exp( EMACS_NEW Expression( exp ) )
, b_local_to( buf )
, b_buffer_specific( 1 )
, b_is_default( 0 )
{ }

// used by restore_var only
Binding::Binding( Expression *exp )
: b_inner( NULL )
, b_exp( exp )
, b_local_to( NULL )
, b_buffer_specific( 0 )
, b_is_default( 0 )
{ }

Binding::Binding( SystemExpression *exp, int is_default )
: b_inner( NULL )
, b_exp( exp )
, b_local_to( NULL )
, b_buffer_specific( 0 )
, b_is_default( is_default )
{
    emacs_assert( exp != NULL );
}

Binding::Binding( SystemExpression *exp, Binding *default_value )
: b_inner( default_value )
, b_exp( exp )
, b_local_to( NULL )
, b_buffer_specific( 0 )
, b_is_default( 0 )
{
    emacs_assert( exp != NULL );
}

Binding::~Binding()
{
    delete b_exp;
}

//
//
//    Expression implementation
//
//
Expression::~Expression()
{
    release_expr();
}

Expression::Expression()
: data( NULL )
{ }

Expression::Expression( int i )
{
    data = EMACS_NEW ExpressionRepresentationInt( i );
    data->add_ref();
}

Expression::Expression( const EmacsString &str )
{
    data = EMACS_NEW ExpressionRepresentationString( str );
    data->add_ref();
}

Expression::Expression( Marker *m )
{
    data = EMACS_NEW ExpressionRepresentationMarker( m );
    data->add_ref();
}
Expression::Expression( EmacsArray &arr )
{
    data = EMACS_NEW ExpressionRepresentationArray( arr );
    data->add_ref();
}
Expression::Expression( EmacsWindowRing *win )
{
    data = EMACS_NEW ExpressionRepresentationWindowRing( win );
    data->add_ref();
}

Expression::Expression( const Expression &e )
{
    if( e.data != NULL )
        e.data->add_ref();
    data = e.data;
}

#if 0
Expression::Expression( Expression *e )
{
    emacs_assert( e != NULL );

    e->data->add_ref();
    data = e->data;
}
#endif
Expression::Expression( ExpressionRepresentation *system_expression )
: data( system_expression )
{
//    data->add_ref();
}

ExpressionRepresentation *Expression::getRepresentation(void) const
{
    // simply return the data
    return data;
}

Expression &Expression::operator=( int i )
{
    return assign_representation( EMACS_NEW ExpressionRepresentationInt( i ) );
}

Expression &Expression::operator=( const EmacsString &str )
{
    return assign_representation( EMACS_NEW ExpressionRepresentationString( str ) );
}

Expression &Expression::operator=( Marker *m )
{
    return assign_representation( EMACS_NEW ExpressionRepresentationMarker( m ) );
}
Expression &Expression::operator=( EmacsArray &arr )
{
    return assign_representation( EMACS_NEW ExpressionRepresentationArray( arr ) );
}
Expression &Expression::operator=( EmacsWindowRing *win )
{
    return assign_representation( EMACS_NEW ExpressionRepresentationWindowRing( win ) );
}

Expression &Expression::operator=( const Expression &e )
{
    return assign_representation( e.getRepresentation() );
}

Expression &Expression::assign_representation( ExpressionRepresentation *new_data )
{
    if( new_data != NULL )
        new_data->add_ref();
    if( data != NULL && data->remove_ref() )
        delete data;
    data = new_data;
    return *this;
}

ExpressionType Expression::exp_type() const
{
    if( data != NULL )
        return data->exp_type();
    return ISVOID;
}

void Expression::release_expr()
{
    if( data == NULL )
        return;

    if( data->remove_ref() )
        delete data;

    data = NULL;
}

int Expression::asInt() const
{
    if( data == NULL )
        throw EmacsExceptionExpressionNotInteger();

    // make sure the value is up todate
    data->fetch_value();

    return data->asInt();
}

EmacsString Expression::asString() const
{
    if( data == NULL )
        throw EmacsExceptionExpressionNotString();

    // make sure the value is up todate
    data->fetch_value();

    return data->asString();
}

Marker * Expression::asMarker() const
{
    if( data == NULL )
        throw EmacsExceptionExpressionNotMarker();

    // make sure the value is up todate
    data->fetch_value();

    return data->asMarker();
}

EmacsArray & Expression::asArray() const
{
    if( data == NULL )
        throw EmacsExceptionExpressionNotArray();

    // make sure the value is up todate
    data->fetch_value();

    return data->asArray();
}

EmacsWindowRing * Expression::asWindows() const
{
    if( data == NULL )
        throw EmacsExceptionExpressionNotWindows();

    // make sure the value is up todate
    data->fetch_value();

    return data->asWindows();
}

//
//
//    SystemExpression implementation
//
//
SystemExpression::SystemExpression( ExpressionRepresentation *system_expression )
: Expression( system_expression )
{
}

SystemExpression::~SystemExpression()
{ }

Expression & SystemExpression::operator=( const Expression &e )
{
    // call the virtual assign value function to deal with the assignment
    data->assign_value( e.getRepresentation() );

    return *this;
}

//
//    return a copy of the data in system variable
//
ExpressionRepresentation *SystemExpression::getRepresentation(void) const
{
    ExpressionRepresentation *result = NULL;

    // make sure the value is up todate
    data->fetch_value();

    switch( exp_type() )
{
    case ISMARKER:    // no marker system variables
    case ISVOID:    // no void system variables
    default:    // can happen...
        emacs_assert(false);    // crash!
        // default to zero in case of serious error
        result = EMACS_NEW ExpressionRepresentationInt( int(0) );
        break;

    case ISINTEGER:
        result = EMACS_NEW ExpressionRepresentationInt( data->asInt() );
        break;
    case ISSTRING:
        result = EMACS_NEW ExpressionRepresentationString( data->asString() );
        break;
    case ISARRAY:
    {
        result = EMACS_NEW ExpressionRepresentationArray( data->asArray() );
    }
        break;
    case ISWINDOWS:
        result = EMACS_NEW ExpressionRepresentationWindowRing( data->asWindows() );
        break;
}

    return result;
}

//
//
//    ExpressionRepresentation implementation
//
//
ExpressionRepresentation::ExpressionRepresentation()
: ref_count(0)
{ }

ExpressionRepresentation::~ExpressionRepresentation()
{
    emacs_assert( ref_count == 0 );
}

int ExpressionRepresentation::asInt() const
{
    throw EmacsExceptionExpressionNotInteger();
}

EmacsString ExpressionRepresentation::asString() const
{
    throw EmacsExceptionExpressionNotString();
}

Marker * ExpressionRepresentation::asMarker() const
{
    throw EmacsExceptionExpressionNotMarker();
}

EmacsArray & ExpressionRepresentation::asArray()
{
    throw EmacsExceptionExpressionNotArray();
}

EmacsWindowRing * ExpressionRepresentation::asWindows() const
{
    throw EmacsExceptionExpressionNotWindows();
}

void ExpressionRepresentation::assign_value( ExpressionRepresentation * /*new_value*/ )
{
    throw EmacsInternalError("ExpressionRepresentation::assign_value called");
}

void ExpressionRepresentation::fetch_value(void)
{ }

void ExpressionRepresentation::add_ref()
{
    emacs_assert( ref_count >= 0 );
    ref_count++;
}

int ExpressionRepresentation::remove_ref()
{
    ref_count--;
    emacs_assert(ref_count >=0);
    return ref_count == 0;
}

//
//
//    ExpressionRepresentationInt implementation
//
//
ExpressionRepresentationInt::ExpressionRepresentationInt( int i )
: exp_int( i )
{ }

EmacsString ExpressionRepresentationInt::asString() const
{
    EmacsString result = FormatString("%d") << exp_int;

    return result;
}

ExpressionRepresentationInt::~ExpressionRepresentationInt(void)
{ }

//
//
//    SystemExpressionRepresentationInt implementation
//
//
SystemExpressionRepresentationInt::SystemExpressionRepresentationInt( int i )
: exp_int( i )
{ }

SystemExpressionRepresentationInt::~SystemExpressionRepresentationInt()
{ }

void SystemExpressionRepresentationInt::assign_value( ExpressionRepresentation *new_value )
{
    // update the variable
    exp_int = new_value->asInt();
}

EmacsString SystemExpressionRepresentationInt::asString() const
{
    EmacsString result( FormatString("%d") << exp_int );

    return result;
}

SystemExpressionRepresentationInt &SystemExpressionRepresentationInt::operator=(class SystemExpressionRepresentationInt &i)
{
    exp_int = i.exp_int;
    return *this;
}
//
//
//    SystemExpressionRepresentationIntReadOnly implementation
//
//
SystemExpressionRepresentationIntReadOnly::SystemExpressionRepresentationIntReadOnly( int i )
: SystemExpressionRepresentationInt( i )
{ }

SystemExpressionRepresentationIntReadOnly::~SystemExpressionRepresentationIntReadOnly()
{ }

void SystemExpressionRepresentationIntReadOnly::assign_value( ExpressionRepresentation * /*new_value*/ )
{
    throw EmacsExceptionVariableReadOnly();
}

//
//
//    SystemExpressionRepresentationIntPositive implementation
//
//
SystemExpressionRepresentationIntPositive::SystemExpressionRepresentationIntPositive( int i )
: SystemExpressionRepresentationInt( i )
{ }

SystemExpressionRepresentationIntPositive::~SystemExpressionRepresentationIntPositive()
{ }

void SystemExpressionRepresentationIntPositive::assign_value( ExpressionRepresentation *new_value )
{
    int value = new_value->asInt();

    if( value < 0 )
        throw EmacsExceptionVariablePositive();

    SystemExpressionRepresentationInt::assign_value( new_value );
}

//
//
//    SystemExpressionRepresentationIntBoolean implementation
//
//
SystemExpressionRepresentationDisplayBoolean::SystemExpressionRepresentationDisplayBoolean( int i )
        : SystemExpressionRepresentationIntBoolean( i )
    { }

SystemExpressionRepresentationDisplayBoolean::~SystemExpressionRepresentationDisplayBoolean()
{ }

SystemExpressionRepresentationIntBoolean::SystemExpressionRepresentationIntBoolean( int i )
: SystemExpressionRepresentationInt( i )
{ }

SystemExpressionRepresentationIntBoolean::~SystemExpressionRepresentationIntBoolean()
{ }

void SystemExpressionRepresentationIntBoolean::assign_value( ExpressionRepresentation *new_value )
{
    int value = new_value->asInt();

    if( value != 0 && value != 1 )
        throw EmacsExceptionVariableBoolean();

    SystemExpressionRepresentationInt::assign_value( new_value );
}

//
//
//    ExpressionRepresentationString implementation
//
//
ExpressionRepresentationString::ExpressionRepresentationString( const EmacsString &str )
: exp_string( str )
{ }

ExpressionRepresentationString::~ExpressionRepresentationString()
{ }

//
//
//    ExpressionRepresentationString implementation
//
//
SystemExpressionRepresentationString::SystemExpressionRepresentationString()
: exp_string()
{ }

SystemExpressionRepresentationString::SystemExpressionRepresentationString( const EmacsString &rep )
: exp_string( rep )
{ }

SystemExpressionRepresentationString::~SystemExpressionRepresentationString()
{
//    remove_ref();
}

void SystemExpressionRepresentationString::assign_value( ExpressionRepresentation *new_value )
{
    exp_string = new_value->asString();
}

int SystemExpressionRepresentationString::asInt() const
{
    const EmacsChar_t *p = exp_string.unicode_data();
    int neg = 0;

    while( unicode_is_mlisp_space( *p ) )
        p++;

    if( *p == '+' || *p == '-' )
    {
        neg = *p++ == '-';
    }
    while( unicode_is_mlisp_space( *p ) )
        p++;

    int n = 0;
    while( unicode_is_digit( *p ) || unicode_is_mlisp_space( *p ) )
    {
        if( unicode_is_digit( *p ) )
            n = n * 10 + *p - '0';
        p++;
    }
    if( *p != 0 )
        error( FormatString("String to integer conversion error: \"%s\"") << exp_string );
    if( neg )
        n = -n;
    return n;
}

int ExpressionRepresentationString::asInt() const
{
    const EmacsChar_t *p = exp_string.unicode_data();
    int neg = 0;

    while( unicode_is_mlisp_space( *p ) )
        p++;

    if( *p == '+' || *p == '-' )
    {
        neg = *p++ == '-';
    }
    while( unicode_is_mlisp_space( *p ) )
        p++;

    int n = 0;
    while( unicode_is_digit( *p ) || unicode_is_mlisp_space( *p ) )
    {
        if( unicode_is_digit( *p ) )
            n = n * 10 + *p - '0';
        p++;
    }
    if( *p != 0 )
        error( FormatString("String to integer conversion error: \"%s\"") << exp_string );
    if( neg )
        n = -n;
    return n;
}

//
//
//    SystemExpressionRepresentationStringReadOnly
//
//
SystemExpressionRepresentationStringReadOnly::SystemExpressionRepresentationStringReadOnly()
: SystemExpressionRepresentationString()
{ }

SystemExpressionRepresentationStringReadOnly::~SystemExpressionRepresentationStringReadOnly()
{ }

SystemExpressionRepresentationStringReadOnly::SystemExpressionRepresentationStringReadOnly( const EmacsString &rep )
: SystemExpressionRepresentationString( rep )
{ }

void SystemExpressionRepresentationStringReadOnly::assign_value( ExpressionRepresentation * /*new_value*/ )
{
    throw EmacsExceptionVariableReadOnly();
}

//
//
//    ExpressionRepresentationMarker implementation
//
//
ExpressionRepresentationMarker::ExpressionRepresentationMarker( Marker *m )
: exp_marker( m )
{ }

ExpressionRepresentationMarker::~ExpressionRepresentationMarker()
{
    delete exp_marker;
}

int ExpressionRepresentationMarker::asInt() const
{
    return exp_marker->get_mark();
}

EmacsString ExpressionRepresentationMarker::asString() const
{
    return exp_marker->m_buf->b_buf_name;
}

Marker * ExpressionRepresentationMarker::asMarker() const
{
    return exp_marker;
}

//
//
//    ExpressionRepresentationArray implementation
//
//
ExpressionRepresentationArray::ExpressionRepresentationArray( EmacsArray &a )
: exp_array( a )
{ }

ExpressionRepresentationArray::~ExpressionRepresentationArray()
{ }

EmacsArray & ExpressionRepresentationArray::asArray()
{
    return exp_array;
}

//
//
//    SystemExpressionRepresentationArray implementation
//
//
SystemExpressionRepresentationArray::SystemExpressionRepresentationArray()
: exp_array()
{ }

SystemExpressionRepresentationArray::~SystemExpressionRepresentationArray()
{ }


EmacsArray & SystemExpressionRepresentationArray::asArray()
{
    return exp_array;
}

void SystemExpressionRepresentationArray::replace( EmacsArray &a )
{
    exp_array = a;
}

//
//
//    ExpressionRepresentationWindowRing implementation
//
//
ExpressionRepresentationWindowRing::ExpressionRepresentationWindowRing( EmacsWindowRing *w )
: exp_windows( w )
{
    w->wr_ref_count++;
}

ExpressionRepresentationWindowRing::~ExpressionRepresentationWindowRing()
{
    dest_window_ring( exp_windows );
}

//
//
//    SystemExpressionRepresentationWindowRing implementation
//
//
SystemExpressionRepresentationWindowRing::SystemExpressionRepresentationWindowRing(void)
: exp_windows( NULL )
{ }

SystemExpressionRepresentationWindowRing::SystemExpressionRepresentationWindowRing( EmacsWindowRing *w )
: exp_windows( w )
{
    w->wr_ref_count++;
}

SystemExpressionRepresentationWindowRing::~SystemExpressionRepresentationWindowRing()
{ }

EmacsWindowRing * SystemExpressionRepresentationWindowRing::asWindows() const
{
    return exp_windows;
}
