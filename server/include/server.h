//
// Remaked by Black9 on 2021/10/09

#ifndef MINISOCKETSERVER_SERVER_H
#define MINISOCKETSERVER_SERVER_H

#include "config.h"

struct params{
    int sockFd;
    int id;
};

int occupied[MAX_CLIENT_NUM];
struct sockaddr_in sa_client_list[MAX_CLIENT_NUM]; // 연결된 각 클라이언트의 소켓 주소
SOCKET s_client_list[MAX_CLIENT_NUM]; //연결된 각 클라이언트의 소켓 핸들링

int next_client_id(int *occ);
void* sub_thread(void* params);
char* get_hello_msg(int id, char* msg);
int get_num_len(int a);

#endif // 미니소켓서버_server_헤더
