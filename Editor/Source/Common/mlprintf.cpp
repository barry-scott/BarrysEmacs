/*	Copyright (c) 1982-1993
		Barry A. Scott and nick Emery */

/*
 *	doprint.c
 *
 *	Barry A. Scott		 8-Aug-1984
 */
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


static void print_decimal( EmacsString &, long int, int );
static void print_hexadecimal( EmacsString &, unsigned long int, int );
static void print_octal( EmacsString &, long int, int );

/*
	The only format characters used by emacs are
		%s	- asciz string
		%d	- decimal number
		%x	- hexadecimal
		%o	- octal
		%-ns	- fixed field width string

	(sprintf "fmt" args...)

 */
int sprintf_cmd(void)
	{
	EmacsString output;
	unsigned char	ch;

	int	width, precision, left_justify;
	
	int arg = 1;

	/* need at least one arg */
	check_args( 1, 0 );

	if( !string_arg( arg++ ) )
		return 0;

	EmacsString format = ml_value.asString();

	for( int p=0; p<format.length(); )
		{
		int explicit_width = 0;

		ch = format[p++];

		if( ch != '%' )
			{
			output.append( ch );
			continue;
			}

		width = 0; precision = 0; left_justify = 0;
		ch = format[p++];

		if( ch == '-' )
			{
			ch = format[p++];
			left_justify = 1;
			}

		if( ch == '*' )
		    {
		    explicit_width = 1;
		    width = numeric_arg( arg++ );
		    if( ml_err )
			return 0;
		    ch = format[p++];
		    }
		else
		    while( ch >= '0' && ch <= '9' )
			{
			width = width*10 + ch - '0';
			ch = format[p++];
			}

		if( ch == '.' )
			{
			ch = format[p++];
			if( ch == '*' )
			    {
			    precision = numeric_arg( arg++ );
			    ch = format[p++];
			    }
			else
			    while( ch >= '0' && ch <= '9' )
				{
				precision = precision*10 + ch - '0';
				ch = format[p++];
				}
			}
		else
			precision = 9999;

		switch( ch )
		{
		case '%':
			output.append( '%' );
			continue;
		case 'c':
			ch = (char) numeric_arg( arg++ );
			if( ml_err ) return 0;
			if( !explicit_width )
				width = 1;
			while( width > 0 )
				{
				output.append( ch );
				width--;
				}
			continue;

		case 's':
			{
			if( !string_arg( arg++ ) )
				return 0;

			EmacsString s = ml_value.asString();

			if( !left_justify )
				{
				int str_len = s.length();

				for( ; str_len < width; width-- )
					output.append( ' ' );
				}

			int len = s.length();
			if( len > precision )
				len = precision;
			output.append( s( 0, precision ) );
			width -= len;
			while( width-- > 0 )
				output.append( ' ' );
			continue;
			}

		case 'd':
			{
			long val = (long)numeric_arg( arg++ );
			if( ml_err ) return 0;

			print_decimal( output, val, width );
			continue;
			}

		case 'o':
			{
			long val = (long)numeric_arg( arg++ );
			if( ml_err ) return 0;

			print_octal( output, val, width );
			continue;
			}

		case 'x':
			{
			unsigned long int val = (long)numeric_arg( arg++ );
			if( ml_err ) return 0;

			print_hexadecimal( output, val, width );
			continue;
			}
		default:
			error( FormatString("Unknown format character %c") << ch );
			return 0;
			}
		}

	ml_value = output;

	return 0;
	}

//
//	print a number "n" in decimal into buffer "buf"
//
static void print_decimal( EmacsString &output, long int n, int w )
	{
	unsigned char digits[12];
	int i, used;

	if( (unsigned long int)n == 0x80000000 )
		{
		output.append( "-2147483648" );
		}

	used = 0;
	if( n < 0 )
		{
		output.append( '-' );
		n = -n;
		used++;
		}

	i = 0;
	do
		{
		digits[i] = (char) ((n % 10) + '0');
		n = n / 10;
		i++;
		}
	while( n );

	if( w && w > i )
		{
		used = w;
		for(; w > i; w-- )
			output.append( ' ' );
		}
	else
		used += i;

	while( i-- )
		output.append( digits[i] );
	}

static void print_hexadecimal( EmacsString &output, unsigned long int n, int w )
	{
	int	i;
	char digits[8];

	for( i=0; n != 0; i++)
		{
		digits[i] = (unsigned char)
			((n&0xf) >= 10 ? (n&0xf) + 'a' - 10 : (n&0xf) + '0');
		n >>= 4;
		}

	while( w > i )
		{
		output.append( ' ' );
		w--;
		}
	while( i )
		{
		i--;
		output.append( digits[i] );
		}
	}

/*
	print a number "n" in octal into buffer "buf"
 */
static void print_octal
	(
	EmacsString &output,
	long int n,
	int w
	)
	{
	unsigned char digits[12];
	int	i;

	if( (unsigned long int)n == 0x80000000 )
		{
		output.append( "-20000000000" );
		}

	if( n < 0 )
		{
		output.append( '-' );
		n = -n;
		}

	i = 0;
	do
		{
		digits[i] = (char) ((n % 8) + '0');
		n = n / 8;
		i++;
		}
	while( n );

	if( w != 0 && w > i )
		{
		for(; w > i; w-- )
			output.append( ' ' );
		}

	while( i-- )
		output.append( digits[i] );
	}

