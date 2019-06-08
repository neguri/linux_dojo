#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#define BUFF 1000
#define ALLOC_SZ (100*1024*1024)

static char command[BUFF];
static char file_contents[BUFF];
static char overwrite_data[] = "HELLO";

int main(void)
{
    pid_t pid;
    pid = getpid();

    snprintf(command, BUFF, "cat /proc/%d/maps",pid);

    puts("*** memory map before mapping file ***");

    fflush(stdout);
    system(command);

    int fd;
    fd = open("testfile", O_RDWR);

    if(fd == -1) err(EXIT_FAILURE, "open() failed");

    char* file_contents;
    file_contents = mmap(NULL, ALLOC_SZ , PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0);

    if(file_contents == (void*) -1) {
        warn("mmap() failed");
        goto close_file;
    }


    puts("");
    printf("*** succeeded to map file: address = %p; size = %d ***\n", file_contents, ALLOC_SZ);

    puts("*** memory map after mapping file ***");

    fflush(stdout);
    system(command);


    puts("");
    printf("*** file contexts beofre overwrite mapped region: %s", file_contents);

    memcpy(file_contents, overwrite_data, strlen(overwrite_data));
    puts("");
    printf("*** overwritten mapped regionwith: %s\n", file_contents);

    if(munmap(file_contents, ALLOC_SZ) == -1) warn("mnummap() failed");

close_file:
    if(close(fd) == -1) warn("close() failed");
    exit(EXIT_SUCCESS);
}



