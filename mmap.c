#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#define BUFF 1000
#define ALLOC_SZ (100*1024*1024)

static char command[BUFF];

int main(void)
{
    pid_t pid;
    pid = getpid();

    snprintf(command, BUFF, "cat /proc/%d/maps",pid);

    puts("*** memory map before memry allocation ***");

    fflush(stdout);
    system(command);

    void *new_memory;
    new_memory = mmap(NULL, ALLOC_SZ, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(new_memory == (void*)-1) err(EXIT_FAILURE, "mmap() failed");

    puts("");
    printf("*** succeeded to allocate memory: address = %p; size = 0x%x ***\n",new_memory, ALLOC_SZ);

    puts("");
    puts("*** memory map after memory allocation ***");
    fflush(stdout);
    system(command);

    if(munmap(new_memory, ALLOC_SZ) == -1) err(EXIT_FAILURE, "munmap() failed");
    exit(EXIT_SUCCESS);
}

