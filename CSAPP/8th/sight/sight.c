#include"/home/xiaoyu/sophomore_1st/CS:APP/csapp.h"

void sigint_handler(int sig)
{
    printf("\nCaught SIGHT!\n");
    exit(0);
}

int main(void)
{
    if (signal(SIGINT, sigint_handler) == SIG_ERR)
        unix_error("signal error");

    Pause();

    return 0;
}