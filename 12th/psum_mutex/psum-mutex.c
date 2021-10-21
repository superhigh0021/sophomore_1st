#include "/home/xiaoyu/sophomore_1st/csapp.h"
#define MAXTHREADS 32

void* sum_mutex(void* argp);

long gsum = 0;
long psum[MAXTHREADS];
long nelems_per_thread;
sem_t mutex;

int main(int argc, char** argv)
{
    long i, nelems, log_nelems, nthreads, myid[MAXTHREADS];
    pthread_t tid[MAXTHREADS];

    if (argc != 3) {
        printf("Usage:%s <nthreads> <log_nelems>\n", argv[0]);
        exit(0);
    }
    nthreads = atoi(argv[1]);
    log_nelems = atoi(argv[2]);
    nelems = (1L << log_nelems);
    nelems_per_thread = nelems / nthreads;
    sem_init(&mutex, 0, 1);

    for (i = 0; i < nthreads; ++i) {
        myid[i] = i;
        Pthread_create(&tid[i], NULL, sum_mutex, &myid[i]);
    }
    for (i = 0; i < nthreads; ++i)
        Pthread_join(tid[i], NULL);

    for(i=0;i<nthreads;++i)
        gsum+=psum[i];
    if (gsum != (nelems * (nelems - 1)) / 2)
        printf("Error: result=%ld\n", gsum);

    exit(0);
}

void* sum_mutex(void* vargp)
{
    long myid = *((long*)vargp);
    long start=myid*nelems_per_thread;
    long end=start+nelems_per_thread;
    long i,sum=0;

    for(i=start;i<end;++i)
        sum+=i;
    psum[myid]=sum;
    return NULL;
}