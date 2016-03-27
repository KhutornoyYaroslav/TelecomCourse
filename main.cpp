#include <iostream>
#include <sys/types.h>
#include <windows.h>

using namespace std;

#pragma comment ( lib, "ws2_32.lib" )

int flag = 0;

int init_sock(char *ip) {

	char buff[1024];
    int len;
	int result;
    int client_sockfd;

    sockaddr_in address;

	if (WSAStartup(0x202,(WSADATA *)&buff[0]))
    {
      printf("WSAStart error %d\n",WSAGetLastError());
      return -1;
    }

	client_sockfd = socket(AF_INET , SOCK_STREAM , 0);
	if (client_sockfd == -1)
	{
	printf("Невозможно создать сокет!");
	}

    address.sin_family = AF_INET;   
	address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons(21);   

    len = sizeof(address);

    result = connect(client_sockfd, (sockaddr *)&address, len); 
	if (result < 0) 
	{
	perror("Невозможно подключиться к серверу");
	return 1;	
	}

	printf("\nГотово.\n");
	printf("Соединенился с сервером!\n");

    return client_sockfd;
}

int readServ(int s) {

    int rc;

    fd_set fdr;
    FD_ZERO(&fdr);
    FD_SET(s,&fdr);
    timeval timeout;
    timeout.tv_sec = 1;   
    timeout.tv_usec = 0;  

    do {
        char buff[512] = {' '};
        recv(s,&buff[0],512,0); 
        cout << buff;
        rc = select(s+1,&fdr,NULL,NULL,&timeout);   
    } while(rc);     

    return 2;
}

int login(int client_sockfd) {

    cout << "Введите логин: "; 
	char name[64]; 
	cin >> name;

    char str[512];
    sprintf(str,"USER %s\r\n",name);
    send(client_sockfd,str,strlen(str),0);
    readServ(client_sockfd);
	
    cout << "Введите пароль: "; char pass[64]; cin >> pass;
    sprintf(str,"PASS %s\r\n",pass);
    send(client_sockfd,str,strlen(str),0);
    //readServ(client_sockfd);

	char size[512] = {' '};
    recv(client_sockfd,size,512,0);
	cout << "\nИнформация о подключении: \n" << size << endl;

	char *tmp_char;
	int a;
	tmp_char = strtok(size," ");
    sscanf(tmp_char, "%d",&a);
	if (a/100 == 5)
	{
		flag = 0;
		return 0;
	}

	flag = 1;
    return 0;
}

int init_data(int client_sockfd, char *ip) {

	int client_data_sockfd;
	int a,b;
	int c,d,e,f;
	int len;
	int result;
	int port;

	char *tmp_char;
	char buff[128] = {' '};

    sockaddr_in address;

    send(client_sockfd,"PASV\r\n",strlen("PASV\r\n"),0);    
    recv(client_sockfd,buff,128,0);

    cout << buff; 

    tmp_char = strtok(buff,"(");
    tmp_char = strtok(NULL,"(");
    tmp_char = strtok(tmp_char, ")");
    sscanf(tmp_char, "%d,%d,%d,%d,%d,%d",&c,&d,&e,&f,&a,&b);

	port = a*256 + b;

    client_data_sockfd = socket(AF_INET, SOCK_STREAM,0);

    address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons(port);   

    len = sizeof(address);

    result = connect(client_data_sockfd, (sockaddr *)&address, len);
    if (result == -1) 
	{
    perror("Ошибка создания канала данных!");
    return -1;
    }

    return client_data_sockfd;
}

int list(int s, int s2){

	char buff[7] = "LIST\r\n";

    send(s,buff,strlen(buff),0);
	readServ(s);

	int readed;

	while(1)
	{
	char buff2[2048] = {' '};
	readed = recv(s2,&buff2[0],sizeof(buff2),0);     

	if ((readed < sizeof(buff2)))
	{
	cout << buff2;
	break;
	}			
	cout << buff2;
    } 

	return 0;
};

int nlst(int s, int s2){

	char buff[7] = "NLST\r\n";
    send(s,buff,strlen(buff),0);
	readServ(s);

	int readed;

	while(1)
	{
	char buff2[2048] = {' '};
	readed = recv(s2,&buff2[0],sizeof(buff2),0);     

	if ((readed < sizeof(buff2)))
	{
	cout << buff2;
	break;
	}			
	cout << buff2;
    } 

	return 0;
};

