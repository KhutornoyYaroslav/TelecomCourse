#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>

#define DEF_PORT 8888
#define DEF_IP "127.0.0.1"

fd_set readfds, testfds;
pthread_t threads[128];

char names[32][32];
char wrbuf[512];
int fd;
int clients[128];
int sizenames[128];
int c = 0;

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


int cliview()
{
    int i = 0;
    int y = 0;

    if(c == 0)
    {
    printf("No clients\n");
    }

    for(i=0;i<c;i++)
    {
    printf("Clients[%i] sock = %i  ", i, clients[i]);

    for(y=0;y<sizenames[i];y++)
    {
    printf("%c", names[i][y]);
    }
    printf("\n");
    }


return 0;
}

int thrview()
{
    int i =0;

    if(c == 0)
    {
    printf("No thr\n");
    }

    for(i=0;i<c;i++)
    {
    printf("thr[%i] = %i\n", i, (int)threads[i]);
    }
return 0;
}

int clirm(int sock)
{

    int i = 0;
    int i1;
    int i2;
    while(i < c)
    {
        if(clients[i] == sock)
        {
            FD_CLR(clients[i], &readfds);

            c=c-1;
            for(i1=i;i1<c;i1++)
            {               
               clients[i1] = clients[i1+1];
               sizenames[i1] = sizenames[i1+1] ;

               for(i2=0;i2<32;i2++)
               {
               names[i1][i2] = names[i1+1][i2];
               }
            }
        }
        else i++;
    }

return 0;
}

void obr()
{
pthread_exit(0);
signal(SIGUSR1,obr);
}

void* clientHandler(void* args)
{
    signal(SIGUSR1,obr);
    int sock = (int)args;
    int res = 0;
    struct Message msg;

    for(;;)
    {

    res =  recv(sock, (char*)&msg, sizeof(msg) ,0);
    if (res == 0)
    {
    printf("socket with id = %i was unconnected\n", sock);
    clirm(sock);
    cliview();
    close(sock);
    pthread_exit(NULL);
    }

    printf("\nClient messageLen: %i\n", msg.messageLen);
    printf("Client messageTo: %i\n", msg.messageID);
    printf("Client messageType: %i\n", msg.messageType);
    printf("Client messageMes: ");

    int i;
    for (i=0;i<msg.messageLen;i++)
    {
    printf("%c",msg.messageMes[i]);
    }

    printf("\n");
    printf("Client messageFrom: %s\n", msg.messageName);

    int type;
    type = msg.messageType;

    if(type == 0)
    {
        //Переотправка указанному клиенту//
        send(msg.messageID,(char*)&msg, sizeof(msg),0);
    }

    if(type == 1)
    {
        int k = 0;
        int u = 0;

        for(k=0;k<128;k++)
        {
            if(clients[k]==sock)
            {
                sizenames[k] = msg.messageSizename;

                for(u=0;u<32;u++)
                {
                names[k][u] = msg.messageName[u];
                }

            }

        }
    }

    if(type == 2)
    {

        int x = 0 ;
        char str[512];
        char sername[] = "Online Clients From Server \n";
        for (x=0;x<c;x++)
        {
        msg.messageLen = sizeof(str)-1;

        memcpy(msg.messageName, sername, sizeof(sername));

        msg.messageSizename = strlen(msg.messageName);

        sprintf(str,"%i)User onlain: %s\n ID user: %i",x+1, &names[x],clients[x]);
        memcpy(msg.messageMes, str, sizeof(str)-1);
        send(sock,(char*)&msg, sizeof(msg)-1,0);
        sleep(1);
        memset(&str,NULL,sizeof(str));

        }
    }
  }

}

