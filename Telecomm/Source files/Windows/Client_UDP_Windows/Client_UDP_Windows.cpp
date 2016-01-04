#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
#include <io.h>
#include <devioctl.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <clocale>
#include <cstring>

#define MAXBUF 1024
#define DEF_PORT 8888

#pragma comment(lib, "ws2_32.lib")

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
int client_sockfd;
int client_sockfd2;
struct sockaddr_in server;

DWORD WINAPI threadHandler(LPVOID);

int menu(char wrbuf[512],int rc)
{
   int nummenu;
   int numid;
   char wrbuf2[8];
   char wrbuf3[8];
   int len = rc;

    if (strncmp(wrbuf,"~menu~\n",7)==0)
    {
       system("cls");
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

           system("cls");
		   printf("*****Khutornoy's client from SPBSPU v1.0*****\n\n");
		   printf("Type '~menu~' for see menu\n\n");

           memset(&wrbuf,NULL,sizeof(wrbuf));
           return 0;
        }

       if (nummenu == 3)
        {
           system("cls");
		   printf("*****Khutornoy's client from SPBSPU v1.0*****\n\n");
		   printf("Type '~menu~' for see menu\n\n");
           memset(&wrbuf,NULL,sizeof(wrbuf));
           return 0;
        }

       if (nummenu == 2)
        {
           system("cls");
           msg2.messageType = 2;
           msg2.messageLen = 0;
           memcpy(msg2.messageMes, wrbuf, sizeof(wrbuf));
		   sendto(client_sockfd2, (char*)&msg2, sizeof(msg2), 0, (sockaddr *)&server, sizeof(server));

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
	char buff[1024];
    int port = 0;

    for(;;)
    {
    system("cls");
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

	sprintf_s(cip1,"%03i",ip1);
	sprintf_s(cip2,"%03i",ip2);
	sprintf_s(cip3,"%03i",ip3);
	sprintf_s(cip4,"%03i",ip4);

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
    Sleep(3);
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

    if (WSAStartup(0x202,(WSADATA *)&buff[0]))
    {
      printf("WSAStart error %d\n",WSAGetLastError());
      return -1;
    }

char namebuf[32];
int rc;

server.sin_addr.s_addr = inet_addr(ipbuf);
server.sin_family = AF_INET;
server.sin_port = htons(port);

client_sockfd = socket(AF_INET , SOCK_DGRAM , 0);
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

printf("\nPlease, input your name. Must be < 30 character.\n");
gets_s(&namebuf[0],sizeof(namebuf)-1);
namebuf[31] = '\0';
namebuf[30] = '\n';
printf("\nOk...\n");

memcpy(msg2.messageName, namebuf, sizeof(namebuf));
msg2.messageType = 1;
msg2.messageSizename = strlen(namebuf);
sendto(client_sockfd, (char*)&msg2, sizeof(msg2), 0, (sockaddr *)&server, sizeof(server));
msg2.messageType = 0;

HANDLE t; 
int number = 0;
t = CreateThread(NULL, 0, threadHandler, (LPVOID)number, 0, NULL);

system("cls");
printf("*****Khutornoy's client from SPBSPU v1.0*****\n\n");
printf("Type '~menu~' for see menu\n\n");

while(1)
{
				   printf("\n<- Your message \n");
                   rc = _read(0,wrbuf,sizeof(wrbuf)-1);

                   if (strncmp(wrbuf,"~menu~\n",7)==0)
                   {
					  menu(wrbuf,rc); 
			       }

                   else
            {
				   msg2.messageType = 0;
                   msg2.messageLen = (rc-1); 
                   memcpy(msg2.messageMes, wrbuf, sizeof(wrbuf));
				   sendto(client_sockfd2, (char*)&msg2, sizeof(msg2), 0, (sockaddr *)&server, sizeof(server));
                   memset(&wrbuf,NULL,sizeof(wrbuf));}
			}

}

DWORD WINAPI threadHandler(LPVOID param)
{ 
	 sockaddr_in server_addr;
		 int server_addr_size=sizeof(server_addr);

			int bsize=recvfrom(client_sockfd, (char*)&msg, sizeof(msg), 0, (sockaddr *)&server_addr, &server_addr_size);

			if (bsize==SOCKET_ERROR)
			{
					printf("You was uncconected or kicked\n", WSAGetLastError());
					closesocket(client_sockfd);
					return 0;
			}
		
			if(msg.messageType == 1)
			{
								
				int newport;
				newport = msg.messageID;		
				server.sin_port = htons(msg.messageID);
				client_sockfd2 = socket(AF_INET , SOCK_DGRAM , 0);

				printf("NEW PORT IS: %i", server.sin_port);

				if (client_sockfd2 == -1)
				{
				printf("Could not create socket");
				}

				if (connect(client_sockfd2 , (struct sockaddr *)&server , sizeof(server)) < 0)
				{
				perror("connect failed. Error");
				return 1;
				}
				printf("\nOk...\n");
				printf("Succses connect to server\n");
				closesocket(client_sockfd);
			}

	for(;;) 
	{ 
		 sockaddr_in server_addr2;
		 int server_addr_size2=sizeof(server_addr2);

			int bsize=recvfrom(client_sockfd2, (char*)&msg, sizeof(msg), 0, (sockaddr *)&server_addr2, &server_addr_size2);

			if (bsize==SOCKET_ERROR)
			{
					printf("You was uncconected or kicked\n", WSAGetLastError());
					closesocket(client_sockfd2);
					return 0;
			}
			
			if((msg.messageType == 0)||(msg.messageType == 2))
			{
				Beep(1000,50);
                printf("-> Message from %s\n",msg.messageName);

                int i;
                for (i=0;i<msg.messageLen;i++)
                {
                printf("%c",msg.messageMes[i]);
                }
				printf("\n\n<- Your message \n");
			}
	}	
	return 0;
}

