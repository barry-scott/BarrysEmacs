#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


int main( int argc, char **argv )
	{
	close(0);
	if(open(argv[1], 0) != 0) {
	    write(1, "Couldn't open input file\n", 25);
	    _exit(-1);
	}
	execvp(argv[2], &argv[2]);
	write(1, "Couldn't execute the program!\n", 30);
	_exit(-1);
	}