int retr(int s, int s2, char *file) {

	int read_old;
    int read_new = 0; 
	int i;
    char str[512];
	char *tmp_char;

    sprintf(str,"RETR %s\r\n",file);
    send(s,str,strlen(str),0);

	char size[512] = {' '};
    recv(s,size,512,0);
    cout << "\nИнформация о скачиваемом файле: \n" << size << endl;

	int a;
	tmp_char = strtok(size," ");
    sscanf(tmp_char, "%d",&a);
	if (a/100 == 5)
	{
		closesocket(s2);
		return 0;
	}

    FILE *f;
    f = fopen(file, "wb");   

    if (f == NULL)
    {
      fputs("Ошибка создания файла", stderr);
      exit(1);
    }

	i = 0;

    do {
            char buff[2048];   
            int readed = recv(s2,buff,sizeof(buff),0);   
            fwrite(buff,1,readed,f);   
			read_old = read_new;
            read_new += readed;  

			if(i==1024)
			{
			i = 0;
			system("cls");
			cout <<"\nСкачано данных: ";
			cout << read_new << " Байт\n";	
			}

			i++;
        } 
	while (read_old < read_new);

    fclose(f);
	system("cls");
	recv(s,size,512,0);
    cout <<"\n"<< size << endl;
    cout << "Готово. Файл получен.\n";
    return 0;
}

int cwd(int s, char *dir)
{
	char str[512];

    sprintf(str,"CWD %s\r\n",dir);
    send(s,str,strlen(str),0);

	char size[512] = {' '};
    recv(s,size,512,0);

    cout << "\nИнформация от сервера: \n" << size << endl;
	return 0;
};

int quit(int s, int s2)
{
	char str[7] = {"QUIT\r\n"};
	char size[512] = {' '};
	cout << "Разрыв соединения с сервером...\n";
    send(s,str,strlen(str),0);
	recv(s,size,512,0);
    cout << size << endl;
	cout << "Соединение разорвано.\n";
	flag = 0;
	closesocket(s);
	closesocket(s2);
	return 0;
};

int start (char *a)
{
	char buff[17] = {' '};
	int ip1,ip2,ip3,ip4;

	while(1)
	{
	cout <<"FTP-клиент. Хуторной Ярослав. СПБПУ 2016."<<endl;
	cout << "Введите ip-адрес ftp-сервера" << endl;
	cin >> buff;

    sscanf(buff, "%d.%d.%d.%d",&ip1,&ip2,&ip3,&ip4);

	if (((ip1<256)&&(ip1>=0)) && ((ip2<256)&&(ip2>=0)) && ((ip3<256)&&(ip3>=0)) && ((ip4<256)&&(ip4>=0)))
	{
	sprintf(a,"%d.%d.%d.%d", ip1,ip2,ip3,ip4);

	cout << "IP-адрес сервера: " << a << endl;;
	cout << "Порт: 21"<<endl;
	break;
	}
	
	cout << "Неверный IP. Повторите.\n";
	Sleep(1000);
	system("cls");
	}

	return 0;
};

int mkd (int s, char *dir)
{
	char size[512] = {' '};
	char str[512];

    sprintf(str,"MKD %s\r\n",dir);
    send(s,str,strlen(str),0);

	recv(s,size,512,0);
	cout << "Информация от сервера:\n";

    cout << size << endl;

return 0;
}

int rmd (int s, char *dir)
{	
	char size[512] = {' '};
	char str[512];

    sprintf(str,"RMD %s\r\n",dir);
    send(s,str,strlen(str),0);

	recv(s,size,512,0);
	cout << "Информация от сервера:\n";

    cout << size << endl;

return 0;
}

int dele (int s, char *name)
{	
	char size[512] = {' '};
	char str[512];

    sprintf(str,"DELE %s\r\n",name);
    send(s,str,strlen(str),0);

	recv(s,size,512,0);
	cout << "Информация от сервера:\n";

    cout << size << endl;

return 0;
}

