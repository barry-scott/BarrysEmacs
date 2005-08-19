//
//
//	emarray.cpp - implement emacs arrays
//
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


EmacsArray::Array::Array()
	: ref_count(1)				// number of referances to this structure
	, dimensions(0)				// number of dimensions
	, total_size(0)				// total number of elements in the array
	, values( NULL )
	{
	for( int i=0; i<ARRAY_MAX_DIMENSION; i++ )
		{
		lower_bound[i] = 0;	// value of the lower bound
		size[i] = 0;		// number of elements in this dimension
		}
	}

EmacsArray::Array::~Array()
	{
	emacs_assert( ref_count == 0 );
#ifndef __has_array_new__
	void *memory = values;
	if( memory != NULL )
		{
		for( int i=0; i<total_size; i++ )
			values[i].~Expression();
		emacs_free( memory );
		}
#else
	delete [] values;
#endif
	}

EmacsArray::EmacsArray()
	: array( EMACS_NEW Array )
	{ }

EmacsArray::EmacsArray( int low1, int high1 )
	: array( EMACS_NEW Array )
	{
	addDimension( low1, high1 );
	create();
	}

EmacsArray::EmacsArray( int low1, int high1, int low2, int high2 )
	: array( EMACS_NEW Array )
	{
	addDimension( low1, high1 );
	addDimension( low2, high2 );
	create();
	}
EmacsArray::EmacsArray( EmacsArray &a )
	: array( a.array )
	{
	array->ref_count++;
	}

EmacsArray::~EmacsArray()
	{
	array->ref_count--;
	if( array->ref_count == 0 )
		delete array;
	}

EmacsArray &EmacsArray::operator=( EmacsArray &a )
	{
	a.array->ref_count++;
	array->ref_count--;
	if( array->ref_count == 0 )
		delete array;

	array = a.array;	
	return *this;
	}

// create the array now
void EmacsArray::create()
	{
#ifndef __has_array_new__
	char *memory = (char *)emacs_malloc
		(
		sizeof( Expression ) * array->total_size
		, malloc_type_emacs_object_Expression_Vector
#if DBG_ALLOC_CHECK
		, THIS_FILE, __LINE__
#endif
		);
	array->values = (Expression *)memory;
	for( int i=0; i<array->total_size; i++ )
		::new((void *)&memory[i*sizeof( Expression )]) Expression;
#else
	array->values = EMACS_NEW Expression[array->total_size];
#endif
	}


// add a dimension
void EmacsArray::addDimension( int low, int high )
	{
	// check that create has not been called
	emacs_assert( array->values == NULL );
	// value check
	emacs_assert( low <= high );

	int dim = array->dimensions;
	emacs_assert( dim < ARRAY_MAX_DIMENSION );

	array->lower_bound[dim] = low;
	int size = array->size[dim] = high-low+1;
	
	if( dim == 0 )
		array->total_size = size;
	else
		array->total_size *= size;

	array->dimensions++;
	}

// return number of dimensions
int EmacsArray::dimensions() const
	{
	return array->dimensions;
	}

// one dim array - range checked
Expression &EmacsArray::operator()(int a)
	{
	emacs_assert( array->dimensions == 1 );

	int index = a - array->lower_bound[0];
	emacs_assert( index >= 0 && index < array->size[0] );

	return array->values[index];
	}

// two dim array - range checked
Expression &EmacsArray::operator()(int a, int b)
	{
	emacs_assert( array->dimensions == 2 );

	int index1 = a - array->lower_bound[0];
	emacs_assert( index1 >= 0 && index1 < array->size[0] );

	int index2 = b - array->lower_bound[1];
	emacs_assert( index2 >= 0 && index2 < array->size[1] );

	return array->values[index1*array->size[1] + index2];
	}

Expression &EmacsArray::getValue( int index )
	{
	emacs_assert( index >= 0 && index < array->total_size );

	return array->values[index];
	}

void EmacsArray::setValue( int index, Expression &value )
	{
	emacs_assert( index >= 0 && index < array->total_size );

	array->values[index] = value;
	}
