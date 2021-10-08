//
// Remaked by Black9 on 2021/10/09


#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <pthread.h>

#include "include/client.h"
#include "include/config.h"

int main(int argc, char* argv[]){
    // winsock 시작
    WSADATA wsaData;
    WORD wVersionRequested;

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
    int sockFd;
    int choice = 0;

    printf("*****MiniSocket Client*****\n");
    printf("1: connect \n2: disconnect\n3: get time\n4: get name\n5: get client list\n6: send message\n7: exit\nYour choice:");
    scanf("%d", &choice);
    while(1){
        switch (choice) {
            case 1:
            {
                if(conn_status == 0){
                    sockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                    if(sockFd == INVALID_SOCKET){
                        perror("Error: Create_socket failed\n");
                        exit(EXIT_FAILURE);
                    }

                    char ip[20];
                    int port;
                    printf("IP address:");
                    scanf("%s", ip);
                    printf("Port:");
                    scanf("%d", &port);
                    printf("\n");

                    struct sockaddr_in sa_server;
                    memset(&sa_server, 0, sizeof(sa_server));
                    sa_server.sin_family = AF_INET;
                    sa_server.sin_addr.S_un.S_addr = inet_addr(ip);
                    sa_server.sin_port = htons(port);

                    printf("Connecting...\n");
                    if(connect(sockFd, (struct sockaddr *)&sa_server, sizeof(sa_server)) == SOCKET_ERROR){
                        printf("Error: Connection establishing failed.\n");
                        break;
                    }
                    printf("Connected to %s:%d\n", inet_ntoa(sa_server.sin_addr), ntohs(sa_server.sin_port));
                    my_connect(sockFd);
                    conn_status = 1;
                }else{
                    printf("You are already connected.\n");
                }
                break;
            }
            case 2:
            {
                if(conn_status == 1){
                    my_disconnect(sockFd);
                    conn_status = 0;
                }else {
                    printf("You haven't connected to the server, please connect first.\n");
                }
                break;
            }
            case 3:
            {
                if(conn_status == 1){
                    get_time(sockFd);
                }else {
                    printf("You haven't connected to the server, please connect first.\n");
                }
                break;
            }
            case 4:
            {
                if(conn_status == 1){
                    get_name(sockFd);
                }else {
                    printf("You haven't connected to the server, please connect first.\n");
                }
                break;
            }
            case 5:
            {
                if(conn_status == 1){
                    get_client_list(sockFd);
                }else {
                    printf("You haven't connected to the server, please connect first.\n");
                }
                break;
            }
            case 6:
            {
                if(conn_status == 1){
                    int max_len = BUF_SIZE - 10;
                    printf("Please give the client id you want to chat to:");
                    int dest_id;
                    scanf("%d", &dest_id);
                    printf("Please write the massage and end by pressing the enter button(maximum massage length: %d):\n ", max_len);
                    char msg[max_len];
//                    fgets(msg, max_len, stdin);
                    getchar();
                    gets(msg);
                    strcat(msg, "\n");
                    if(strlen(msg) > max_len){
                        msg[max_len] = '\0';
                    }
                    send_msg(sockFd, dest_id, msg);
                }else {
                    printf("You haven't connected to the server, please connect first.\n");
                }
                break;
            }
            case 7:
            {
                if(conn_status == 0){
                    exit(0);
                }else{
                    my_exit(sockFd);
                    exit(0);
                }
            }
            default:
                printf("Invalid number!\n");
        }
        Sleep(100);
        printf("Your choice:");
        scanf("%d", &choice);
    }
    return 0;
}


void my_connect(int sockFd){
    pthread_t recv_tid;
    // 주소(&sockFd) 대신 sockFd 값을 사용해야 함
    if(pthread_create(&recv_tid , NULL , receive, (void*)sockFd) != 0)
    {
        perror("pthread create error.\n");
        exit(1);
    }
}

void *receive(void* fd){
    SOCKET sockFd = (SOCKET)fd;
    while(1)
    {
        if(conn_status == 0){
            break;
        }
        char buf[BUF_SIZE];
        memset(buf , 0 , BUF_SIZE);
        int tail = recv(sockFd , buf , BUF_SIZE , 0);
        if( tail == -1)
        {
//            perror("Error: recv error.\n");
//            exit(EXIT_FAILURE);
            break;
        }else{
            buf[tail] = '\0';
            if(buf[0] == 'C'){
                printf("\n");
                printf("%s", buf);
            }else if(strlen(buf) != 0) {

                printf("Server: %s\n", buf);
            }
        }
    }
    return 0;
}

void my_disconnect(int sockFd){
    closesocket(sockFd);
}

void get_time(int sockFd){
    char msg[10] = "get_time";
    send(sockFd, msg, strlen(msg), 0);
}

void get_name(int sockFd){
    char msg[10] = "get_name";
    send(sockFd, msg, strlen(msg), 0);
}

void get_client_list(int sockFd){
    char msg[20] = "get_client_list";
    send(sockFd, msg, strlen(msg), 0);
}

void send_msg(int sockFd, int dest_id, char* s_msg){
    char msg[BUF_SIZE] = "Send";
    char tmp[5] = "\0";
    itoa(dest_id, tmp, 10);
    strcat(msg, tmp);
    strcat(msg, ":");
    strcat(msg, s_msg);
    send(sockFd, msg, strlen(msg), 0);
}

void my_exit(int sockFd){
    my_disconnect(sockFd);
}




