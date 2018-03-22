#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

void* echo_reply(void *);
void echo_server(int);
char* my_strcpy(char *,char *);
void make_send_data(char *,char *,char *);
char number_data_1[1024];
char number_data_2[1024];
char *send_data;

char test[1024];

char check_order[1024];
char android_order_1[1024];
char android_order_2[1024];
int i;

struct sockaddr_in saddr;
struct sockaddr_in caddr;

int len;

int main( int argc, char *argv[] ){
    i=0;
    strcpy(number_data_1,"0");
    strcpy(number_data_2,"0");
    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s Port_number\n", argv[0] );

        exit( 1 );
    }
    int  port = atoi( argv[1] );
    echo_server(port);
}




void echo_server(int portno){
    int fd1;
    int fd2;

    pthread_t worker;

    //通信用のソケットを生成
    if ( ( fd1 = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
        perror( "socket" );
        fprintf(stderr,"socket");

        exit( 1 );
    }
    //saddrが0じゃないとbindがまずいらしい
    memset( &saddr, 0, sizeof( saddr ) );
    //ソケットとアドレス、ポートのつなぎ合わせ
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(portno);

    if ( bind( fd1, ( struct sockaddr * )&saddr, ( socklen_t )sizeof( saddr ) ) < 0 ) {
    perror( "bind" );
    fprintf(stderr,"bind");

    exit( 1 );
  }

  if ( listen( fd1, 5 ) < 0 ) {
    perror( "listen" );
    fprintf(stderr,"listen");
    exit( 1 );
  }

  while( 1 ) {
    //接続要求の受付
    len = sizeof( caddr );
    if ( ( fd2 = accept( fd1, ( struct sockaddr * )&caddr, ( socklen_t * )&len ) ) < 0 ) {
      perror( "accept" );
      fprintf(stderr,"accept");
      exit( 1 );
    }
    printf("start conection");
    if( pthread_create( &worker, NULL, (void *)echo_reply,&fd2)!= 0 ){
 	    perror("pthread_create()");
        fprintf(stderr,"pthread_create()");
	    exit( 1 );
 	}
 	pthread_detach( worker );
  }
  close(fd1);
}

void* echo_reply(void *fd){
    int fd2=*(int *)fd;
    int rsize;
    char buf[1024];
    char buf2[1024];
    //char buf2[1024];
    printf("connected");
    do {
        rsize = recv(fd2, buf, 1024, 0);
        if (rsize == 0) { /* クライアントが接続を切ったとき */
            fprintf(stderr,"やきにく");
            break;
        }
        else if (rsize == -1) {
             fprintf(stderr,"やきにく");
            perror("recv");
            exit(EXIT_FAILURE);
        }
        else {
            fprintf( stderr, "%s\n", buf);
            while(1){
            if(strcmp(buf,"arduino1")==0){
                len=read(fd2,buf2,1024);
                fsync(fd2);
                buf2[len] = '\0';
                strcpy(number_data_2,buf2);
                fprintf(stderr,"room1:%s room2:%s\n",number_data_1,number_data_2);
            }
            else if(strcmp(buf,"arduino2")==0){
                len=read(fd2,buf2,1024);
                fsync(fd2);
                buf2[len] = '\0';
                strcpy(number_data_2,buf2);
                fprintf(stderr,"room1:%s room2:%s\n",number_data_1,number_data_2);
            }
            else if(strcmp(buf,"change")==0){
                len=read(fd2,buf2,1024);
                fsync(fd2);
                buf2[len] = '\0';
                strcpy(number_data_1,buf2);
                fprintf(stderr,"room1:%s room2:%s\n",number_data_1,number_data_2);
            }

            else if(strcmp(buf,"check")==0){
                char buf3[1024];
                strcat(buf3,number_data_1);
                strcat(buf3,"|");
                strcat(buf3,number_data_2);
                 write( fd2, buf3, strlen(buf3)+1 );
                 len=read(fd2,buf2,1024);
                fsync(fd2);
                buf2[len] = '\0';
                strcpy(number_data_1,buf2);
                fprintf(stderr,"room1:%s room2:%s\n",number_data_1,number_data_2);

            }

            else{
               write( fd2, number_data_1, strlen( number_data_1)+1 );
                 write( fd2, number_data_2, strlen( number_data_2)+1 ); 
                /*len=read(fd2,buf2,1024);
                fsync(fd2);
                buf2[len] = '\0';
                strcpy(number_data_2,buf2);
                fprintf(stderr,"room1:%s room2:%s\n",number_data_1,number_data_2);
*/
            }
            }
        }
    }
    while(1);
    if(close(fd2)<0){
        fprintf(stderr,"やきに0く");

        perror("close");
        exit(EXIT_FAILURE);
    }

  fprintf(stderr,"やきにくk");

  return 0;
}
