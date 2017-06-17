#define _LARGEFILE64_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>

#define O_MAP           00000004
#define iter_max	1000
#define block_max	64
#define MIN(a,b)        (a < b ? a : b)
#define COUNT		102400
#define OP_MSG		((WRITE) ? "WRIT" : "READ")


int main(int argc, char **argv)
{
	int opt, flags = O_RDWR, ret = 0, dax_fd = -1;
	unsigned long long iter = 0, iter_count = iter_max;
	unsigned long long n_rdwr = 0;
	char pid[8], dax_file[] = "/mnt/dax/largefile";
	char *buf = 0;
	struct stat st;
	ssize_t bytes = 0;
	size_t block = block_max;
	off_t size;
	bool TIME = true, WRITE = false;
	struct timeval start, stop;
	struct timezone tz;

	sprintf(pid, "%d\n", getpid());	
	printf("pid   = %s", pid);	

	/*
	 * Options:
	 * m = toggle O_MAP
	 * i = iter
	 * b = read io size
	 * w = write or read
	 */
        while ((opt = getopt(argc, argv, "mi:b:w")) != -1) {

	        switch (opt) {
			case 'm':
				flags = O_RDWR | O_MAP;
				break;

			case 'i':
				iter_count = MIN(iter_max, (unsigned long long) atoll(optarg));
				break;
			case 'b':
				block = MIN(block_max, (unsigned long long) atoll(optarg));
				break;
			case 'w':
				WRITE = true;
				break;
			default: /* '?' */
				fprintf(stderr, "Usage: %s [-m <NONE>] [-i iter	<%d>] 	\
						[-b size (kilobytes) <%d>] [-w write/read]	\
					\n", argv[0], iter_max, block_max);
				exit(EXIT_FAILURE);
               }
	}

	printf("type  = %s\n", OP_MSG);
	printf("block = %lu kilobytes\n", block);
	printf("iter  = %llu (times %d)\n", iter_count, COUNT);
	printf("O_MAP = %s\n", ((flags & O_MAP) ? "yes" : "no"));
	block *= 1024;	/* convert to bytes */

	buf = (char*) malloc(block);
	if(!buf) {
		ret = -6;
		printf("malloc(%lu) failed.\n", block);
		goto done;
	}

	if(WRITE) {
		for(iter = 0; iter < block; ++iter)
			buf[iter] = 'x';
	}

	dax_fd = open(dax_file, flags);
	if(dax_fd == -1)
	{
		printf("failed to open dax file %s\n", dax_file);
		ret = -5;
		goto done;
	}
	stat(dax_file, &st);
	size = st.st_size;	/* bytes */
	iter = 0;
	
	printf("fsize = %0.2lf gigabytes\n", 
		(double)size/(1.0*1024*1024*1024));

	printf("===============================================\n");
	printf("INITIATE RANDOM %sING LARGEFILE\n", OP_MSG);

	if (TIME) {
		gettimeofday(&start, &tz);
	}
	while(iter < iter_count)
	{
		/*
		 * Run for an arbitrarily large number of times
		 * to stress the kernel indexes.
		 */
		while(n_rdwr < COUNT) 
		{
			ret = lseek64(dax_fd, block*(random()%(size/block)), SEEK_SET);
			if(ret == -1)
				goto close;
			if(WRITE)
				bytes = write(dax_fd, buf, block);
			else
				bytes = read(dax_fd, buf, block);
			assert(bytes == block);
			n_rdwr += 1;
		}
		iter += 1;
		n_rdwr = 0;
	}
	if (TIME)
	{
		gettimeofday(&stop, &tz);
		printf("TIME  = %lu secs\n",
				((stop.tv_sec*1000000 + stop.tv_usec) -
				 (start.tv_sec*1000000 + start.tv_usec))/
				1000000);
	}
close:
	printf("===============================================\n");
	close(dax_fd);
done:
	return ret;
}

