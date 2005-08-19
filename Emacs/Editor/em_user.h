//
//	em_user.h
//
//	Emacs External image call back support functions
//

typedef enum EmacsCallBackFunctions
	{
	EMACS__K_MESSAGE,		// Display a string as a message
	EMACS__K_ERROR,			// Display a string as an error message
	EMACS__K_SITFOR,		// Re-displays and waits for n 1/10s sec
	EMACS__K_ARG_COUNT,		// Return the argument count
	EMACS__K_CHECK_ARGS,     	// Check the arguments, and return an error
	EMACS__K_STRING_ARG,		// Return Nth arg as a string
	EMACS__K_NUMERIC_ARG,		// Return Nth arg as an integer
	EMACS__K_STRING_RESULT,		// Make the function return a string
	EMACS__K_NUMERIC_RESULT,	// Make the function return a number
	EMACS__K_BUFFER_SIZE,		// Returns the size of the buffer
	EMACS__K_DOT,			// Returns the components of dot
	EMACS__K_MARK,			// Returns the components of mark
	EMACS__K_SET_MARK,		// Set the mark to where dot is
	EMACS__K_BUFFER_NAME,		// Return current buffer name
	EMACS__K_USE_BUFFER,		// Temp use buffer
	EMACS__K_GOTO_CHARACTER,	// Sets dot to the specified character
	EMACS__K_INSERT_CHARACTER,	// Inserts the character at dot
	EMACS__K_INSERT_STRING,		// Inserts a string at dot
	EMACS__K_DELETE_CHARACTERS,	// Delete character, +ve is forward
	EMACS__K_FETCH_CHARACTER,	// Fetch character at a given spot
	EMACS__K_RETURN_STRING_TO_PARAM,// The given parameter is set to a string
	EMACS__K_RETURN_NUMBER_TO_PARAM,// The given parameter is set to a number
	EMACS__K_BUFFER_EXTENT,		// Returns the buffer extent
	EMACS__K_GAP_TO,		// Move the buffer gap
	EMACS__K_GAP_ROOM,		// Make the gap bigger and BETTER
	EMACS__K_ALLOCATE_STRING,	// Allocate a heap string buffer
	EMACS__K_REALLOCATE_STRING,	// Reallocate the memory of a heap string buffer
	EMACS__K_FREE_STRING		// Free the memory of a heap string buffer
	} EmacsCallBackFunctions;

enum { EMACS__K_MIN = 0 };
enum { EMACS__K_MAX = EMACS__K_GAP_ROOM };

typedef enum EmacsCallBackStatus
	{
	EMACS__K_SUCCESS = 0,
	EMACS__K_FAILURE
	} EmacsCallBackStatus;


//
//	Prototype of the function that users call back into emacs with
//
typedef EmacsCallBackStatus (*EmacsCallBackFunction)(EmacsCallBackFunctions,...);
//
//	Prototype of the function that emacs call the user functions with
//
typedef EmacsCallBackStatus (*EmacsUser_C_Function)( EmacsCallBackFunction call_back );


