#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<math.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXBUF 1024

#define PORT 9734



#pragma pack(push,1)
struct Message {
    int messageLen;
   int messageID;
    char messageMes[512];
    };
#pragma pack(pop)
char wrbuf[512];
struct Message msg;
struct Message msg2;
fd_set readfds, testfds;






int menu(char wrbuf[512])
{
   int wrbuf2[8];
   char wrbuf3[8];

   if (!strcmp(&wrbuf[0],"~menu~\n"))
    {
       system("clear");
       printf("***************Menu***************\n");
       printf("1) Select ID of client\n");
       printf("2) Exit from menu\n");

       fgets(&wrbuf2[0],sizeof(wrbuf2)-1,stdin);
       if (!strcmp(&wrbuf2[0],"1\n"))
        {

           printf("Slect ID: ");

           fgets(&wrbuf3[0],sizeof(wrbuf3)-1,stdin);

           if (!strcmp(&wrbuf3[0],"4\n"))
            {
               msg2.messageID = 4;
            };

           if (!strcmp(&wrbuf3[0],"5\n"))
            {
               msg2.messageID = 5;
            };

           if (!strcmp(&wrbuf3[0],"6\n"))
            {
               msg2.messageID = 6;
            };

           if (!strcmp(&wrbuf3[0],"7\n"))
            {
               msg2.messageID = 7;
            };

           if (!strcmp(&wrbuf3[0],"8\n"))
            {
               msg2.messageID = 8;
            };

           system("clear");
           return 0;
        }

       if (!strcmp(&wrbuf2[0],"2\n"))
        {
           system("clear");
           return 0;
        }


    }
}





int main()
{
int client_sockfd;
struct sockaddr_in server;

 int result;
//fd_set readfds, testfds;

//struct Message msg;
int cnt = sizeof(msg);
int rc;


//Создание сокета
client_sockfd = socket(AF_INET , SOCK_STREAM , 0);
if (client_sockfd == -1)
{
printf("Could not create socket");
}
puts("Socket created\n");

server.sin_addr.s_addr = inet_addr("127.0.0.1");
server.sin_family = AF_INET;
server.sin_port = htons( 9734 );

//Connect to remote server
if (connect(client_sockfd , (struct sockaddr *)&server , sizeof(server)) < 0)
{
perror("connect failed. Error");
return 1;
}

FD_ZERO(&readfds);
FD_SET(client_sockfd, &readfds);
FD_SET(0, &readfds);


while(1)
{
    int fd;
    int nread;
   // char wrbuf[512];

m1:   testfds = readfds;


   printf("\nclient waiting\n");


   result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);

   if (result < 0) {
    perror("error select");
    exit(1);}


  for (fd = 0; fd < FD_SETSIZE; fd++) {

     if (FD_ISSET(fd, &testfds)) {


 //обработка ввода с клавиатуры
           if (fd == 0)
           {

                   rc = read(0,wrbuf,sizeof(wrbuf)-1);
                   menu(wrbuf);




                   msg2.messageLen = (rc-1); // htonl
                 //  msg.messageID = (4); // htonl
                   memcpy(msg2.messageMes, wrbuf, sizeof(wrbuf));
                   send(3,(char*)&msg2, sizeof(msg2),0);
           }
//Прием сообщений и обработка отключения от сервера
         else
         {
             ioctl(fd, FIONREAD, &nread);
             if ((nread == 0))
              {
               close(fd);
               FD_CLR(fd, &readfds);
               printf("removing server on fd %d\n", fd);
              }


              else {
              rc =  recv(fd, (char*)&msg + sizeof(msg) - cnt, cnt,0);
              if (rc == 0)
                  perror("uncorrect socket from select");
              if (rc < 0)
                  perror("error vizova recv");

                cnt = cnt -rc;
                if(cnt>0) goto m1;
                cnt=sizeof(msg);

                printf("Server messageLen: %i\n", msg.messageLen);
                printf("Server messageID: %i\n", msg.messageID);
                printf("Server messageMes: ");
                int i;
                for (i=0;i<msg.messageLen;i++)
                {
                printf("%c",msg.messageMes[i]);
                }
             //   printf("\n server on fd %d\n", fd);


              }
             }
            }
           }
          }
         }
