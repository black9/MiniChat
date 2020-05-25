//
// Created by 邵尧 on 2020/5/21.
//

#ifndef MINISOCKETSERVER_SERVER_H
#define MINISOCKETSERVER_SERVER_H

#include "config.h"

struct params{
    int sockFd;
    int id;
};

int occupied[MAX_CLIENT_NUM];
struct sockaddr_in sa_client_list[MAX_CLIENT_NUM]; // sock addr of each connected client
SOCKET s_client_list[MAX_CLIENT_NUM]; // sock handle of each connected client

int next_client_id(int *occ);
void* sub_thread(void* params);
char* get_hello_msg(int id, char* msg);
int get_num_len(int a);

#endif //MINISOCKETSERVER_SERVER_H