int menu(char wrbuf[512])
{
   int wrbuf2[8];
   char wrbuf3[8];
   int numcli;
   int nummenu;


   if (!strcmp(&wrbuf[0],"~menu~\n"))
    {
       system("clear");
       printf("***************Menu***************\n");
       printf("1) Exit from menu\n");
       printf("2) Delete client\n");
       printf("3) View list of onlain client\n");

       fgets(&wrbuf2[0],sizeof(wrbuf2)-1,stdin);
       nummenu = atoi(&wrbuf2[0]);

       if (nummenu == 1)
        {
           system("clear");
           return 0;
        }

       if (nummenu == 2)
        {
           system("clear");
           printf("Please, choose ID of client for deleting\n");
           cliview();
           printf("\nSlect ID: ");

           fgets(&wrbuf3[0],sizeof(wrbuf3)-1,stdin);
           numcli = atoi(&wrbuf3[0]);

           system("clear");
           pthread_kill(threads[numcli],SIGUSR1);
           close(clients[numcli]);
           clirm(clients[numcli]);

           printf("removing %s with ID = %i.\n Press Enter for exit to main window\n",&names[numcli], numcli);

           fgets(&wrbuf3[0],sizeof(wrbuf3)-1,stdin);
           if (!strcmp(&wrbuf3[0],"\n"))
            {
               system("clear");
               return 0;
            };
            return 0;
        }


       if (nummenu == 3)
        {
           system("clear");
           printf("*****The list of OnLine clients*****\n");
           cliview();
           return 0;
        }

    }
   return 0;
}

int main()
{
signal(SIGUSR1,SIG_IGN);
char ipbuf[18];
char portbuf[10];

int port = 0;

for(;;)
{

system("clear");
printf("*****Khutornoy's server from SPBSPU v1.0*****\n\n");

int ip1 = 0;
int ip2 = 0;
int ip3 = 0;
int ip4 = 0;

char cip1[10];
char cip2[10];
char cip3[10];
char cip4[10];

printf("Please, choose IP-addres for using\n");
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
printf("Your addres = %i.%i.%i.%i\n\n", ip1,ip2,ip3,ip4);
break;
}
}

printf("Please, choose PORT for using\n");
fgets(&portbuf[0],sizeof(portbuf)-1,stdin);
port = atoi(&portbuf[0]);
printf("Your port = %i\n", port);

if ((port < 0) || (port > 65535))
{
printf("Uncorrect port = %i\n", port);
printf("Using default port %d\n",DEF_PORT);
port = DEF_PORT;
}


struct sockaddr_in listenerInfo;
listenerInfo.sin_family = AF_INET;
listenerInfo.sin_port = htons( port );
listenerInfo.sin_addr.s_addr =  inet_addr(ipbuf);


int listener = socket(AF_INET, SOCK_STREAM, 0 );
if ( listener < 0 ) {
perror( "Can't create socket to listen: " );
exit(1);
}

int res = bind(listener, ( struct sockaddr * )&listenerInfo, sizeof(
listenerInfo ) );
if ( res < 0 ) {
perror( "Can't bind socket" );
exit( 1 );
}

res = listen(listener,5);
if (res) {
perror("Error while listening:");
exit(1);
}

FD_ZERO(&readfds);
FD_SET(listener, &readfds);
FD_SET(0, &readfds);

int ressel;

for(;;)
{
    testfds = readfds;

    ressel = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
    if (ressel < 0)
    {
    perror("error select");
    exit(1);
    }

    for (fd = 0; fd < FD_SETSIZE; fd++)
    {

        if (FD_ISSET(fd, &testfds))

         {

            if (fd == listener)
            {
            int client = accept(listener, NULL, NULL );
            printf("New client connected with id = %i\n", client);
            clients[c] = client;

            pthread_t thrd;
            res = pthread_create(&thrd, NULL, clientHandler, (void *)(client));
            if (res)
            {
            printf("Error while creating new thread\n");
            }
            threads[c] = thrd;
            c++;
            cliview();
            thrview();

            }


            if (fd == 0)
            {
            read(0,wrbuf,sizeof(wrbuf)-1);
            menu(wrbuf);
            }
        }
    }
}

return 0;
}



