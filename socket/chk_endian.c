// network 은 big endian 을 사용하게 되는데, 그래서 network byte order 라고 함
// local host는 little endian을 사용하므로, host byte order라고 부름

#include <stdio.h>
#include <arpa/inet.h>

union byte_long {
    long l;
    unsigned char c[4];
};

int main()
{

    union byte_long bl;
    bl.l = 1200000L;

    printf("%02x-%02x-%02x-%02x\n", bl.c[0],bl.c[1],bl.c[2],bl.c[3]);
    bl.l = htonl(bl.l);
    printf("%02x-%02x-%02x-%02x\n", bl.c[0],bl.c[1],bl.c[2],bl.c[3]);
    return 0;
}
