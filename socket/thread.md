스레드란
POSIX thread와 OpenMP 방식이 많이 사용되는  스레드 기법

POSIX thread는 줄여서 pthread 라고 부름



레이턴시: I/O 요청 이후 실제 응답이 오기 전에 대기하는 시간을 바로 레이턴시라고 부름. 그래서 CPU와 입출력하는 I/O장치의 응답시간 차이가 클수록 레이턴시는 커진다.



→ 멀티 스레드가 도입되면 CPU 개수 < 스레드 개수 인 상황에서 레이턴시가 발생하는 상황에서 CPU는 대기하지 않고 다른 스레드 작업을 할 수 있다.



→ 다른 작업에 의해서 레이턴시가 숨겨지는 것 처럼 보이기 때문에 레이턴시 하이딩이라고 부름. 

결국엔 네트워크나 디스크 입출력과 같이 레이턴시가 많이 발생하는 작업일 수록 멀티 스레딩의 효과가 높아진다. 



그런데 CPU 사용률이 포화 상태이거나 thrashing에 가까울 정도로 메모리 교체가 잦은 경우면 오버헤드가 발생해서 더 느려질 수도 있다.



멀티 스레드 vs 멀티 프로세스


멀티 프로세스를 사용하면, 메모리 영역이 보호되는 장점이 있고, 동기적 프로그래밍 모델(무슨말인지..)을 사용하므로 디버깅의 편리함 같은 장점이 있다.

하지만 IPC를 사용해야 하므로 I/O 대역폭을 소모하게 되어서 일정 성능 이상을 올리는게 쉽지 않다. 

멀티 스레드를 사용하면, 메모리를 공유하게 되므로 IPC를 사용하지 않고 주소만 넘겨주면 된다.  이런 방식은 TLB hit rate를 높은 수준으로 유지 할수 있기 때문에 메모리 중점저긍로

사용하는 시스템에서 유리하다.  

결국엔 I/O 처리량이 높거나 빈도수가 높으면 멀티 스레드가 성능상에 좋다.

``` cpp
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_THREADS 5
#define pr_err printf
#define pr_out printf

struct thread_arg {
    pthread_t tid;
    int idx;
}*t_arg;

void* start_thread(void*);
void clean_thread(struct thread_arg*);

int main()
{
    int i, ret;

    t_arg = (struct thread_arg*)calloc(NUM_THREADS, sizeof(struct thread_arg));

    for(i = 0; i<NUM_THREADS; i++) {
        t_arg[i].idx = i;
       if((ret =  pthread_create(&t_arg[i].tid, NULL, start_thread, (void*)&t_arg[i]))){
           pr_err("thread_create: %ld\n", t_arg[i].tid);
           return 0;
       }
       pr_out("pthread_crate: tid = %lu\n", t_arg[i].tid);
    }

    clean_thread(t_arg);

    return 0;
}


void *start_thread(void* arg)
{

    struct thread_arg *t_arg = (struct thread_arg*) arg;
    sleep(2);
    printf("\t Hello I'm pthred(%d) - TID(%lu)\n", t_arg->idx, t_arg->tid);
    t_arg->idx += 10;
    pthread_exit(t_arg);
}


void clean_thread(struct thread_arg* t_arg)
{

    int i; struct thread_arg* t_arg_ret;

    for(i = 0; i<NUM_THREADS; i++, t_arg++){
    
        pthread_join(t_arg->tid, (void**)&t_arg_ret);
        pr_out("pthread_join: %d -%lu\n", t_arg->idx, t_arg->tid);
    }
}

```
