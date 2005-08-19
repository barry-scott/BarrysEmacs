[toascii]
int toascii (character)						   [VAX-11 C]
#include ctype
char character;
/* Converts an 8-bit ASCII character to a 7-bit ASCII character */
[tolower]
char tolower (character)					   [VAX-11 C]
char character;
/* Changes an upper case letter into a lower case letter */
[toupper]
char toupper (character)					   [VAX-11 C]
char character;
/* Changes a lower case letter to upper case */
[tan]
double tan (x)							   [VAX-11 C]
#include math
double x;
/* Returns the tangent of its radian argument */
[tanh]
double tanh (x)							   [VAX-11 C]
#inlcude math
double x;
/* Returns the nyperbolic tangent ofits argument */
[sqrt]
double sqrt (x)							   [VAX-11 C]
#include math
double x;
/* returns the square root of its argument */
[setuid]
int setuid (member_number)					   [VAX-11 C]
unsigned member_number;
/* Compatibility only - no operation */
[setgid]
int setgid (group_number)					   [VAX-11 C]
unsigned group_number;
/* Compatibility only - no operation */
[scanf]
int scanf (fmt, dst1,...)					   [VAX-11 C]
char *fmt;
/* Performs formatted input from STDIN */
[fscanf]
int fscanf (file_pointer, fmt, dst1,...)			   [VAX-11 C]
#include stdio
FILE *file_pointer;
char *fmt;
/* Performs formatted input from the specified file */
[sscanf]
int scanf (buf, fmt, dst1,...)					   [VAX-11 C]
char *buf*fmt;
/* Performs formatted input from the specified string */
[rand]
int rand ()							   [VAX-11 C]
/* Returns a pseudo random number in the range 0 => (2 ** 31 - 1) */
[srand]
int srand (seed)						   [VAX-11 C]
int seed;
/* Re-initialise the pseudo random number generator */
[putc]
int putc (character, file_pointer				   [VAX-11 C]
#include stdio
char character;
FILE *file_pointer;
/* Writes a byte to a file */
[putchar]
int putchar (character)						   [VAX-11 C]
#include stdio
char character;
/* Writes a byte to STDOUT */
[fputc]
int fputc (character, file_pointer)				   [VAX-11 C]
#include stdio
FILE *file_pointer;
char character;
/* Write a byte to a file */
[putw]
int putw (integer, file_pointer)				   [VAX-11 C]
#include stdio
FILE *file_pointer;
int integer;
/* Writes a long word to a file */
[puts]
int puts (string)						   [VAX-11 C]
char *string;
/* Writes a string to STDOUT as a line */
[fputs]
int fputs (string, file_pointer)				   [VAX-11 C]
#include stdio
FILE *file_pointer;
char *string;
/* Writes a string as a line to the specified file */
[printf]
int printf (fmt, src1,...)					   [VAX-11 C]
char *fmt, *src1, ... *srcn;
/* Formats a string and sends it to STDOUT */
[sprintf]
int sprintf (buf, fmt, src1,...)				   [VAX-11 C]
char *buf, *fmt, *src1, ... *srcn;
/* Formats a string and stores the result in buf */
[fprintf]
int fprintf (fptr, fmt, src1,...)				   [VAX-11 C]
#include stdio
FILE *fptr;
char *fmt, *src1, ... *srcn;
/* Formats a string and sends it to the specified file */
[mktemp]
char *mktemp (template)						   [VAX-11 C]
char *template;
/* Creates a unique file name from the template supplied */
[log]
double log (x)							   [VAX-11 C]
#include math
double x;
/* Returns the natural log of its argument */
[log10]
double log10 (x)						   [VAX-11 C]
#include math
double x;
/* Returns the base-10 logarathm of its argument */
[longjump]
longjump (env, val)						   [VAX-11 C]
#include setjump
jmp_buf env;
int val;
/* Return to the specified environment with the specified return value */
[localtime]
struct tm *localtime (bintim)					   [VAX-11 C]
#include time
int *bintim;
/* Converts a time in seconds into hours, mins, secs etc. */
[isxdigit]
int isxdigit (character)					   [VAX-11 C]
#include stdio
char character;
/* Returns non-zero if the character is a hexadecimal digit */
[getc]
int getc (file_pointer)						   [VAX-11 C]
#include stdio
FILE *file_pointer;
/* fetch the next character from a file */
[getw]
int getw (file_pointer)						   [VAX-11 C]
#include stdio
FILE *file_pointer;
/* Returns the next four bytes as an integer from the specified file */
[getname]
char *getname (file_descriptor, buffer)				   [VAX-11 C]
int file_descriptor;
char *buffer;
/* Returns the fully expanded file name of the file descriptor */
[fgetname]
char *fgetname (file_pointer, buffer)				   [VAX-11 C]
FILE *file_pointer;
char *buffer;
/* Returns the fully expanded file name of the file pointer */
[geteuid]
unsigned geteuid ()						   [VAX-11 C]
/* Returns the user part of the current UIC */
[getuid]
unsigned getuid ()						   [VAX-11 C]
/* Returns the user part of the current UIC */
[fgetc]
int fgetc (file_pointer)					   [VAX-11 C]
FILE *file_pointer;
/* Fetches a byte from the designated file */
[getchar]
int getchar ()							   [VAX-11 C]
#include stdio
/* Fetches the next character from the standard input */
[getegid]
unsigned getegid ()						   [VAX-11 C]
/* Returns the group part of the current UIC */
[getgid]
unsigned getgid ()						   [VAX-11 C]
/* Returns the group part of the current UIC */
[fdopen]
FILE *fdopen (file_descriptor, type)				   [VAX-11 C]
int file_descriptor;
char *type;
/* create a file pointer from an open file descriptor */
[execl]
int execl (name, arg0, ..., 0)					   [VAX-11 C]
char *name, *arg0, ..., *argn;
/* Execute the specified image passing the arguments */
[execv]
int execv (name, argv)						   [VAX-11 C]
char *name, *argv [];
/* Execute the specified image passing the arguments */
[execle]
int execle (name, arg0, ..., 0, envp)				   [VAX-11 C]
char *name, *arg0, ..., *argn, *envp [];
/* Execute the specified image passing the arguments and environmenet */
[execve]
int execve (name, argv, envp)					   [VAX-11 C]
char *name, *argv [], *envp [];
/* Execute the specified image passing the arguments and environement */
[gcvt]
char *gcvt (value, ndigit, buf)					   [VAX-11 C]
double value;
int ndigit;
char *buf;
/* Converts a double to an ASCII string in any format */
[fcvt]
char *fcvt (value, ndigit, decpt, sign)				   [VAX-11 C]
double value;
int ndigit, *decpt, *sign;
/* Converts a double to an ASCII string in floating format */
[ecvt]
char *ecvt (value, ndigit, decpt, sign)				   [VAX-11 C]
double value;
int ndigit, *decpt, *sign;
/* Converts a double to an ASCII string in exp format */
[dup2]
int dup2 (file_descriptor_1, file_descriptor_2)			   [VAX-11 C]
int file_descriptor_1, file_dewscriptor_2;
/* make file_descriptor_2 point to the same file as file_descriptor_1 */
[close]
int close (file_descriptor)					   [VAX-11 C]
int file_descriptor;
/* Closes an open file */
[cfree]
int cfree (pointer)						   [VAX-11 C]
char *pointer;
/* Free some dynamic memory */
[abort]
abort ()							   [VAX-11 C]
/* abort the program with an illegal instruction */
[abs]
int abs (interger)						   [VAX-11 C]
int interger;
/* Returns the absolute value of an intenger */
[access]
int access (name, mode)						   [VAX-11 C]
char *name;
int mode;
/* Checks a file to see if a specified access mode is allowed.
[acos]
double acos(x)							   [VAX-11 C]
#include math
double x;
/* returns the arc cosine of a radian argument */
[alarm]
int alarm (seconds)						   [VAX-11 C]
unsigned seconds;
/* Sends a SIGALRM after the specified number of seconds */
[asin]
double asin (x)							   [VAX-11 C]
double x;
/* Returns the arc sine of the radian argument */
[atan]
double atan (x)							   [VAX-11 C]
double x;
/* Returns the arc tan of the radian argument */
[atan2]
double atan2 (x, y)						   [VAX-11 C]
double x,y;
/* Returns the arc tan of x/y */
[atof]
double atof (string)						   [VAX-11 C]
char *string;
/* Converts an ASCII string to a double */
[atoi]
int atoi (string)						   [VAX-11 C]
char *string;
/* Converts an ASCII string to an integer */
[atol]
long atol (string)						   [VAX-11 C]
char *string;
/* Converts an ASCII string to an integer */
[brk]
char *brk (addr)						   [VAX-11 C]
unsigned *addr;
/* Defines the break for the current image */
[cabs]
double cabs (z)							   [VAX-11 C]
struct { double x, y;} z;
/* Returns the square root of (x*x + y*y) */
[calloc]
char *calloc (size, nunber)					   [VAX-11 C]
unsigned size, number;
/* Allocates space for an area in memory */
[ceil]
double ceil (x)							   [VAX-11 C]
double x;
/* Returns the smallest integer => than its argument */
[chdir]
int chdir (name)						   [VAX-11 C]
char *name;
/* Changes the default directory */
[chmod]
int chmod (name, mode)						   [VAX-11 C]
char *name;
unsigned mode;
/* Changes a file's protection */
[chown]
int chown (name, owner, group)					   [VAX-11 C]
char *name;
unsigned owner,group;
/* Changes the ownership of a file */
[clearerr]
clearerr (file_pointer)						   [VAX-11 C]
#include stdio
FILE *file_pointer;
/* Resets the error and EOF indicators for a file */
[cos]
double cos (x)							   [VAX-11 C]
double x;
/* Returns the cosine of its radian argument */
[cosh]
double cosh (x)							   [VAX-11 C]
double x;
/* Returns the hyperbolic cosine of its argument */
[creat]
int creat (name, prot, options...)				   [VAX-11 C]
char *name, *options;
int prot;
/* Creates a file */
[create]
You probably meant to type creat!				   [VAX-11 C]
[ctermid]
char *ctermid (string)						   [VAX-11 C]
char *string;
/* Returns the name of the controlling terminal */
[cuserid]
char *cuserid (string)						   [VAX-11 C]
char *string;
/* Returns the name of the user who ran the image */
[delete]
int delete (name)						   [VAX-11 C]
char *name;
/* Deletes the specified file */
[modf]
double modf (value, iptr)					   [VAX-11 C]
double value,*iptr;
/* Returns the positive fractional part and assigns the integral part to iptr */
[dup]
int dup (fd)							   [VAX-11 C]
int fd;
/* Creates a new file descriptor for an existing file */
[exit]
exit (status)							   [VAX-11 C]
int status;
/* Terminate the image */
[exp]
double exp (x)							   [VAX-11 C]
#include math
double x;
/* returns the base e raised to the power of the argument */
[fabs]
double fabs (x)							   [VAX-11 C]
#include math
double x;
/* returns the absolute value of the argument */
[fclose]
int fclose (file_pointer)					   [VAX-11 C]
#include stdio
FILE *file_pointer;
/* Closes a file opened with fopen */
[feof]
int feof (file_pointer)						   [VAX-11 C]
#inlcude stdio
FILE *file_pointer;
/* Test for end of file */
[ferror]
int ferror (file_pointer)					   [VAX-11 C]
FILE *file_pointer;
/* Test for an error during a read or write */
[fflush]
int fflush (file_pointer)					   [VAX-11 C]
FILE *file_pointer;
/* Writes out any buffered information to the file */
[fgets]
char *fgets (string, max, fp)					   [VAX-11 C]
char *string;
int max;
FILE *fp;
/* Reads a line from a file */
[gets]
char *gets (string)						   [VAX-11 C]
char *string;
/* Reads a line from the standard input */
[fileno]
int fileno (file_pointer)					   [VAX-11 C]
FILE *file_pointer;
/* returns a file descriptor */
[fopen]
FILE *fopen (file_spec, access_mode)				   [VAX-11 C]
char *file_spec, *access_mode;
/* Open a file by file pointer */
[fread]
int fread (pointer, sizeof (*pointer), number_items, file_pointer) [VAX-11 C]
char *pointer;
int nitems;
FILE *file_pointer;
/* Reads a specified number of items from a file */
[free]
free (pointer)							   [VAX-11 C]
char *pointer;
/* Deallocate an area allocated by malloc, calloc or realloc */
[freopen]
FILE *freopen (file_spec, mode, fp)				   [VAX-11 C]
char *file_spec, *mode;
FILE *fp;
/* re-assigns the address of a file pointer and opens the file */
[frexp]
double frexp (value, eptr)					   [VAX-11 C]
double value;
int *eptr;
/* returns the mantissa of a double value */
[fseek]
int fseek (fp, offset, direction)				   [VAX-11 C]
FILE *fp;
int offset, direction;
/* Position to a specific byte in a file */
[ftell]
int ftell (file_pointer)					   [VAX-11 C]
FILE *file_pointer;
/* returns the byte offset of the current location is a file */
[fwrite]
int fwrite (pointer, sizeof (*pointer), number_items,		   [VAX-11 C]
    file_pointer)
char *pointer;
int nitems;
FILE *file_pointer;
/* Appends the specified number of items to the file */
[getenv]
char *getenv (name)						   [VAX-11 C]
char *name;
/* Return translation of an evironment variable or logical name */
[getpid]
int getpid()							   [VAX-11 C]
/* Returns the current Process ID */
[gsignal]
int gsignal(sig)						   [VAX-11 C]
int sig;
/* Raises the specified software signal */
[hypot]
double hypot (x, y)						   [VAX-11 C]
double x, y;
/* Retuens sqrt (x*x + y*y) */
[isalnum]
int isalnum (character)						   [VAX-11 C]
#icnlude ctype
char character;
/* Returns non zero if the character is alphanumeric */
[isalpha]
int isalpha (character)						   [VAX-11 C]
char character;
/* Returns non zero if the character is alphabetic */
[isascii]
int isascii (character)						   [VAX-11 C]
#include ctype
char character;
/* Returns non-zero if the character is ASCII */
[isatty]
int isatty (file_descriptor)					   [VAX-11 C]
int file_descriptor;
/* Returns 1 if the file descriptor is associated with a terminal */
[iscntrl]
int iscntrl (character)						   [VAX-11 C]
#include ctype
char character;
/* Returns non-zero if the character is a control character */
[isdigit]
int isdigit (character)						   [VAX-11 C]
#include ctype
char character;
/* Returns non-zero is the character is a digit */
[isgraph]
int isgraph (character)						   [VAX-11 C]
#include ctype
char character;
/* Returns non-zero if the character is "graphic" */
[islower]
int islower (character)						   [VAX-11 C]
#include ctype
char character;
/* Returns non-zero if the character is lower case */
[isprint]
int isprint (character)						   [VAX-11 C]
#include ctype
char character;
/* Returns non-zero if the character is an ASCII printing character */
[ispunct]
int ispunct (character)						   [VAX-11 C]
#inclued ctype
char character;
/* Returns non-zero if the character is punctuation */
[isspace]
int isspace (character)						   [VAX-11 C]
#icnlude ctype
char character;
/* Returns non-zero if the characater is a space HT VT CR FF or NL */
[isupper]
int isupper (character)						   [VAX-11 C]
#include ctype
char character;
/* Return non-zero if the character is upper case */
[kill]
int kill (pid, sig)						   [VAX-11 C]
int pid,sig;
/* Sends a signal to the specified process */
[ldexp]
double ldexp (value, exp)					   [VAX-11 C]
#include math
double value;
int exp;
/* Returns the value * 2 ** exp */
[lseek]
int lseek (fd, offset, direction)				   [VAX-11 C]
int offset, fd ,direction;
/* Positions to a byte offset within a file */
[malloc]
char *malloc (size)						   [VAX-11 C]
unsigned size;
/* Allocates an area of contiguous memory */
[nice]
int nice (increment)						   [VAX-11 C]
int increment;
/* Changes the process priority */
[open]
int open (name, mode)						   [VAX-11 C]
char *name;
int mode;
/* Opens an existing  file */
[pause]
int pause()							   [VAX-11 C]
/* Stops the process until it receives a signal */
[perror]
perror (string)							   [VAX-11 C]
char *string;
/* Writes message describing the last error encoutered */
[pipe]
int pipe (fdvec)						   [VAX-11 C]
int fdvec [2];
/* Creates a pipe for use after a vfork/exec */
[pow]
double pow (x, y)						   [VAX-11 C]
#include math
double x,y;
/* Returns x ** y */
[read]
int read (fd, buf, count)					   [VAX-11 C]
int fd, count;
char *buf;
/* Reads some bytes from a file */
[realloc]
char *realloc (pointer, size)					   [VAX-11 C]
char *pointer;
unsigned size;
/* Changes the size of an area of memory (produces a new address for it) */
[rewind]
int rewind (file_pointer)					   [VAX-11 C]
#include stdio
FILE *file_pointer;
/* Re-positions a file to its start */
[sbrk]
char *sbrk (incr)						   [VAX-11 C]
unsigned incr;
/* Changes the program break */
[setbuf]
setbuf (file_pointer, buffer)					   [VAX-11 C]
FILE *file_pointer;
char *buffer;
/* Associates a buffer with a file */
[setjmp]
setjmp (env)							   [VAX-11 C]
#include setjmp
jmp_buf env;
/* Saves the environment for later restoration with longjmp */
[signal]
int (*signal (sig, func))()					   [VAX-11 C]
int sig, (*func)();
/* Allows a program to trap a signal */
[sin]
double sin (x)							   [VAX-11 C]
#inlcude math
double x;
/* Returns the sine of the radian argument */
[sinh]
double sinh (x)							   [VAX-11 C]
#includemath
double x;
/* Returns the hyperbolic size of its argument */
[sleep]
sleep (seconds)							   [VAX-11 C]
unsigned seconds;
/* Pause the process for at least the specified time */
[ssignal]
int (* ssignal (sig, action)) ()				   [VAX-11 C]
int sig, (*action)();
/* Establishes the action to take when a signal is raised */
[strcat]
char *strcat (src, dst)						   [VAX-11 C]
char *src,*dst;
/* Concatenates two strings */
[strcatn]
You probably meant to type strncat				   [VAX-11 C]
[strchr]
char *strchr (string, character)				   [VAX-11 C]
char *string,character;
/* Returns a pointer to the given character is a string */
[strcmp]
int strcmp (string_1, string_2)					   [VAX-11 C]
char *string_1, *string_2;
/* Compares two strings */
[strcmpn]
You probably meant to type strncmp				   [VAX-11 C]
[strcpy]
char *strcpy (dst, src)						   [VAX-11 C]
char *dst, *src
/* Copies string_2 to string_1 */
[strcspn]
int strcspn (string, charset)					   [VAX-11 C]
char *string, *charset;
/* Searches for an occurrence of a character set in a string */
[strlen]
int strlen (string)						   [VAX-11 C]
char *string;
/* Returns the length of a string */
[strncat]
char *strncat (src, dst, max)					   [VAX-11 C]
char *src, *dst;
int max;
/* Concatenates src to dst for a maximum of max */
[strncmp]
int strncmp (string_1, string_2, max)				   [VAX-11 C]
char *string_1, *string_2;
int max;
/* Compares two counted strings */
[strncpy]
char *strncpy (dst, src, max)					   [VAX-11 C]
char *src, *dst;
int max;
/* Copies string_2 into string_1 to a maximum character count */
[strpbrk]
char *strpbrk (string, charset)					   [VAX-11 C]
char *string, *charset;
/* Searches for a character set in a string */
[strrchr]
char *strrchr (string, character)				   [VAX-11 C]
char *string, character;
/* searches for a character in a string */
[strspn]
int strspn (string, charset)					   [VAX-11 C]
char *string, *charset;
/* skips character is a character set in a string */
[time]
long time (time_location)					   [VAX-11 C]
long *time_location;
/* Returns the time elapsed since 00:00:00 Jan 1 1970 */
[times]
times (buf_pointer)						   [VAX-11 C]
struct time_buffer *buf_pointer;
/* Returns CPU time of current process and terminated children */
[tmpfile]
FILE *tmpfile ()						   [VAX-11 C]
/* Creates a temporary file and returns a file pointer to it */
[tmpnam]
char *tmpnam (name)						   [VAX-11 C]
char *name;
/* Creates a character string to be used as a temporary file */
[umask]
int umask (mode_complement)					   [VAX-11 C]
unsigned mode_complement;
/* Creates a file protection mask */
[vfork]
int vfork()							   [VAX-11 C]
/* Sets up environment for spawning a new process */
[wait]
int wait (status)						   [VAX-11 C]
int *status;
/* Stop process execution until a signal or a child terminates */
[write]
int write (fd, buf, count)					   [VAX-11 C]
int fd, count;
char *buf;
/* Write a number of bytes to a file */
