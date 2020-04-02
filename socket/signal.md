좀비 프로세스란


프로세스는 두가지 정보로 이루어져 있는데, 데이터 + 메타 데이터임. 데이터는 말 그대로 실제 데이터인데, 메타 데이터는 파일 경로, 크기, 권한, 생성 시간등이다.

그래서 파일을 이동하거나 권한을 변경하면 메타 데이터가 변경되는 것이고 실제 데이터가 변경되는 것은 아니다.



좀비 프로세스는 프로세스가 가진 자원인 데이터와 메타 데이터를 모두 해제해야 하는데, 데이터만 해제되고 메타 데이터가 해제되지 않을때 발생한다.

메타 데이터는 PCB(process control block)로 불리게 되는데 . PCB에 정보가 저장되어 있음. 결국 PCB가 해제되지 않으면 좀비 프로세스가 되는 것임.



모든 프로세스는 데이타가 먼제 해제되고 메타 데이타가 해제 되므로 , 순간적으로 좀비 프로세스가 되는데, 이게 오래 지속되면 정말 좀비 프로세스가 됨. 



그럼 어떻게 좀비 프로세스를 없애느냐?

→ 부모 프로세스에서 wait, waitpid, waitid와 같은 시스템 콜은 해주면 된다. 다시 말해 프로세스가 종료될때, 부모 프로세스에서 PCB를 읽어 갈때 까지 리소스를 유지하게 된다.



만약 부모 프로세스에서 자식 프로세스의 상태를 알 필요가 없을 경우에는? 이런때는 wait, waitpid 를 호출할 필요 없이 SIGCHLD 을 무시하도록 시그널 핸들러를 설치해주면 된다.

sigaction의 sa_handler에서 SIG_IGN으로 설정한 뒤에 시그널 핸들러를 설치해준다.



그런데, 부모 프로세스가 먼저 죽으면? 자식 프로세스는 init process가 대신 리소스를 해제해 준다.

``` cpp

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
 
void sa_handler_chld(int signum);
 
int main()
{
 
    int ret;
    struct sigaction sa_chld;
    memset(&sa_chld, 0, sizeof(sigaction));
    sa_chld.sa_handler = sa_handler_chld;
    sigfillset(&sa_chld.sa_mask);
    //sa_chld.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa_chld, NULL);
    printf("[MAIN] SIGNAL handler installed\n");
 
    switch((ret = fork())){
        case 0:
            pause();
            exit(EXIT_SUCCESS);
        case -1:
            break;
        default:
            printf("[MAIN] parent pid = %d\n", getpid());
            printf("- Child pid = %d\n", ret);
 
            break;
   }
 
   while(1) {
       pause();
       printf("[MAIN] Recv SIGNAL ...\n");
   }
 
    return EXIT_SUCCESS;
}
 
void sa_handler_chld(int signaum)
{
 
    pid_t pid_child;
    int status;
    printf("[SIGNAL] Recv SIGCHLD signal\n");
    while(1) {
 
        if((pid_child = waitpid(-1, &status, WNOHANG))>0) {
            printf("\t-Exit child PID(%d)\n", pid_child);
        }else{
            break;
        }
    }
}
```




wait() vs waitpid()
wait 은 waitpid의 축소 버젼이므로 waitpid만 알아두면 된다. man 페이지에 자세히 나와 있는데, 자식 프로세스가 종료될때 까지 대기한다고 한다.

WNOHANG( return immediately), WIFEXITED(return true if the child terminated normaly) 이런 값들이 세번째 인자에 들어감. 

waitpid 사용할때는 루프를 사용해야 한다. 그래야 좀비 프로세스가 생성되지 않는다. 그런데 루프 빠져나올때 waitpid가 non blocking으로 동작해야 하므로 WNOHANG옵션을 사용해야 한다. 



ctrl + c
ctrl + c는 SIGINT 인데, 프로세스 그룹에 전파시켜서 그룹내 속한 자식 프로세스까지 한번에 종료시킨다.


``` cpp
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
 
 
void sa_handler_chld(int signum);
 
int main()
{
    int ret;
    struct sigaction sa_chld;
    memset(&sa_chld, 0, sizeof(struct sigaction));
    sa_chld.sa_handler = sa_handler_chld;
    sigfillset(&sa_chld.sa_mask);
    sigaction(SIGCHLD, &sa_chld, NULL);
    printf("[MAIN] SIGNAL handler installed(sig_waitd), pid(%d)\n", getpid());
    switch((ret = fork())){
 
        case 0:
            pause();
            exit(EXIT_SUCCESS);
        case -1:
            break;
        default:
            printf("-Child pid = %d\n",ret);
    }
 
    while(1){
        pause();
        printf("[MAIN] Recv SIGNAL...\n");
    }
 
 
    return EXIT_SUCCESS;
}
 
 
 void sa_handler_chld(int signum)
{
    printf("[SIGNAL] Recv SIGCHLD signal\n");
    int optflags = WNOHANG | WEXITED | WSTOPPED | WCONTINUED;
    siginfo_t wsiginfo ={.si_pid = 0};
    char *str_status;
 
    while(1) {
 
        if(waitid(P_ALL, 0, &wsiginfo, optflags) == 0 && wsiginfo.si_pid != 0) {
 
            switch(wsiginfo.si_code){
 
                case CLD_EXITED:
                    str_status = "Exited";
                    break;
 
                case CLD_KILLED:
                    str_status = "Killed";
                    break;
 
                case CLD_DUMPED:
                    str_status = "Dumped";
                    break;
 
                case CLD_STOPPED:
                    str_status = "Stopped";
                    break;
 
                case CLD_CONTINUED:
                    str_status = "Continued";
                    break;
                default:
                    str_status = "si_code";
                    break;
            }
 
            printf("child pid(%d) %s(%d)\n", wsiginfo.si_pid, str_status, wsiginfo.si_status);
          }else{
              break;
          }
    }
}
```

프로세스 그룹


프로세스 그룹이란, fork를 통해서 생성되는 자식 프로세스들을 관리할 수 있도록 만들어진 그룹임.  프롬프트에서 명령을 내리는 경우에, 프로세서는 자시으니 PID와 동일한 PGID(process group id)를 가지는 프로세스 그룹을 생성한다.

→ 그런데 PGID는 어떻게 보는 것인지 모르겠음. /proc/pid/status 에서는 관련 내용을 찾을 수가 없음  


