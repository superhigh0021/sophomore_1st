/*  epoll_create出来的是一个监视事件集合，ep_event是保存发生了的事件的文件描述符集合，
    event是监视对象的事件类型(触发条件)，需要使用epoll_ctl来针对epoll_create出来的例程进行增删改，
    其中就包括增加的时候附带有该事件的触发条件(第四个参数)
    */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/epoll.h>

#define BUF_SIZE 100
#define EPOLL_SIZE 50
void error_handling(char *buf);

int main(int argc, char **argv){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t addr_sz;
    int str_len, i;
    char buf[BUF_SIZE];

    struct epoll_event *ep_events;  //! 保存发生了的事件的文件描述符集合
    struct epoll_event event;   //! 监视对象的事件类型(比如发生什么的时候触发事件,触发条件)
    int epfd, event_cnt;

    if(argc != 2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error!");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error!");

    epfd = epoll_create(EPOLL_SIZE);    //! epoll的文件描述符
    ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    while(1){
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if(event_cnt == -1){
            puts("epoll_wait() error!");
            break;
        }

        for(i = 0; i < event_cnt; ++i){
            if(ep_events[i].data.fd == serv_sock){
                addr_sz = sizeof(clnt_addr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_sz);
                event.events = EPOLLIN; //! 此处还需要重新注册一遍
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
            }else{
                //! 客户端要求读取数据
                str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
                if(str_len == 0){
                    //! 收到EOF 所以没有read到内容
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                    close(ep_events[i].data.fd);
                    printf("closed client: %d \n", ep_events[i].data.fd);
                }else write(ep_events[i].data.fd, buf, str_len);
            }
        }
    }
    close(serv_sock);
    close(epfd);
    return 0;
}

void error_handling(char *buf){
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}