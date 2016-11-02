#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAX_BUF 256

void chomp(char *str){
    int len;
    len = strlen(str);
    if((len > 0) && (str[len - 1] == '\n')){
        str[len - 1] = '\0';
    }
    return;
}

int main(int argc, char *argv[]){
    int opts;
    char *target_list = NULL;
    char *uri = NULL;
    char *signature = NULL;
    FILE *fp;
    char line[MAX_BUF];

    while((opts=getopt(argc, argv, "l:u:s:")) != -1){
        switch(opts){
            case 'l':
                target_list = optarg;
                break;
            case 'u':
                uri = optarg;
                break;
            case 's':
                signature = optarg;
                break;
            default:
                break;
        }
    }

    printf("[+] TARGET_LIST : %s\n", target_list);
    printf("[+] URI : %s\n", uri);
    printf("[+] SIGNATURE : %s\n", signature);

    if((fp = fopen(target_list, "r")) == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while((fgets(line, MAX_BUF, fp)) != NULL){
        chomp(line);
        printf("%s\n", line);
    } 

    return 0;
}


