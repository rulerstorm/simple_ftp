
#include "server_main.h"
#include "socket_tcp.h"

void remove_space(char *str);


int main(int argc, char const *argv[])
{
//----------------------connection----------------------

    char buf[5];

    int fd_client_udp = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    set_addr(&server_addr, SERVER_IP, SERVER_PORT);

    struct sockaddr_in client_addr;
    set_addr(&client_addr, (char*)argv[1], (unsigned short)atoi(argv[2]));

    if (bind(fd_client_udp, (struct sockaddr *)&client_addr,
        sizeof(struct sockaddr_in)) == -1)  ERR_EXIT("Bind!!!");

    // sendto(fd_client_udp,"0",1,0,(struct sockaddr*)&server_addr,sizeof(server_addr));

    // recvfrom(fd_client_udp, buf, 1, 0, NULL, NULL);
    // close(fd_client_udp);
// printf("UDP ok\n");

    int fd_client_ftp = socket_init_tcp(client_addr);
    if(fd_client_ftp == -1) ERR_EXIT("Socket!!!");

    if(listen(fd_client_ftp, 5)==-1) ERR_EXIT("Listen!!!");

    sendto(fd_client_udp,"0",1,0,(struct sockaddr*)&server_addr,sizeof(server_addr));
    recvfrom(fd_client_udp, buf, 1, 0, NULL, NULL);
    close(fd_client_udp);
printf("UDP ok\n");

    int fd_server = my_accept(fd_client_ftp, &server_addr);  //与服务器连接的fd
printf("TCP ok\n");
    // close(fd_client_ftp);

//----------------------request--------------------------------------

    char send_buff[128], recv_buff[1024], pwd[128], password[16];

    while(1)
    {
        memset(pwd, 0, 128);
        memset(send_buff, 0, 128);
        memset(password, 0, 16);

        recv(fd_server, pwd, 128, 0);
        system("clear");
        printf("+--------------------------------------------------+\n");
        printf("|  Simple FTP               2014.07.06  by:Rock Lu |\n");
        printf("+--------------------------------------------------+\n");
        printf("|  Command List:                                   |\n");
        printf("|    1. ls serious                                 |\n");
        printf("|    2. cd + dir_name                              |\n");
        printf("|    3. download + file_name                       |\n");
        printf("|    4. upload + file_path                         |\n");
        printf("|    5. rm + file_path                             |\n");
        printf("+--------------------------------------------------+\n");
        printf("PWD: %s\n", pwd);
        printf("%s\n", recv_buff);

        fgets(send_buff, 128, stdin);
        send_buff[strlen(send_buff)-1] = '\0';       //去回车
        remove_space(send_buff);        //去空格，大写转小写

        if (0==strncmp("rm", send_buff, 2))
        {
            printf("Need password:\n");
            fgets(password, 16, stdin);
            if(0!=strncmp("131313", password, 6)){
                printf("Wrong password!\n");
                memset(send_buff, 0, 128);
            } 
            else{
                printf("ok, please take care.....\n");
            }
        }

        send(fd_server, send_buff, 128, 0);
        memset(recv_buff, 0, 1024);        
        recv(fd_server, recv_buff, 1024, 0);

        if (0==strncmp("download", send_buff, 8))
        {
            printf("%s", recv_buff);
            fgets(send_buff, 128, stdin);
            send_buff[strlen(send_buff)-1] = '\0';       //去回车
            remove_space(send_buff);        //去空格，大写转小写
            send(fd_server, send_buff, 1, 0);

            if(0==strncmp("n", send_buff, 1)) {
                memset(recv_buff, 0, 1024);
                continue;
            }

            recv_file_tcp(send_buff+8, fd_server);
            memset(recv_buff, 0, 1024);
            printf("press any key to contiune...\n");
            getchar();
        }

        if (0==strncmp("upload", send_buff, 6))
        {
            printf("%s", recv_buff);
            fgets(send_buff, 128, stdin);
            send_buff[strlen(send_buff)-1] = '\0';       //去回车
            remove_space(send_buff);        //去空格，大写转小写
            send(fd_server, send_buff, 1, 0);

            if(0==strncmp("n", send_buff, 1)) {
                memset(recv_buff, 0, 1024);
                continue;
            }

            send_file_tcp(send_buff+6, fd_server);
            memset(recv_buff, 0, 1024);
            printf("press any key to contiune...\n");
            getchar();
        }

    }

    return 0;
}



void remove_space(char *str)
{
    int i = 0;
    char *p = str;
    while (*p)
    {
        if (isupper(*p))  *p = *p + 32;
        if ((*p) != ' ')  str[i++] = *p;
        p++;
    }
    str[i] = '\0';
}








