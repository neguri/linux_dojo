#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

#define BUFF (100 * 1024 * 1024)
#define CYCLE 10
#define PG_SZ 4096


int main(void)
{
    char* p;
    time_t t;
    char* s;

    t = time(NULL);
    s = ctime(&t);
    printf("%.*s: before allocation, please press Enter key\n",(int)(strlen(s)-1),s);
    getchar();

    p = malloc(BUFF);
    if(p == NULL) err(EXIT_FAILURE, "malloc() failed");

    t = time(NULL);
    s = ctime(&t);
    printf("%.*s: allocated %dMB, please press Enter key\n", (int)(strlen(s)-1), s, BUFF/(1024*1024));

    getchar();

    int i ;
    for(i = 0; i<BUFF; i+=PG_SZ) {
        p[i] = 0;
        int cycle = i / (BUFF/CYCLE);
        if(cycle != 0 && i %(BUFF/CYCLE) == 0) {
            t = time(NULL);
            s = ctime(&t);
            printf("%.*s: touched %dMB\n",(int)(strlen(s)-1), s, i/(1024*1024));
            sleep(1);
        }
    }

    t = time(NULL);
    s = ctime(&t);
    printf("%.*s: touched %dMB, please press Enter key\n", (int)(strlen(s)-1), s, BUFF/(1024*1024));
    getchar();
    exit(EXIT_SUCCESS);
}