int stor(int s, int s2, char *file)
{
    char str[512];
	char *tmp_char;

    sprintf(str,"STOR %s\r\n",file);
    send(s,str,strlen(str),0);

	char size[512] = {' '};
    recv(s,size,512,0);
    cout << "\nИнформация от сервера: \n" << size << endl;

	int a;
	tmp_char = strtok(size," ");
    sscanf(tmp_char, "%d",&a);
	if (a/100 == 5)
	{
		closesocket(s2);
		return 0;
	}

    FILE *f;
    f = fopen(file, "rb");   

    if (f == NULL)
    {
    fputs("Ошибка файла", stderr);
    exit(1);
    }

	fseek(f , 0 , SEEK_END);                         
    long lSize = ftell(f);                           
    rewind (f); 

	int writed;
	int summ;
 
	summ = 0;
	while(summ<lSize)
	{
	char buff[1];  
	writed = fread(buff,1,1,f);
	send(s,buff,strlen(buff),0);
	summ = summ + writed;
	cout << "Передано: " << writed << " Байт\n";	
	}

    fclose(f);

	recv(s,size,512,0);
    cout <<"\n"<< size << endl;
    cout << "Готово. Файл передан.\n";

return 0;
}

void main ()
{
	setlocale(LC_CTYPE, "RUS");

	int sok_info;
	int sok_data;

	char ip[18];
	char vibor; 
	char vvod[128];

	cout <<"Адреса серверов для теста: \n";
	cout <<"193.166.3.2"<<endl;
	cout <<"89.108.120.88"<<endl<<endl;

	start(ip);

	while(1)
	{

	cout << "\nВыберите действие: \n";
		

	switch(flag)
	{
	case 1:
	{
	cout << "1) Перейти в другой каталог\n";
	cout << "2) Отобразить содержимое текущего каталога\n";
	cout << "3) Отобразить краткое содержимое текущего каталога\n";
	cout << "4) Скачать файл\n";
	cout << "5) Передать файл\n";	
	cout << "6) Удалить файл\n";
	cout << "7) Создать свой каталог\n";
	cout << "8) Удалить каталог\n";
	cout << "9) Разорвать соединение с сервером\n";
	break;
	}
	case 0:
	{
	cout << "1) Подключиться к серверу\n";
	break;
	}
	}

	cin >> vibor;

	switch(vibor)
	{
	case '1' :
	{
	if (flag == 0)
	{
	sok_info = init_sock(ip);
	readServ(sok_info);
	login(sok_info);	
	break;
	}
	if (flag == 1)
	{
	cout << "Введите имя каталога для перехода: ";
	cin >> vvod;
	cwd(sok_info, vvod);		
	break;
	}
	}
	case '4' :
	{
	if (flag == 1)
	{
	cout << "Введите имя файла, который нужно загрузить: ";
	cin >> vvod;
	sok_data = init_data(sok_info,ip);
	retr(sok_info, sok_data, vvod);	
	break;
	}	
	}
	case '9' :
	{
	if (flag == 1)
	{
	quit(sok_info, sok_data);	
	break;
	}
	}
	case '2' :
	{
	if (flag == 1)
	{
	sok_data = init_data(sok_info,ip);
	list(sok_info,sok_data);		
	break;
	}
	}
	case '3' :
	{
	if (flag == 1)
	{
	sok_data = init_data(sok_info,ip);
	nlst(sok_info,sok_data);
	break;
	}
	}
	case '7' :
	{
	if (flag == 1)
	{
	cout << "Введите имя каталога, который хотите создать: ";
	cin >> vvod;
	mkd (sok_info, vvod);
	break;
	}
	}
	case '8' :
	{
	if (flag == 1)
	{
	cout << "Введите имя каталога, который хотите удалить: ";
	cin >> vvod;
	rmd (sok_info, vvod);
	break;
	}
	}
	case '6' :
	{
	if (flag == 1)
	{
	cout << "Введите имя файла, который хотите удалить: ";
	cin >> vvod;
	dele(sok_info, vvod);
	break;
	}
	}
	case '5' :
	{
	if (flag == 1)
	{
	cout << "Введите имя файла, который хотите передать: ";
	cin >> vvod;
	stor(sok_info, sok_data, vvod);
	break;
	}
	}

	default:
	{
	 cout << "В списке нет такого действия/n";	
	}

	}

	}
};