#include"client.hpp"

void actions()
{
	char getWork[5];//用字符串代替int防止输入错误
	int actions;
	while(1)
	{	
		firstMenu();
		printf("输入操作:");
		gets(getWork);
		actions = atoi(getWork);
		switch(actions)
		{
			case 1://注册
				registerNewAccount();				
				break;					
			case 2://登入
				if(enterAccount() == 1)					
				{
					bossFlag = 0;
					pthread_t pId;				//启用线程
					pthread_create(&pId,NULL,recvThread,(void *)clientSocket);
					pthread_detach(pId);
					secondMenuAndAction();					
				}
				break;	
			case 3:
				updatePassword();					//修改密码
				break;
			case 4:
				updateName();					//修改密码
				break;
			case 5:
				retrieveAccountPassword();
				break;
			case 6:
				linkOffline();
				exit(0);							//退出
				break;
		}
		memset(getWork,0,strlen(getWork));
	}
}

int main(int argc,char *argv[])
{
	if(argc != 2)
	{
		printf("请加上IP地址\n");
		exit(-1);
	}
	else
	{
		if(ifIpError(argv[1]))				//IP地址是否合法的检测
		{
			printf("IP地址错误\n");
			exit(-1);
		}
		printf("IP = %s\n",argv[1]);
		strcpy(IP,argv[1]);
		printf("IP =  %s\n",IP);
	}
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == -1)
	{
		perror ("socket");
		return -1;
	}
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family  = AF_INET;     // 设置地址族
	addr.sin_port    = htons(PORT); // 端口号转换为网络字节序
	inet_aton(IP,&(addr.sin_addr));	 //addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	int ret = connect(clientSocket, (struct sockaddr *)&addr, sizeof(addr));
	if (ret == -1)
	{
		perror ("connect errer");
		return -1;
	}
	printf("成功连上服务器\n");
	actions();
	close(clientSocket);
	return 0;	
}
