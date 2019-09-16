#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>


#define LISTEN_BACKLOG 20
#define MAX_POOL       3
#define print_msg(io, msgtype, arg...) \
    flockfile(io); \
    fprintf(io, "["#msgtype"][%s/%s:%03d] ", __FILE__, __FUNCTION__, __LINE__); \
    fprintf(io, arg);\
    fputc('\n', io); \
    funlockfile(io);

#define pr_err(arg...) print_msg(stderr, ERR, arg)
#define pr_out(arg...) print_msg(stdout, ERR, arg)

int fd_listener;

void start_child(int fd, int idx);

int main(int argc, char* argv[])
{
    int i;
    short port;
    socklen_t len_saddr;
    struct sockaddr_in saddr_s = {};
    pid_t pid;

    if(argc > 2) {
        printf("%s [port number]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(argc == 2) {
        port = (short) atoi((char*)argv[i]);
    }else{
        port = 0;
    }

    if( (fd_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) == -1) {
        pr_err("[TCP server] : Fail : socket()");
        exit(EXIT_FAILURE);
    }

    saddr_s.sin_family = AF_INET;
    saddr_s.sin_addr.s_addr = INADDR_ANY;
    saddr_s.sin_port = htons(port);

    if(bind(fd_listener, (struct sockaddr*)&saddr_s, sizeof(saddr_s)) == -1) {
        pr_err("[TCP server] Fail: bind()");
        exit(EXIT_FAILURE);
    }
    
    if( port == 0) {
        len_saddr = sizeof(saddr_s);
        getsockname(fd_listener, (struct sockaddr*)&saddr_s, &len_saddr);
    }

    pr_out("[TCP Server] Port : #%d", ntohs(saddr_s.sin_port));
    listen(fd_listener, LISTEN_BACKLOG);

    for(i=0; i<MAX_POOL; i++) {
        switch(pid=fork()) {
            case 0: // child
                start_child(fd_listener, i);
                exit(EXIT_SUCCESS);
                break;
            case 1:
                pr_err("[TCP Server]: Fail: fork()");
                break;
            default:
                pr_out("[TCP Server] Making child process No.%d",i);
        }
    }
    for(;;) pause();
    return 0;
}

void start_child(int sfd, int idx)
{

}




