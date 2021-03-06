#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>


#define DEF_PORT 8888
#define DEF_IP "127.0.0.1"

//DWORD WINAPI threadHandler(LPVOID);
//DWORD WINAPI clientHandler(LPVOID);



char names[32][32];
char wrbuf[512];
int fd;
int clients[128];
int sizenames[128];
int c = 0;
//HANDLE threads[128];
pthread_t threads[128];
struct sockaddr_in  soki[512];
struct sockaddr_in newlistenerInfo;

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
    printf("Name: ",i);


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
    printf("thr[%i] = %i\n", i, threads[i]);
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
            c=c-1;


            for(i1=i;i1<c;i1++)
            {
               clients[i1] = clients[i1+1];
               sizenames[i1] = sizenames[i1+1] ;
               soki[i1] = soki[i1+1];

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

int menu(char wrbuf[512])
{
   char wrbuf2[8];
   char wrbuf3[8];
   int numcli;
   int nummenu;

   if (strncmp(wrbuf,"~menu~\n",7)==0)
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

           //TerminateThread(threads[numcli],NULL);
           pthread_kill(threads[numcli],SIGUSR1);
           close(clients[numcli]);
//           ZeroMemory(&soki[numcli], sizeof(struct sockaddr_in));
//           ZeroMemory(&clients[numcli], sizeof(int));
//           ZeroMemory(&sizenames[numcli], sizeof(int));
           memset(&soki[numcli],NULL,sizeof(struct sockaddr_in));
            memset(&clients[numcli],NULL,sizeof(int));
             memset(&sizenames[numcli],NULL,sizeof(int));
           //CloseHandle(threads[numcli]);
           clirm(clients[numcli]);

           system("clear");
           printf("removing %s with ID = %i.\n Press Enter for exit to main window\n",&names[numcli], numcli);

           fgets(&wrbuf3[0],sizeof(wrbuf3)-1,stdin);
            if (strncmp(wrbuf3,"\n",1)==0)
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

void obr()
{
pthread_exit(0);
signal(SIGUSR1,obr);
}

void* threadHandler(void* args)
{
    for(;;)
    {
        read(0,wrbuf,sizeof(wrbuf)-1);
        menu(wrbuf);
    }
    return 0;
}

void* clientHandler(void* args)
{
    signal(SIGUSR1,obr);
    int sock = args;
    int res = 0;
    struct Message msg;
    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);

    res = recvfrom(sock, (char*)&msg, sizeof(msg), 0, (struct sockaddr *) &client_addr, &client_addr_size);
    soki[c-1].sin_port = client_addr.sin_port;

    if (res == 0)
    {
    printf("socket with id = %i was unconnected\n", sock);
    clirm(sock);
    cliview();
    close(sock);


    int h = 0;
    for (h=0;h<c;h++)
    {
        if(clients[h] == sock)
        {
         memset(&soki[h],NULL,sizeof(struct sockaddr_in));
         memset(&clients[h],NULL,sizeof(int));
         memset(&sizenames[h],NULL,sizeof(int));
        }
    }

    return 0;
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
        int r = 0;
        for (r=0;r<c;r++)
        {
            if (msg.messageID == clients[r])
            {
             break;
            }
        }

    sendto(msg.messageID, (char*)&msg, sizeof(msg), 0, (struct sockaddr *)&soki[r], sizeof(soki[r]));
    }

    if(type == 2)
    {
        int x = 0 ;
        char str[512];
        char sername[] = "Online Clients From Server \n";
        for (x=0;x<c;x++)
        {
        memcpy(msg.messageName, sername, sizeof(sername));
        msg.messageSizename = strlen(msg.messageName);
        sprintf(str,"%i)User onlain: %s\n ID user: %i",x+1, &names[x],clients[x]);
        msg.messageLen = strlen(str);
        memcpy(msg.messageMes, str, sizeof(str)-1);
        sendto(sock, (char*)&msg, sizeof(msg), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        sleep(1);
        memset(&str,NULL,sizeof(str));
        }
    }

    for(;;)
    {
    res = 0;
    res = recvfrom(sock, (char*)&msg, sizeof(msg), 0, (struct sockaddr *) &client_addr, &client_addr_size);

    if (res == 0)
    {
    printf("socket with id = %i was unconnected\n", sock);
    clirm(sock);
    cliview();
    close(sock);

    int h = 0;
    for (h=0;h<c;h++)
    {
        if(clients[h] == sock)
        {
        memset(&soki[h],NULL,sizeof(struct sockaddr_in));
        memset(&clients[h],NULL,sizeof(int));
        memset(&sizenames[h],NULL,sizeof(int));
        }
    }
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
        if((msg.messageMes[0] == '~') &&
            (msg.messageMes[1] == 'q') &&
            (msg.messageMes[2] == 'u') &&
            (msg.messageMes[3] == 'i') &&
            (msg.messageMes[4] == 't') &&
            (msg.messageMes[5] == '~') &&
            (msg.messageMes[6] == '\n'))
            {
            int h = 0;
            for (h=0;h<c;h++)
            {
                if(clients[h] == sock)
                {
                memset(&soki[h],NULL,sizeof(struct sockaddr_in));
                memset(&clients[h],NULL,sizeof(int));
                memset(&sizenames[h],NULL,sizeof(int));
                clirm(clients[h]);
                }
            }
           close(sock);
           return 0;
            }

            int r = 0;
            for (r=0;r<c;r++)
            {
                if (msg.messageID == clients[r])
                {
                 break;
                }
            }
    sendto(msg.messageID, (char*)&msg, sizeof(msg), 0, (struct sockaddr *)&soki[r], sizeof(soki[r]));
    }

    if(type == 2)
    {
        int x = 0 ;
        char str[512];
        char sername[] = "Online Clients From Server \n";
        for (x=0;x<c;x++)
        {
        memcpy(msg.messageName, sername, sizeof(sername));
        msg.messageSizename = strlen(msg.messageName);
        sprintf(str,"%i)User onlain: %s\n ID user: %i",x+1, &names[x],clients[x]);
        msg.messageLen = strlen(str);
        memcpy(msg.messageMes, str, sizeof(str)-1);
        sendto(sock, (char*)&msg, sizeof(msg), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        sleep(1);
        memset(&str,NULL,sizeof(str));
        }
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

int listener = socket(AF_INET, SOCK_DGRAM, 0 );
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


int number = 0;
pthread_t t;
int res24 = pthread_create(&t, NULL, threadHandler, (void *)(number));
if (res24)
{
printf("Error while creating new thread\n");
}

system("clear");
printf("*****Khutornoy's server from SPBSPU v1.0*****\n\n");
printf("Type '~menu~' for see menu\n\n");

struct Message msg;

for(;;)
{
    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);
    int bsize = recvfrom(listener, (char*)&msg, sizeof(msg), 0, (struct sockaddr *) &client_addr, &client_addr_size);

    soki[c] = client_addr;

    if (bsize==0) printf("recvfrom() error: %d\n");

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

    if(type==1)
    {

        sizenames[c] = msg.messageSizename;

        int u = 0;
        for(u=0;u<32;u++)
        {
        names[c][u] = msg.messageName[u];
        }

        msg.messageSizename = 0;
        msg.messageLen = 0;
        msg.messageID = 10000+c;
        msg.messageType = 1;
        sendto(listener, (char*)&msg, sizeof(msg), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));

        newlistenerInfo.sin_family = AF_INET;
        newlistenerInfo.sin_port = htons( msg.messageID );
        newlistenerInfo.sin_addr =  client_addr.sin_addr;

        int newlistener = socket(AF_INET, SOCK_DGRAM, 0 );
        if ( newlistener < 0 ) {
        perror( "Can't create socket to listen: " );
        exit(1);
        }

        int res = bind(newlistener, ( struct sockaddr * )&newlistenerInfo, sizeof(newlistenerInfo) );
        if ( res < 0 ) {
        perror( "Can't bind socket" );
        exit( 1 );
        }

        soki[c].sin_port = newlistenerInfo.sin_port;

        pthread_t t2;
        int res123 = pthread_create(&t2, NULL, clientHandler, (void *)(newlistener));
        if (res123)
        {
        printf("Error while creating new thread\n");
        }

        printf("New client connected with id = %i\n", newlistener);

        clients[c] = newlistener;
        threads[c] = t2;
        c++;

        cliview();
        thrview();

    }
}
return 0;
}




