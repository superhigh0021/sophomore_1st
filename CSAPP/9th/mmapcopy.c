#include"/home/xiaoyu/sophomore_1st/CSAPP/csapp.h"

void mmapcopy(int fd, int size){
    char *bufp;
    bufp = Mmap(NULL, size,PROT_READ,MAP_PRIVATE,fd,0);
    Write(1,bufp,size);
    return ;
}

int main(int argc,char** argv){
    struct stat stat;
    int fd;
    if(argc !=2){
        printf("usage: %s <filname>\n",argv[0]);
        exit(0);
    }

    fd=Open(argv[1],O_RDONLY,0);
    fstat(fd,&stat);
    mmapcopy(fd,stat.st_size);
    exit(0);
}
