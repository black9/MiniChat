//
// Created by 邵尧 on 2020/5/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>
#include <windows.h>
#include <pthread.h>

#include "include/server.h"
#include "include/config.h"

int main(int argc, char* argv[]){
    // start winsock
    WSADATA wsaData;
    WORD wVersionRequested;
    SOCKET sListen;
    SOCKET sClient;
    struct sockaddr_in sa_server;

    wVersionRequested = MAKEWORD( 2, 2 );
    if ( WSAStartup( wVersionRequested, &wsaData ) != 0 )
    {
        perror("Error: WSA start failed\n");
        exit(EXIT_FAILURE);
    }

    if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
    {
        perror("Error: Invalid winsock version\n");
        WSACleanup( );
        exit(EXIT_FAILURE);
    }

    // initialize
    sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sListen == INVALID_SOCKET){
        perror("Error: Create_socket failed\n");
        exit(EXIT_FAILURE);
    }
    memset(occupied, 0, sizeof(occupied));

    printf("Initialize complete.\n");

    memset(&sa_server, 0, sizeof(sa_server));
    sa_server.sin_family = AF_INET;
    sa_server.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    sa_server.sin_port = htons(SERVER_PORT);

    //binding
    if (bind(sListen, (struct sockaddr *)&sa_server, sizeof(sa_server)) == SOCKET_ERROR)
    {
        perror("Error: Binding faild!\n");
        closesocket(sListen); //关闭套接字
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    printf("Binding complete.\n");

    // listen
    if (listen(sListen, WAIT_LIST_LEN) == SOCKET_ERROR)
    {
        perror("Error: Listen faild!\n");
        closesocket(sListen); //关闭套接字
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    printf("Start listening.\n");

    // accept
    int length = sizeof(sa_client_list[0]);
    while(1){
        int c_id = next_client_id(occupied);
        if (c_id == -1){
            continue;
        }
        sClient = accept(sListen, (struct sockaddr*)&sa_client_list[c_id], &length);
        s_client_list[c_id] = sClient;
        if (sClient == INVALID_SOCKET) {
            perror("Error: Accept faild!\n");
            closesocket(sClient); //关闭套接字
            continue;
        }
        printf("Server: New connection from ip:%s, port:%d, client id %d\n", inet_ntoa(sa_client_list[c_id].sin_addr), ntohs(sa_client_list[c_id].sin_port), c_id);

        occupied[c_id] = 1;
        // new thread
        pthread_t p_id;
        // init params
        struct params* p = (struct params *)malloc(sizeof(struct params));
        p->sockFd = sClient;
        p->id = c_id;
        if(pthread_create(&p_id , NULL , sub_thread, (void*)p) != 0)
        {
            perror("pthread create error.\n");
            closesocket(sClient); //关闭套接字
            occupied[c_id] = 0;
            continue;
        }
        // Todo: free(p)
    }

    return 0;
}

char* get_hello_msg(int id, char* msg){
    strcpy(msg, "Hello, Your id is \0");
    char tmp[5];
    itoa(id, tmp, 10);
    strcat(msg, tmp);
    return msg;
}


void* sub_thread(void* p){
    struct params* pp = (struct params*)p;
    SOCKET sockFd = pp->sockFd;
    int id = pp->id;

    // send hello message
    char hello_msg[BUF_SIZE] = "\0";
    get_hello_msg(id, hello_msg);
    if(send(sockFd, hello_msg, strlen(hello_msg), 0) == -1){
        printf("Error: Sending hello Msg failed.\n");
    }
    printf("Sending hello msg to client %d.\n", id);

    while(1)
    {
        char buf[BUF_SIZE];
        memset(buf , 0 , BUF_SIZE);
        int tail = recv(sockFd , buf , BUF_SIZE , 0);
        if( tail == -1)
        {
            printf("Client %d disconnected.\n", id);
            occupied[id] = 0;
            closesocket(sockFd);
            break;
        }
        // print client request
        buf[tail] = '\0';
        printf("Client%d: %s\n", id, buf);

        if(strcmp(buf, "get_time") == 0){
            //parse time
            time_t rawtime;
            struct tm * timeinfo;
            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            char msg[BUF_SIZE];
            strcpy(msg, asctime (timeinfo));
            //send time to client
            int res = send(sockFd, msg, strlen(msg), 0);
            if(res == -1){
                perror("Error: Sending time failed.\n");
            }
        }
        if(strcmp(buf, "get_name") == 0){
            const int buf_size = 50;
            char msg[buf_size];
            DWORD dwNameLen = buf_size;
            if(!GetComputerName(msg, &dwNameLen)) {
                perror("Error: Can't get computer name.\n");
            }else{
                strcat(msg, "\n");
                int res = send(sockFd, msg, strlen(msg), 0);
                if(res == -1){
                    perror("Error: Sending computer name failed.\n");
                }
            }
        }
        if(strcmp(buf, "get_client_list") == 0){
            int i;
            const int buf_size = MAX_CLIENT_NUM * 30;
            char msg[buf_size];
            strcpy(msg, "\n");
            strcat(msg, "\0");
            for(i=0; i< MAX_CLIENT_NUM; i++){
                if(occupied[i] == 1){
                    char tmp[5];
                    itoa(i, tmp, 10);
                    strcat(msg, tmp);
                    strcat(msg, " ");
                    strcat(msg, inet_ntoa(sa_client_list[i].sin_addr));
                    strcat(msg, " ");
                    itoa(ntohs(sa_client_list[i].sin_port), tmp, 10);
                    strcat(msg, tmp);
                    strcat(msg, "\n");
                }
            }
            int res = send(sockFd, msg, strlen(msg), 0);
            if(res == -1){
                perror("Error: Sending client list failed.\n");
            }
        }
        if (strlen(buf) >= 4 && strncmp(buf, "Send", 4) == 0){
            // printf("%s", buf);
            int id_dest;
            sscanf(buf+4,"%d", &id_dest);
            int offset = get_num_len(id_dest);
            if(occupied[id_dest] == 0){
                char msg[BUF_SIZE] = "No such client id";
                int res = send(sockFd, msg, strlen(msg), 0);
                if(res == -1){
                    perror("Error: Sending client list failed.\n");
                }
            }else{
                char msg[BUF_SIZE] = "Client";
                char tmp[5];
                itoa(id, tmp, 10);
                strcat(msg, tmp);
                strcat(msg, ": ");
                strcat(msg, buf+4+offset);
                int res = send(s_client_list[id_dest], msg, strlen(msg), 0);
                if(res == -1){
                    perror("Error: Sending client list failed.\n");
                }
            }
        }
    }
    return 0;
}


int next_client_id(int *occ){
    int i;
    int res = -1;
    for(i=0; i<MAX_CLIENT_NUM; i++){
        if(occ[i] == 0){
            res = i;
            break;
        }
    }
    return res;
}

int get_num_len(int a){
    int len = 1;
    while(a / 10 != 0){
        len++;
        a /= 10;
    }
    return len;
}
