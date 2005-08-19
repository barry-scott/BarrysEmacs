/*	Copyright (c) 1982-1993
		Barry A. Scott and nick Emery */

/*
 *	doprint.c
 *
 *	Barry A. Scott		 8-Aug-1984
 */

static int print_decimal( unsigned char *, long int, int );
static void print_hexadecimal( unsigned char *, long int, int );
static int print_octal( unsigned char *, long int, int );

/*
	The only format characters used by emacs are
		%s	- asciz string
		%d	- decimal number
		%x	- hexadecimal
		%o	- octal
		%-ns	- fixed field width string

 */
int do_print
	(
	unsigned char *format,
	va_list *argp_p,
	unsigned char *buf,
	int buf_len
	)
	{
	unsigned char	*p = format;
	unsigned char	ch;
	int	i;
	int	width, precision, left_justify;
	va_list argp;
	
	/*
	 * We need to have a copy the va_list parameter
	 * becuase the va_arg macro will update the
	 * orginal
	 */
	if( argp_p != NULL )
		memcpy( &argp, argp_p, sizeof( argp ) );
	else
		memset( &argp, 0, sizeof( argp ) );
	buf_len--;	/* trim off space for the trailing NUL */
	i = buf_len;

	while( (ch = *p++) != 0 )
		{
		if( ch != '%' )
			{
			*buf++ = ch;
			if( --i == 0 ) return buf_len;

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
		    width = va_arg( argp, int );
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
			    precision = va_arg( argp, int );
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
			if( --i == 0 ) return buf_len;
			*buf++ = '%';
			continue;
		case 'c':
			if( --i == 0 ) return buf_len;
			*buf++ = (char) va_arg( argp, int );
			continue;

		case 's':
			{
			unsigned char	*s = va_arg( argp, unsigned char * );

			if( s == NULL )
				s = u_str("");

			if( !left_justify )
				{
				int str_len = _str_len( s );

				for( ; str_len < width; width-- )
					{
					if( --i == 0 ) return buf_len;
					*buf++ = ' ';
					}
				}

			while( (ch = *s++) != 0 && precision > 0 )
				{
				if( --i == 0 ) return buf_len;
				*buf++ = ch;
				width--;
				precision--;
				}
			while( width-- > 0 )
				{
				if( --i == 0 ) return buf_len;
				*buf++ = ' ';
				}
			continue;
			}

		case 'd':
			{
			int	used;
			long val = (long)va_arg( argp, int );

			if( i < 12 ) return buf_len - i;

			used = print_decimal( buf, val, width );

			buf += used;
			i -= used;
			continue;
			}

		case 'D':
			{
			int	used;
			long int val = va_arg( argp, long int );

			if( i < 12 ) return buf_len - i;

			used = print_decimal( buf, val, width );

			buf += used;
			i -= used;
			continue;
			}

		case 'o':
			{
			int	used;
			long val = (long)va_arg( argp, int );

			if( i < 12 ) return buf_len - i;

			used = print_octal( buf, val, width );

			buf += used;
			i -= used;
			continue;
			}

		case 'O':
			{
			int	used;
			long int val = va_arg( argp, long int );

			if( i < 12 ) return buf_len - i;

			used = print_octal( buf, val, width );

			buf += used;
			i -= used;
			continue;
			}

		case 'x':
			{
			long int val = (long)va_arg( argp, int );

			if( i < 4 ) return buf_len - i;

			print_hexadecimal( buf, val, 4 );

			buf += 4;
			i -= 4;
			continue;
			}
		case 'X':
			{
			long int val = va_arg( argp, long int );

			if( i < 8 ) return buf_len - i;

			print_hexadecimal( buf, val, 8 );

			buf += 8;
			i -= 8;
			continue;
			}
		case 'p':
			if( i < 9 ) return buf_len - i;
			{
			long int val = va_arg( argp, long int );
#ifdef	_MSDOS              
			print_hexadecimal( buf, val >> 16l, 4 );
			buf[4] = ':';
			print_hexadecimal( buf+5, val, 4 );
			buf += 9;
			i -= 9;
#else
			print_hexadecimal( buf, val, 8 );
			buf += 8;
			i -= 8;
#endif
			}
			continue;

		default:
			{
#if	defined(REAL_TTY)
			fprintf
			(
			stderr,
			"\r\ndo_print called with %c in format %s\r\n",
			ch, format
			);
#endif
			return 0;
			}
		}
		}

	return buf_len - i;
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

static void print_hexadecimal( unsigned char *buf, long int n, int w )
	{
	int	i;

	for( i=w-1; i >= 0; i--)
		{
		buf[i] = (unsigned char)
			((n&0xf) >= 10 ? (n&0xf) + 'a' - 10 : (n&0xf) + '0');
		n >>= 4;
		}
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
