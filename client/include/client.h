//
// Created by 邵尧 on 2020/5/21.
//

#ifndef MINISOCKETCLIENT_CLIENT_H
#define MINISOCKETCLIENT_CLIENT_H

#include "config.h"

int conn_status = 0;

struct params{
    int sockFd;
    int dest_id;
    char send_msg[BUF_SIZE];
};

void my_connect(int sockFd);
void *receive(void* fd);
void my_disconnect(int sockFd);
void get_time(int sockFd);
void get_name(int sockFd);
void get_client_list(int sockFd);
void send_msg(int sockFd, int dest_id, char* s_msg);
void my_exit(int sockFd);

#endif //MINISOCKETCLIENT_CLIENT_H
