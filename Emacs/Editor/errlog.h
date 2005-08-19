//
// copyright 1994-1998 Barry A. Scott
//

class ErrorBlock : public EmacsObject
	{
public:
	ErrorBlock( EmacsBuffer *erb, int erp, EmacsBuffer *b, int p);
	~ErrorBlock();

	EMACS_OBJECT_FUNCTIONS( ErrorBlock )

	Marker e_mess;		// points to the error message
        Marker e_text;		// points to the next compiler error
        ErrorBlock *e_next;	// the next error in the chain
        ErrorBlock *e_prev;	// the next error in the chain
private:
	// prevent the compiler making the default class functions
	ErrorBlock();
	ErrorBlock( const ErrorBlock & );
	ErrorBlock &operator=( const ErrorBlock & );
	};

