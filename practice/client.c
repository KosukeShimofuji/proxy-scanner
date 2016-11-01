#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define MAX_BUF 256

void print_hex (char *buf, int size)
{
  int a;
  
  for (a = 0; a < size; a++) 
    printf("0x%02X ", buf[a]);
  printf("\n");
}

int main(){
    struct sockaddr_in addr;
    int sock;
    char len = 0;
    char recv_buf[MAX_BUF];
    char header[MAX_BUF*2];

    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
        perror("socket");
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(1080);
    addr.sin_addr.s_addr = inet_addr("192.168.10.102");

    printf("[+] TARGET = %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1){
        perror("connect");
        return 1;
    };

    char step_one_request[] = {
        0x05, /* socks version */
        0x01, /* auth list num */
        0x00  /* unencessary authentication */
    };

    send(sock, step_one_request, sizeof(step_one_request), 0);

    printf("[+] SENDED STEP ONE REQUEST (%d) = ", sizeof(step_one_request));
    print_hex(step_one_request, sizeof(step_one_request));

    len = recv(sock, recv_buf, MAX_BUF, 0);
    if( len > 0 ){
        printf("[+] RECVED DATA(%d): ", len);
        print_hex(recv_buf, len);

        if(recv_buf[0] != 0x05){
            printf("[!] Invalid socks version\n");
            goto CONNECTION_END;
        }
        if(recv_buf[1] != 0x00){
            printf("[!] Invalid authentication method\n");
            goto CONNECTION_END;
        }

        char step_two_request [MAX_BUF] = {
            0x05, /* socks version */
            0x01, /* connect method */
            0x00, /* reserved */
            0x01  /* ip v4 */
        };

        addr.sin_port = htons(80);
        addr.sin_addr.s_addr = inet_addr("59.106.174.199");

        memcpy(step_two_request + 4, &addr.sin_addr.s_addr, 4);
        memcpy(step_two_request + 8, &addr.sin_port, 2);
        send(sock, step_two_request, 10, 0);
        printf("[+] SENDED STEP TWO REQUEST (%d) = ", sizeof(step_two_request));
        print_hex(step_two_request, sizeof(step_two_request));

        memset(recv_buf, '\0', MAX_BUF);

        len = recv(sock, recv_buf, 10, 0);
        if ( len > 0 ){
            printf("[+] RECVED DATA(%d): ", len);
            print_hex(recv_buf, len);

            sprintf(header, "GET /e/ HTTP/1.1\r\n"
                            "Host: taruo.net\r\n\r\n"); 
            printf("\nHeader...\n%s\n", header);
            send(sock, header, strlen(header), 0);

            memset(recv_buf, '\0', MAX_BUF);
            len = recv(sock, recv_buf, MAX_BUF, 0);
            printf("%d\n", len);
        }
    }

    CONNECTION_END:
    close(sock);

    return 0;
}