#ifdef __cplusplus
class EmacsCallBackInterface
	{
public:
	EmacsCallBackInterface( EmacsCallBackFunction _call_back )
		: call_back( _call_back )
		{ }
	virtual ~EmacsCallBackInterface()
		{ }

	//
	// Display a string as a message
	//
	EmacsCallBackStatus message( const unsigned char *message)
		{ return call_back( EMACS__K_MESSAGE, message ); }
	EmacsCallBackStatus message( const char *message)
		{ return call_back( EMACS__K_MESSAGE, message ); }
	//
	// Display a string as an error message
	//
	EmacsCallBackStatus error( const unsigned char *error_message)
		{ return call_back( EMACS__K_ERROR, error_message ); }
	EmacsCallBackStatus error( const char *error_message)
		{ return call_back( EMACS__K_ERROR, error_message ); }
	//
	// Re-displays and waits for n 1/10s sec
	//
	EmacsCallBackStatus sitFor( int period)
		{ return call_back( EMACS__K_SITFOR, period ); }
	//
	// Return the argument count
	//
	EmacsCallBackStatus argCount( int *num_args)
		{ return call_back( EMACS__K_ARG_COUNT, num_args ); }
	//
    	// Check the arguments, and return an error
	//
	EmacsCallBackStatus checkArgs( int min_args, int max_args )
		{ return call_back( EMACS__K_CHECK_ARGS, min_args, max_args ); }
	//
	// Return Nth arg as a string
	//
	EmacsCallBackStatus stringArg( int arg_num, unsigned char **string )
		{ return call_back( EMACS__K_STRING_ARG, arg_num, string ); }
	EmacsCallBackStatus stringArg( int arg_num, char **string )
		{ return call_back( EMACS__K_STRING_ARG, arg_num, string ); }
	//
	// Return Nth arg as an integer
	//
	EmacsCallBackStatus numericArg( int arg_num, int *number )
		{ return call_back( EMACS__K_NUMERIC_ARG, arg_num, number ); }
	//
	// Make the function return a string
	//
	EmacsCallBackStatus stringResult( const unsigned char *result_string )
		{ return call_back( EMACS__K_STRING_RESULT, result_string ); }
	EmacsCallBackStatus stringResult( const char *result_string )
		{ return call_back( EMACS__K_STRING_RESULT, result_string ); }
	//
	// Make the function return a number
	//
	EmacsCallBackStatus numericResult( int result_number)
		{ return call_back( EMACS__K_NUMERIC_RESULT, result_number ); }
	//
	// Returns the size of the buffer
	//
	EmacsCallBackStatus bufferSize( int *buffer_size )
		{ return call_back( EMACS__K_BUFFER_SIZE, buffer_size ); }
	//
	// Returns the components of dot
	//
	EmacsCallBackStatus dot( int *dot)
		{ return call_back( EMACS__K_DOT, dot ); }
	//
	// Returns the components of mark
	//
	EmacsCallBackStatus mark( int *position )
		{ return call_back( EMACS__K_MARK, position ); }
	//
	// Set the mark to where dot is
	// or if set_mark is 0 unset the mark
	//
	EmacsCallBackStatus setMark()
		{ return call_back( EMACS__K_SET_MARK, 1 ); }
	EmacsCallBackStatus unsetMark()
		{ return call_back( EMACS__K_SET_MARK, 0 ); }
	//
	// Temp use buffer
	//
	EmacsCallBackStatus useBuffer( const unsigned char *buffer_name )
		{ return call_back( EMACS__K_USE_BUFFER, buffer_name ); }
	EmacsCallBackStatus useBuffer( const char *buffer_name )
		{ return call_back( EMACS__K_USE_BUFFER, buffer_name ); }
	//
	// Return current Buffer name
	//
	EmacsCallBackStatus bufferName( unsigned char **buffer_name )
		{ return call_back( EMACS__K_BUFFER_NAME, buffer_name ); }
	EmacsCallBackStatus bufferName( char **buffer_name )
		{ return call_back( EMACS__K_BUFFER_NAME, buffer_name ); }
	//
	// Sets dot to the specified character
	//
	EmacsCallBackStatus gotoCharacter( int position )
		{ return call_back( EMACS__K_GOTO_CHARACTER, position ); }
	//
	// Inserts the character at dot
	//
	EmacsCallBackStatus insertCharacter( unsigned char ch )
		{ return call_back( EMACS__K_INSERT_CHARACTER, ch ); }
	EmacsCallBackStatus insertCharacter( char ch )
		{ return call_back( EMACS__K_INSERT_CHARACTER, ch ); }
	//
	// Inserts a string at dot
	//
	EmacsCallBackStatus insertString( const unsigned char *string )
		{ return call_back( EMACS__K_INSERT_STRING, string ); }
	EmacsCallBackStatus insertString( const char *string )
		{ return call_back( EMACS__K_INSERT_STRING, string ); }
	//
	// Delete character, +ve is forward
	//
	EmacsCallBackStatus deleteCharacters( int num_chars)
		{ return call_back( EMACS__K_DELETE_CHARACTERS, num_chars ); }
	//
	// Fetch character at a given spot
	//
	EmacsCallBackStatus fetchCharacter( int position, int *ch )
		{ return call_back( EMACS__K_FETCH_CHARACTER, position, ch ); }
	//
	// The given parameter is set to a string
	//
	EmacsCallBackStatus returnStringToParam( int arg_num, const unsigned char *string )
		{ return call_back( EMACS__K_RETURN_STRING_TO_PARAM, arg_num, string ); }
	EmacsCallBackStatus returnStringToParam( int arg_num, const char *string )
		{ return call_back( EMACS__K_RETURN_STRING_TO_PARAM, arg_num, string ); }
	//
	// The given parameter is set to a number
	//
	EmacsCallBackStatus returnNumberToParam( int arg_num, int number)
		{ return call_back( EMACS__K_RETURN_NUMBER_TO_PARAM, arg_num, number ); }
	//
	// Returns the buffer extent
	//
	EmacsCallBackStatus bufferExtent( int *size_part1, unsigned char *part1, int *size_part2, unsigned char *part2 )
		{ return call_back( EMACS__K_BUFFER_EXTENT, size_part1, part1, size_part2, part2 ); }
	//
	// Move the buffer gap
	//
	EmacsCallBackStatus gapTo( int position )
		{ return call_back( EMACS__K_GAP_TO, position ); }
	//
	// Make the gap bigger and BETTER
	//
	EmacsCallBackStatus gapRoom( int size )
		{ return call_back( EMACS__K_GAP_ROOM, size ); }
	//
	// Allocate a heap string buffer
	//
	EmacsCallBackStatus allocateString( int size, unsigned char **new_string )
		{ return call_back( EMACS__K_ALLOCATE_STRING, size, new_string ); }
	EmacsCallBackStatus allocateString( int size, char **new_string )
		{ return call_back( EMACS__K_ALLOCATE_STRING, size, new_string ); }
	//
	// Reallocate the memory of a heap string buffer
	//
	EmacsCallBackStatus reallocateString( unsigned char *old_string, int new_size, unsigned char **new_string )
		{ return call_back( EMACS__K_REALLOCATE_STRING, old_string, new_size, new_string); }
	EmacsCallBackStatus reallocateString( char *old_string, int new_size, char **new_string )
		{ return call_back( EMACS__K_REALLOCATE_STRING, old_string, new_size, new_string); }
	//
	// Free the memory of a heap string buffer
	//
	EmacsCallBackStatus freeString( unsigned char **old_string )
		{ return call_back( EMACS__K_FREE_STRING, old_string ); }
	EmacsCallBackStatus freeString( char **old_string )
		{ return call_back( EMACS__K_FREE_STRING, old_string ); }
	
private:
	EmacsCallBackFunction call_back;
	};


//
//	Prototype of the function that emacs call the user functions with
//
typedef EmacsCallBackStatus (*EmacsUser_CXX_Function)( EmacsCallBackInterface &call_back );

#endif

