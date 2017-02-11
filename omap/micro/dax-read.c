#define _LARGEFILE64_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define O_MAP           00000004
#define iter_max	1000
#define block_max	64
#define MIN(a,b)        (a < b ? a : b)
#define COUNT		102400


int main(int argc, char **argv)
{
	int opt, flags = O_RDWR, ret = 0, dax_fd = -1;
	unsigned long long iter = 0, iter_count = iter_max;
	unsigned long long n_rd = 0;
	char pid[8], dax_file[] = "/mnt/dax/largefile";
	char *buf = 0;
	struct stat st;
	ssize_t bytes = 0;
	size_t block = block_max;
	off_t size;

	sprintf(pid, "%lu\n", getpid());	
	printf("pid   = %s", pid);	

	/*
	 * Options:
	 * m = toggle O_MAP
	 * i = iter
	 * b = read io size
	 */
        while ((opt = getopt(argc, argv, "mi:b:")) != -1) {

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
			default: /* '?' */
				fprintf(stderr, "Usage: %s [-m <NONE>] [-i iter			\
					<%llu>] [-b size (kilobytes) <%llu>] \n", argv[0],	\
					iter_max, block_max);
				exit(EXIT_FAILURE);
               }
	}

	printf("O_MAP = %s\n", ((flags & O_MAP) ? "yes" : "no"));
	printf("iter  = %llu\n", iter_count);
	printf("block = %llu kilobytes\n", block);
	block *= 1024;	/* convert to bytes */

	buf = (char*) malloc(block);
	if(!buf) {
		ret = -6;
		printf("malloc(%llu) failed.\n", block);
		goto done;
	}

	dax_fd = open(dax_file, flags);
	if(dax_fd == -1)
	{
		printf("failed to open dax file %s\n", dax_file);
		ret = -5;
		goto done;
	}
	stat(dax_file, &st);
	size = st.st_size;
	
	printf("filesize = %0.2lf gigabytes\n", 
		(double)size/(1.0*1024*1024*1024));
	printf("initiate random reading largefile\n");
	while(iter < iter_count)
	{
		/*
		 * Run for an arbitrarily large number of times
		 * to stress the kernel indexes.
		 */
		while(n_rd < COUNT) 
		{
			lseek64(dax_fd, random()%(size/block), SEEK_SET);
			bytes = read(dax_fd, buf, block);
			assert(bytes == block);
			n_rd += 1;
		}
		iter += 1;
		n_rd = 0;
	}

	printf("iter = %llu\n", iter);
	close(dax_fd);
done:
	return ret;
}

