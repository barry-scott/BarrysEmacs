/*	Copyright (c) 1982-1993
		Barry A. Scott and nick Emery */

/*
 *	doprint.c
 *
 *	Barry A. Scott		 8-Aug-1984
 */
#include <emacs.h>

static int print_decimal( unsigned char *, long int, int );
static int print_hexadecimal( unsigned char *, unsigned long int, int );
static int print_octal( unsigned char *, long int, int );

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
	unsigned char format[1024];
	unsigned char output_buffer[1024];

	unsigned char *buf = output_buffer;
	int buf_len = sizeof( output_buffer );

	unsigned char	*p = format;
	unsigned char	ch;

	int	i;
	int	width, precision, left_justify;
	
	int arg = 1;

	/* need at least one arg */
	check_args( 1, 0 );

	if( !string_arg( arg++ ) )
		return 0;

	if( ml_value->exp_int >= buf_len )
		{
		error( u_str("Format string is too long") );
		return 0;
		}

	_str_cpy( format, ml_value->exp_v.v_string );

	buf_len--;	/* trim off space for the trailing NUL */
	i = buf_len;

	while( (ch = *p++) != 0 )
		{
		int explicit_width = 0;

		if( ch != '%' )
			{
			*buf++ = ch;
			if( --i == 0 ) goto buf_too_small;

			continue;
			}

		width = 0; precision = 0; left_justify = 0;
		ch = *p++;

		if( ch == '-' )
			{
			ch = *p++; left_justify = 1;
			}

		if( ch == '*' )
		    {
		    explicit_width = 1;
		    width = numeric_arg( arg++ );
		    if( err )
			return 0;
		    ch = *p++;
		    }
		else
		    while( ch >= '0' && ch <= '9' )
			{
			width = width*10 + ch - '0';
			ch = *p++;
			}

		if( ch == '.' )
			{
			ch = *p++;
			if( ch == '*' )
			    {
			    precision = numeric_arg( arg++ );
			    ch = *p++;
			    }
			else
			    while( ch >= '0' && ch <= '9' )
				{
				precision = precision*10 + ch - '0';
				ch = *p++;
				}
			}
		else
			precision = 9999;

		switch( ch )
		{
		case '%':
			if( --i == 0 ) goto buf_too_small;
			*buf++ = '%';
			continue;
		case 'c':
			ch = (char) numeric_arg( arg++ );
			if( err ) return 0;
			if( !explicit_width )
				width = 1;
			while( width > 0 )
				{
				if( --i == 0 ) goto buf_too_small;
				*buf++ = ch;
				width--;
				}
			continue;

		case 's':
			{
			unsigned char	*s;
			if( !string_arg( arg++ ) )
				return 0;
			s = ml_value->exp_v.v_string;

			if( s == NULL )
				s = u_str("");

			if( !left_justify )
				{
				int str_len = _str_len( s );

				for( ; str_len < width; width-- )
					{
					if( --i == 0 ) goto buf_too_small;
					*buf++ = ' ';
					}
				}

			while( (ch = *s++) != 0 && precision > 0 )
				{
				if( --i == 0 ) goto buf_too_small;
				*buf++ = ch;
				width--;
				precision--;
				}
			while( width-- > 0 )
				{
				if( --i == 0 ) goto buf_too_small;
				*buf++ = ' ';
				}
			continue;
			}

		case 'd':
			{
			int	used;
			long val = (long)numeric_arg( arg++ );
			if( err ) return 0;

			if( i < 12 ) goto buf_too_small;

			used = print_decimal( buf, val, width );

			buf += used;
			i -= used;
			continue;
			}

		case 'o':
			{
			int	used;
			long val = (long)numeric_arg( arg++ );
			if( err ) return 0;

			if( i < 12 ) goto buf_too_small;

			used = print_octal( buf, val, width );

			buf += used;
			i -= used;
			continue;
			}

		case 'x':
			{
			int	used;
			unsigned long int val = (long)numeric_arg( arg++ );
			if( err ) return 0;

			if( i < 8 ) goto buf_too_small;

			used = print_hexadecimal( buf, val, width );

			buf += used;
			i -= used;
			continue;
			}
		default:
			error(u_str("Unknown format character %c"), ch );
			return 0;
			}
		}

	/* terminate the string */
	*buf++ = '\0';
	i--;

	buf_len = buf_len - i - 1;

	release_expr (ml_value);
	ml_value->exp_type = ISSTRING;
	ml_value->exp_int = buf_len;
	ml_value->exp_release = 1;
	ml_value->exp_v.v_string = malloc_ustr( buf_len+1 );
	_str_cpy( ml_value->exp_v.v_string, output_buffer );

	return 0;
buf_too_small:
	error( u_str("sprintf - attempt to output more then %d characters"), sizeof( output_buffer )-1 );
	return 0;
	}

/*
	print a number "n" in decimal into buffer "buf"
 */
static int print_decimal( unsigned char *buf, long int n, int w )
	{
	unsigned char *p, digits[12];
	int i, used;

	if( n == 0x80000000 )
		{
		for( p=u_str( "-2147483648" ); (*buf++ = *p++) != 0;)
			;
		return 11;
		}

	used = 0;
	if( n < 0 )
		{
		*buf++ = '-';
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
		for(; w > i; w-- ) *buf++ = ' ';
		}
	else
		used += i;

	while( i-- )
		*buf++ = digits[i];

	return used;
	}

static int print_hexadecimal( unsigned char *buf, unsigned long int n, int w )
	{
	int	i, result;
	char digits[8];

	for( i=0; n != 0; i++)
		{
		digits[i] = (unsigned char)
			((n&0xf) >= 10 ? (n&0xf) + 'a' - 10 : (n&0xf) + '0');
		n >>= 4;
		}

	result = max( i, w );

	while( w > i )
		{
		*buf++ = ' ';
		w--;
		}
	while( i )
		{
		i--;
		*buf++ = digits[i];
		}

	return result;
	}

/*
	print a number "n" in octal into buffer "buf"
 */
static int print_octal
	(
	unsigned char *buf,
	long int n,
	int w
	)
	{
	unsigned char	*p, digits[12];
	int	i, used;

	if( n == 0x80000000 )
		{
		for( p=u_str( "-20000000000" ); (*buf++ = *p++) != 0;);
		return 11;
		}

	used = 0;
	if( n < 0 )
		{
		*buf++ = '-';
		n = -n;
		used++;
		}

	i = 0;
	do
		{
		digits[i] = (char) ((n % 8) + '0');
		n = n / 8;
		i++;
		}
	while( n );

	if( w && w > i )
		{
		used = w;
		for(; w > i; w-- ) *buf++ = ' ';
		}
	else
		used += i;

	while( i-- )
		*buf++ = digits[i];

	return used;
	}

