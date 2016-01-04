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
#define DEF_PORT 8888

#pragma pack(push,1)
struct Message {
int messageType;
int messageLen;
int messageID;
int messageSizename;
char messageName[32];
char messageMes[512];
};
#pragma pack(pop)

char wrbuf[512];
struct Message msg;
struct Message msg2;
fd_set readfds, testfds;


int menu(char wrbuf[512])

{
   int nummenu;
   int numid;
   char wrbuf2[8];
   char wrbuf3[8];


   if (!strcmp(&wrbuf[0],"~menu~\n"))
    {
       system("clear");
       printf("***************Menu***************\n");
       printf("1) Select ID of client\n");
       printf("2) View OnLine client\n");
       printf("3) Exit from menu\n");

       fgets(&wrbuf2[0],sizeof(wrbuf2)-1,stdin);
       nummenu = atoi(&wrbuf2[0]);


       if (nummenu == 1)
        {

           printf("Select ID: ");

           fgets(&wrbuf3[0],sizeof(wrbuf3)-1,stdin);
           numid = atoi(&wrbuf3[0]);

           msg2.messageID = numid;
           system("clear");
           memset(&wrbuf,NULL,sizeof(wrbuf));
           return 0;
        }


       if (nummenu == 3)
        {
           system("clear");
           memset(&wrbuf,NULL,sizeof(wrbuf));
           return 0;
        }

       if (nummenu == 2)
        {
           system("clear");

           msg2.messageType = 2;
           msg2.messageLen = 0;
           memcpy(msg2.messageMes, wrbuf, sizeof(wrbuf));
           send(3,(char*)&msg2, sizeof(msg2),0);

           msg2.messageType = 0;
           memset(&wrbuf,NULL,sizeof(wrbuf));
           return 0;
        }

    }


return 0;
}


int main()
{

    char ipbuf[18];
    char portbuf[10];

    int port = 0;

    for(;;)
    {

    system("clear");
    printf("*****Khutornoy's client from SPBSPU v1.0*****\n\n");

    int ip1 = 0;
    int ip2 = 0;
    int ip3 = 0;
    int ip4 = 0;

    char cip1[10];
    char cip2[10];
    char cip3[10];
    char cip4[10];

    printf("Please, choose IP-addres of server\n");
    printf("Like this: xxx.xxx.xxx.xxx\n");
    printf("xxx must be in range from 0 to 255\n");
    printf("For example: 127.000.000.001\n\n");

    fgets(&ipbuf[0],sizeof(ipbuf)-1,stdin);

    ip1= atoi(&ipbuf[0]);
    ip2= atoi(&ipbuf[4]);
    ip3= atoi(&ipbuf[8]);
    ip4= atoi(&ipbuf[12]);


    sprintf(cip1,"%03i",ip1);
    sprintf(cip2,"%03i",ip2);
    sprintf(cip3,"%03i",ip3);
    sprintf(cip4,"%03i",ip4);


    if ((((cip1[0] != ipbuf[0]) || (cip1[1] != ipbuf[1]) || (cip1[2] != ipbuf[2])) ||
        ((cip2[0] != ipbuf[4]) || (cip2[1] != ipbuf[5]) || (cip2[2] != ipbuf[6])) ||
        ((cip3[0] != ipbuf[8]) || (cip3[1] != ipbuf[9]) || (cip3[2] != ipbuf[10])) ||
        ((cip4[0] != ipbuf[12]) || (cip4[1] != ipbuf[13]) || (cip4[2] != ipbuf[14])))

     || (((ip1<0) || (ip1>255)) ||
         ((ip2<0) || (ip2>255)) ||
         ((ip3<0) || (ip3>255)) ||
         ((ip4<0) || (ip4>255))))

    {
    printf("\nError addres!\n");
    printf("Follow the rules!\n");
    printf("Please repeat!\n");
    sleep(3);
    }
    else

    {
    printf("\nOk...\n");
    printf("Addres of server = %i.%i.%i.%i\n\n", ip1,ip2,ip3,ip4);
    break;
    }
    }

    printf("Please, choose PORT of server\n");
    fgets(&portbuf[0],sizeof(portbuf)-1,stdin);
    port = atoi(&portbuf[0]);
    printf("\nOk...\n");
    printf("Port of server = %i\n", port);

    if ((port < 0) || (port > 65535))
    {
    printf("Uncorrect port = %i\n", port);
    printf("Using default port %d\n",DEF_PORT);
    port = DEF_PORT;
    }

char namebuf[32];
int client_sockfd;
struct sockaddr_in server;
int result;
int rc;

server.sin_addr.s_addr = inet_addr(ipbuf);
server.sin_family = AF_INET;
server.sin_port = htons(port);


client_sockfd = socket(AF_INET , SOCK_STREAM , 0);
if (client_sockfd == -1)
{
printf("Could not create socket");
}

if (connect(client_sockfd , (struct sockaddr *)&server , sizeof(server)) < 0)
{
perror("connect failed. Error");
return 1;
}
printf("\nOk...\n");
printf("Succses connect to server\n");

FD_ZERO(&readfds);
FD_SET(client_sockfd, &readfds);
FD_SET(0, &readfds);

printf("\nPlease, input your name. Must be < 30 character.\n");
gets(&namebuf[0]);
namebuf[31] = '\0';
namebuf[30] = '\n';
printf("\nOk...\n");

memcpy(msg2.messageName, namebuf, sizeof(namebuf));
msg2.messageType = 1;
msg2.messageSizename = strlen(namebuf);
send(3,(char*)&msg2, sizeof(msg2),0);
msg2.messageType = 0;


system("clear");
printf("*****Khutornoy's client from SPBSPU v1.0*****\n\n");
printf("Type '~menu~' for see menu\n\n");


while(1)
{

int fd;
int nread;

testfds = readfds;

result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);

    if (result < 0)
    {
     perror("error select");
     exit(1);
    }


    for (fd = 0; fd < FD_SETSIZE; fd++)
    {

        if (FD_ISSET(fd, &testfds))
        {

             //обработка ввода с клавиатуры
           if (fd == 0)
           {
                   printf("\n<- Your message \n");
                   rc = read(0,wrbuf,sizeof(wrbuf)-1);

                   if (!strcmp(&wrbuf[0],"~menu~\n"))
                   {
                   menu(wrbuf);
                   }
                   else
            {
                   msg2.messageLen = (rc-1); // htonl
                   memcpy(msg2.messageMes, wrbuf, sizeof(wrbuf));
                   send(3,(char*)&msg2, sizeof(msg2),0);

                   //printf("Your message \n");
                   memset(&wrbuf,NULL,sizeof(wrbuf));}
           }

            //Прием сообщений и обработка отключения от сервера
          else
          {
             ioctl(fd, FIONREAD, &nread);
             if ((nread == 0))
              {
               close(fd);
               FD_CLR(fd, &readfds);
               printf("Server was unconnected or kicked you!");
              }


              else
             {
              rc =  recv(fd, (char*)&msg , sizeof(msg) ,0);
              if (rc == 0)
                  perror("uncorrect socket from select");
              if (rc < 0)
                  perror("error vizova recv");



               // printf("Server messageLen: %i\n", msg.messageLen);
               // printf("Server messageID: %i\n", msg.messageID);
                printf("-> Message from %s\n",msg.messageName);

                int i;
                for (i=0;i<msg.messageLen;i++)
                {
                printf("%c",msg.messageMes[i]);
                }

                printf("\n\n");
                printf("<- Your message \n");
              }

             }

            }
        }
    }
}
