#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/ioctl.h>
#include	<errno.h>
#include	<fcntl.h>
#include	<stropts.h>	// stream options

// extern char	*ptsname(int);	// prototype not in any system header

int ptym_open(char *pts_name)
	{
	char	*ptr;
	int	fdm;

	strcpy(pts_name, "/dev/ptmx");	/* in case open fails */
	if ( (fdm = open(pts_name, O_RDWR|O_NONBLOCK)) < 0)
		return(-1);

	if (grantpt(fdm) < 0)
		{
		/* grant access to slave */
		close(fdm);
		return(-2);
		}
	if (unlockpt(fdm) < 0)
		{
		/* clear slave's lock flag */
		close(fdm);
		return(-3);
		}
	if ( (ptr = ptsname(fdm)) == NULL)
		{
		/* get slave's name */
		close(fdm);
		return(-4);
		}

	strcpy(pts_name, ptr);	/* return name of slave */
	return(fdm);			/* return fd of master */
	}

int ptys_open(int fdm, char *pts_name)
	{
	int	fds;

	/* following should allocate controlling terminal */
	if ( (fds = open(pts_name, O_RDWR|O_NONBLOCK)) < 0)
		{
		close(fdm);
		return(-5);
		}
	if (ioctl(fds, I_PUSH, "ptem") < 0)
		{
		close(fdm);
		close(fds);
		return(-6);
		}
	if (ioctl(fds, I_PUSH, "ldterm") < 0)
		{
		close(fdm);
		close(fds);
		return(-7);
		}

//	if (ioctl(fds, I_PUSH, "ttcompat") < 0)
//		{
//		close(fdm);
//		close(fds);
//		return(-8);
//		}

	return(fds);
	}
