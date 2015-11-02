#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

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
int clients[128];
int fd;
int c ;

int clirm(int fd)
{
    int i2 = 0;
    int i3;
    while(i2 < c)
    {
        if(clients[i2] == fd)
        {
            c=c-1;
            for(i3=i2;i3<c;i3++)
            {
               clients[i3] = clients[i3+1];
            }
        }
        else i2++;
    }

}

int cliview()
{
    int i =0;
    if(c == 0)
    {
        printf("No clients\n");
    }
    for(i=0;i<c;i++)
    {
     printf("clients[%i] = %i\n", i, clients[i]);
    }
}

int menu(char wrbuf[512])
{
   int wrbuf2[8];
   char wrbuf3[8];
   int delfd;

   if (!strcmp(&wrbuf[0],"~menu~\n"))
    {
       system("clear");
       printf("***************Menu***************\n");
       printf("1) Select ID of client\n");
       printf("2) Exit from menu\n");
       printf("3) Delete client\n");
       printf("4) View list of onlain client\n");

       fgets(&wrbuf2[0],sizeof(wrbuf2)-1,stdin);
       if (!strcmp(&wrbuf2[0],"1\n"))
        {
           system("clear");
           printf("Select ID of client from list:\n");
           cliview();
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

       if (!strcmp(&wrbuf2[0],"3\n"))
        {
           system("clear");
           printf("Please, choose ID of client for deleting\n");
           cliview();
           printf("\nSlect ID: ");


           fgets(&wrbuf3[0],sizeof(wrbuf3)-1,stdin);

           if (!strcmp(&wrbuf3[0],"4\n"))
            {
              delfd = 4;
            };

           if (!strcmp(&wrbuf3[0],"5\n"))
            {
               delfd = 5;
            };

           if (!strcmp(&wrbuf3[0],"6\n"))
            {
               delfd = 6;
            };

           if (!strcmp(&wrbuf3[0],"7\n"))
            {
               delfd = 7;
            };

           if (!strcmp(&wrbuf3[0],"8\n"))
            {
               delfd = 8;
            };

           system("clear");
           clirm(delfd);
           close(delfd);
           FD_CLR(delfd, &readfds);
           printf("removing client with ID=%i.\n Press Enter for exit to main window\n", delfd);
           fgets(&wrbuf3[0],sizeof(wrbuf3)-1,stdin);
           if (!strcmp(&wrbuf3[0],"\n"))
            {
               system("clear");
               return 0;
            };
            return 0;
        }

       if (!strcmp(&wrbuf2[0],"4\n"))
        {
           system("clear");
           printf("*****The list of OnLine clients*****\n");
           cliview();
           printf("Press Enter for exit to main winwow\n");
           fgets(&wrbuf3[0],sizeof(wrbuf3)-1,stdin);
           if (!strcmp(&wrbuf3[0],"\n"))
            {
               system("clear");
               return 0;
            };
           return 0;
        }

    }
}

int main() {
 int server_sockfd, client_sockfd;
 int server_len, client_len;
 struct sockaddr_in server_address;
 struct sockaddr_in client_address;
 int result;

 //struct Message msg;
 int cnt = sizeof(msg);
 //int rc;
 //int c = 0;
 //int clients[128];

 int rc;
c = 0;


 //fd_set readfds, testfds;


 server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(9734);
  server_len = sizeof(server_address);

 bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

 listen(server_sockfd, 5);

  FD_ZERO(&readfds);
  FD_SET(server_sockfd, &readfds);
  FD_SET(0, &readfds);

  while(1) {

   // int fd;
    int nread;


 m1:   testfds = readfds;


    printf("server waiting\n");

    result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);

    if (result < 0) {
     perror("error select");
     exit(1);}


   for (fd = 0; fd < FD_SETSIZE; fd++) {

      if (FD_ISSET(fd, &testfds)) {

          if (fd == server_sockfd)
             {
              client_len = sizeof(client_address);
              client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_address, &client_len);
              FD_SET(client_sockfd, &readfds);
              printf("adding client on fd %d\n", client_sockfd);

              clients[c] = client_sockfd;
              c++;
              cliview();
             }

          else
          {
            if (fd == 0)
            {
                    rc = read(0,wrbuf,sizeof(wrbuf)-1);
                    menu(wrbuf);

                    msg2.messageLen = (rc-1); // htonl
                  //  msg.messageID =(250); // htonl
                    memcpy(msg2.messageMes, wrbuf, sizeof(wrbuf));
                    send(msg2.messageID,(char*)&msg2, sizeof(msg2),0);
            }

          else
          {
              ioctl(fd, FIONREAD, &nread);
              if ((nread == 0))
               {
                clirm(fd);
                close(fd);
                FD_CLR(fd, &readfds);
                printf("removing client on fd %d\n", fd);
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

                 printf("Client messageLen: %i\n", msg.messageLen);
                 printf("Client messageID: %i\n", msg.messageID);
                 printf("Client messageMes: ");
                 int i4;
                 for (i4=0;i4<msg.messageLen;i4++)
                 {
                 printf("%c",msg.messageMes[i4]);
                 }
                 printf("\nserving client on fd %d\n", fd);

                 //Переотправка указанному клиенту//
                 send(msg.messageID,(char*)&msg, sizeof(msg),0);



                }
               }
              }
             }
            }
           }
          }

