#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sqlite3.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

class MsgData
{
public:
	int work;					//cli工作指令
	int flag;					//回执flag
	char mess[1024];				//消息
	char account[30];			//用户登入账号
	char name[30];				//用户昵称
	char pass[30];				//用户密码
	char online[30];				//在线情况
	char time[30];
	int root;					//root权限？	
	int jinyan;					//禁言flag？
	char toName[30];			
	char fromName[30];
	char fileName[30];
	char question[256];			//密保问题
	char answer[256];			//密保答案
};

class MyLink				//聊用聊表
{
public:
	char name[30];
	int copyClientSocket;			//拷贝的cli套接字
	int isInChat;				//在群聊否  1在0不在,是on chat??
	int state;					//主动状态，融合私聊
	/*
	 *	所有int融合成一个state
	 *	0；正常状态
	 *	1；请勿打扰（禁止私聊）
	 */
	MyLink *next;
};

char IP[15];					
short PORT;
MyLink *pH;//在线链表

sqlite3 *db = NULL;//数据库
char *errmsg = NULL;//错误集
char **result = NULL;//查询结果

void createTable();//建表
void registerNewAccount(int clientSocket,MsgData *msg);//注册
void* serviceThread(void* _clientSocket);
void offLink(int ClientSocket,MsgData *msg);
MyLink * initLink();
int ifIpError(char *ip);
void enterAccount(int clientSocket,MsgData *msg);
int displayAccount();
int checkPassword(int clientSocket,MsgData *msg);
int checkOnlineAccount(MsgData *msg);
void chatAll(int clientSocket,MsgData *msg);
void chatOne(int clientSocket,MsgData *msg);
void getTheManState(int clientSocket,MsgData *msg);
void linkDeleteNode(int clientSocket,MsgData *msg);
int linkInsertOnlinePeople(int clientSocket,MsgData *msg);
int initSocket();
int myAccept(int serverSocket);
void linkDisplayInchatPeople();
void lookPeopleInChat(int clientSocket,MsgData *msg);
void isRoot(int clientSocket,MsgData *msg);
void makeSlient(int clientSocket,MsgData *msg);
void releaseSlient(int clientSocket,MsgData *msg);
void updatePassword(int clientSocket,MsgData *msg);
void updateName(int clientSocket,MsgData *msg);
void sendFile(int clientSocket,MsgData *msg);
void linkOffline(int clientSocket);
void setRoot(int clientSocket,MsgData *msg);
void removeRoot(int clientSocket,MsgData *msg);
void findTheManAndGetQuestion(int clientSocket,MsgData *msg);
void retrieveAccountPassword(int clientSocket,MsgData *msg);
void getOut(int clientSocket,MsgData *msg);
void informOutPeople(MsgData *msg);
void* serviceThread(void* _clientSocket);
void lookSlientPeople(int clientSocket,MsgData *msg);



MyLink * initLink() //初始化链表
{
	MyLink *p = (MyLink *)malloc(sizeof(MyLink));
	if(p == NULL)
	{
		printf("ERRER\n");
		exit(-1);
	}
	p->next = NULL;
	return p;
}

void linkDeleteNode(int clientSocket,MsgData *msg)  //人员从链表下线
{
	MyLink *p = pH->next;
	MyLink *pR = pH;
	while(p != NULL)
	{
		if(p->copyClientSocket == clientSocket)
		{
			pR->next = p->next;
			free(p);
			return;
		}
		pR = pR->next;
		p = pR->next;
	}	
}


int ifIpError(char *ip)
{
	int i = 0;
	int j = 0;
	int flagi = 0;
	int last = 0;								//ip每一段的初始位置下标
	char temp[50];								//存放ip的每段
	int ipTemp = 0;								//每段ip转换为int型用来判断
	int tempi = 0;
	int num = 0;
	while(ip[i] != 0)
	{
		if(ip[i] == ' ')							//输入空格的错误
		{
			return 1;
		}
		if(ip[i] == '.' || ip[i + 1] == 0)
		{
			flagi = i;
			if(ip[i + 1] == 0)					//能够判断最后一段
			{
				flagi++;
			}	
			num++;								//计算.的数量，正确的应有3
			for(j = last; j < flagi; j++)
			{
				temp[tempi++] = ip[j];			//每段复制给temp
			}
			temp[tempi] = 0;
			last = flagi + 1;						//保存每次.后ip段的开始位置
			tempi = 0;
			ipTemp = atoi(temp);					//temp转为int
			if(!(ipTemp >= 0 && ipTemp <= 255))
			{
				return 1;						//超出范围直接返回
			}			
		}		
		i++;
	}
	printf("2\n");
	if(num != 4)
	{
		//printf("3\n");
		return 1;
	}
	return 0;
}

int checkPassword(int clientSocket,MsgData *msg)    //检验密码
{
	char buf[200];
	char tempPassword[30];
	int nrow = 0;  
	int ncol = 0;
	int rc = sqlite3_open("test.db", &db);
	if(rc)
	{
		printf("can't open db!\n");
		return -1;
	}
	else
	{
		printf("open db success!\n");
	}
	sprintf(buf,"select password from info where account = '%s'",msg->account);
	if(SQLITE_OK != sqlite3_get_table(db, buf, &result, &nrow, &ncol, &errmsg))
	{
		printf("view error\n");
		sqlite3_close(db);	
		return -1;
	}
	strcpy(tempPassword,result[ncol]);
	sqlite3_free_table(result); 
	memset(buf,0,sizeof(buf));
	if(strcmp(msg->pass,tempPassword) == 0)
	{
		return 1;		//正确返回1
	}
	else
	{
		return 0;
	}	
}


int initSocket()
{
	PORT = 7777;
	int fdSocket = socket(AF_INET,SOCK_STREAM,0);  //采用TCP连接
	if (fdSocket == -1)
	{
		perror("创建socket失败");
		exit(-1);
	}
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(IP); //addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	if (bind(fdSocket,(struct sockaddr *)&addr,sizeof(addr)) == -1)
	{
		printf("IP = %s\n",IP);
		perror("绑定失败");
		exit(-1);
	}
	if (listen(fdSocket,100) == -1)
	{
		perror("设置监听失败");
		exit(-1);
	}
	printf("等待客户端的连接..........\n");
	return fdSocket;
}


int myAccept(int serverSocket)        //返回链接上的套接字
{
	struct sockaddr_in client_addr;// 用来保存客户端的ip和端口信息
	int len = sizeof(client_addr);
	int clientSocket = accept(serverSocket,(struct sockaddr *)&client_addr,  (socklen_t*)&len);
	if (clientSocket == -1)
	{
		perror ("accept");
	}
	printf ("成功接收一个客户端: %s\n", inet_ntoa(client_addr.sin_addr));
	return clientSocket;
}

int displayAccount()//数据库校验并且打印出来
{  
	int i = 0;  
	int nrow = 0;  
	int ncol = 0;  
	int rc = sqlite3_open("test.db", &db);
	if(rc)
	{
		printf("can't open database!\n");
	}
	else
	{
		printf("open database success!\n");
	}	
	const char *sql = "select * from info;";   
	
	//判断sqlite3_get_table是否运用成功，成功返回SQLITE_OK  
	if(SQLITE_OK != sqlite3_get_table(db, sql, &result, &nrow, &ncol, &errmsg))
	{  
		printf("fail:%s\n",errmsg);  
		printf("\n");  
		exit(-1);  
	}  
	for(i = 0; i < (nrow + 1) * ncol; i++)//将表中的数据打印出来  
	{  
		printf("%-12s",result[i]);  
		if((i + 1) % ncol == 0)  
		{  
			printf("\n");  
		}  
	}
	sqlite3_free_table(result);//释放result  
	sqlite3_close(db);
}

void registerNewAccount(int clientSocket, MsgData *msg)    //注册插入数据库
{
	char buf[200];
	int nrow = 0;											//行
	int ncol = 0;											//列
	char tempName[30];
	int i = 0;
	int j = 0;
	int rc = sqlite3_open("test.db", &db);
	if(rc)
	{
		printf("can't open data!\n");
		exit(-1);
	}
	else
	{
		printf("open data success!\n");
	}
	createTable();										
	sprintf(buf,"select name from info where name = '%s'",msg->name);   //检查昵称重复注册
	if(SQLITE_OK != sqlite3_get_table(db, buf, &result, &nrow, &ncol, &errmsg))
	{
		printf("view error%s\n",errmsg);
		return;
	}
	memset(buf,0,sizeof(buf));							//用完就初始化吧
	if(ncol == 0)//没查到结果表示无此账号，可以注册
	{
		sqlite3_free_table(result);
		sprintf(buf, "insert into info(account,name,password,root,slient,question,answer) \
				values ('%s','%s','%s','%d','%d','%s','%s');",\
				msg->account,msg->name,msg->pass,msg->root,0,msg->question,msg->answer);
		int ret = sqlite3_exec(db, buf, NULL, NULL, &errmsg);
		if (ret != SQLITE_OK)
		{
			printf("错误原因%s\n",errmsg);
			msg->flag = 0;
		}
		else
		{
			msg->flag = 1;//成功	
			displayAccount();	//终端打印校验，
		}
	}
	else
	{
		printf("ncol = %d\n",ncol);
		printf("result[ncol] = %s\n",result[ncol]);
		strcpy(tempName,result[ncol]);
		sqlite3_free_table(result);
		if(strcmp(result[ncol],msg->name) == 0)
		{
			msg->flag = 3;//重名了	
		}	
	}
	send(clientSocket,msg,sizeof(MsgData),0);
	sqlite3_close(db);
}


void createTable()
{
	printf("createTable\n");
	const char *sql = "create table if not exists info(id integer primary key,account text,\
				 name text,password text,root text,slient text,question text,answer text);";  
	printf("sql = %s\n",sql);
	if(SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &errmsg))//判断是否成功成功返回SQLITE_OK  
	{  
		printf("失败原因:%s\n",errmsg);  
		printf("\n");  
		exit(-1);  
	}
}

int linkInsertOnlinePeople(int clientSocket,MsgData *msg) //链表插入人员(在enterAccount函数中)
{
	MyLink *p = pH;
	MyLink *pNew = (MyLink *)malloc(sizeof(MyLink));
	if(pNew == NULL)
	{
		printf("malloc ERRER\n");
		return -1;
	}
	strcpy(pNew->name,msg->name);
	pNew->copyClientSocket = clientSocket;
	pNew->isInChat = 0;
	pNew->state = 0;
	pNew->next = p->next;
	p->next = pNew;
	return 1;
}

int checkOnlineAccount(MsgData *msg)//检查是否重复登入
{	
	char buf[200];
	char tempName[30];
	int nrow = 0;  
	int ncol = 0;
	int rc = sqlite3_open("test.db", &db);
	if(rc)
	{
		printf("can't open db!\n");
		return -1;
	}

	sprintf(buf,"select name from info where account = '%s'",msg->account); //根据账号得到昵称
	if(SQLITE_OK != sqlite3_get_table( db, buf , &result, &nrow, &ncol, &errmsg))
	{
		printf("view error\n");
		sqlite3_close(db);
		return -1;
	}
	if(ncol == 0)
	{	
		printf("无此用户\n");
		return 3;
	}
	strcpy(tempName,result[ncol]);
	sqlite3_free_table(result); 
	MyLink *p = pH;	
	while(p->next != NULL)//由昵称在在线链表中查询是否在线
	{
		p = p->next;
		if(strcmp(p->name,tempName) == 0)//检查链表,只要登入了就在链表中
		{
			return 0;	//已在线		//修改昵称的话，链表中的name也要修改
		}
	}
	sqlite3_close(db);
	return 1;							//此人不在线

}

void enterAccount(int clientSocket,MsgData *msg)//登入
{
	char buf[200];
	int nrow = 0;  
	int ncol = 0;
	int rc = sqlite3_open("test.db", &db);
	if(rc)
	{
		printf("can't open db!\n");
		msg->flag = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		return;
	}
	int mFlag = checkOnlineAccount(msg);//重复在线查询
	if(mFlag == 0)//此人已在线
	{
		msg->flag = 2;//已登入
	}
	else if(mFlag == -1)//意外情况
	{
		msg->flag = -1;
	}
	else if(mFlag == 3)//此用户不存在
	{
		msg->flag = 5;
	}
	else if(mFlag == 1)//此人不在线
	{
		if(checkPassword(clientSocket,msg) == 1)//密码检验函数 == 1 密码正确
		{
			sprintf(buf,"select name from info where account = '%s'",msg->account);//根据账号将昵称找出来
			if(SQLITE_OK != sqlite3_get_table(db, buf, &result, &nrow, &ncol, &errmsg))
			{
				printf("view error\n");
				msg->flag = -1;					//意外情况		 = -1,cli要printf说发生意外
			}
			else
			{
				strcpy(msg->name,result[ncol]);	
				sqlite3_free_table(result);
				if(linkInsertOnlinePeople(clientSocket,msg) == -1)//插入在线链表
				{

					msg->flag = -1;  //增加malloc失败的提示
				}
				else
				{
					printf("登入成功\n");	
					msg->flag = 1;					//正确flag	
				}
			}
		}
		else									
		{
			msg->flag = 3;//密码错误
		}
	}	
	send(clientSocket,msg,sizeof(MsgData),0); 
	sqlite3_close(db);	
}

void offLink(int ClientSocket,MsgData *msg)   //退出群聊的处理(chatAll的子函数)
{
	MyLink *p = pH->next;
	while(p != NULL)
	{
		if(p->copyClientSocket == ClientSocket)
		{
			p->isInChat = 0;//仅退出群聊，仍在链表中就是在线，还可以重新进入群聊
		}
		p = p->next;
	}
}


void chatAll(int clientSocket,MsgData *msg)				//群聊操作	
{
	MyLink *p = pH->next;
	MyLink *pP = pH->next;
	MyLink *pJ = pH->next;

	if(strcmp(msg->mess,"bye") == 0)
	{	
		offLink(clientSocket,msg);
		return;
	}
	else
	{
		while(pP != NULL)
		{
			if(pP->copyClientSocket == clientSocket)
			{
				pP->isInChat = 1;				//1在群聊中
			}
			pP = pP->next;
		}
	}
	//被动状态放数据库中，主动状态放链表中
	//基于数据库的禁言		//如果基于链表，那么这个不在线就禁不了言了
	int nrow = 0;
	int ncol = 0;
	int rc = sqlite3_open("test.db", &db);
	if(rc)
	{
		printf("can't open db!\n");
		msg->work = -1;
	}
	char buf[200];

	sprintf(buf,"select slient from info where name = '%s'",msg->name);//判断是否被禁言，slient被动状态
	if(SQLITE_OK != sqlite3_get_table(db, buf, &result, &nrow, &ncol, &errmsg))
	{
		printf("view error\n");
		msg->work = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		return;
	}
	else
	{
		int temp = atoi(result[ncol]);
		sqlite3_free_table(result);
		if(temp == 1 && msg->flag != 1 && strcmp(msg->mess,"退出了群聊\n") != 0)	//==1被禁言,flag == 1,是欢迎语句
		{
			MsgData tempMsg;	
			tempMsg.work = 8;					//8是禁言用户
			send(clientSocket,&tempMsg,sizeof(MsgData),0);
			return;
		}
		//群聊操作
		while(p != NULL)
		{	
			if(p->isInChat == 1)
			{
				send(p->copyClientSocket,msg,sizeof(MsgData),0);//发送给所有在群聊的人;
				printf("在聊天室的人 %s\n",p->name);
			}
			p = p->next;
		}
		printf("\n");
	}
}


void getTheManState(int clientSocket,MsgData *msg)//私聊检验
{
	MyLink *p = pH;
	int flag = 0;	
	while(p->next != NULL)
	{
		p = p->next;
		printf("checkName = %s\n",p->name);
		if((strcmp(p->name,msg->toName) == 0) && p->isInChat == 1)
		{
			flag = 1;
			break;
		}		
	}
	if(flag == 0)
	{
		msg->work = 4;
		send(clientSocket,msg,sizeof(MsgData),0);
	}
}

//如果对方在私聊时下线，必须通知并且关闭私聊返回群聊
void chatOne(int serverSocket, MsgData *msg)//私聊
{
	MyLink *p = pH;
	int flag = 0;
	printf("聊天室中用户如下：\n");
	linkDisplayInchatPeople();
	while(p->next != NULL)
	{
		p = p->next;
		if((strcmp(p->name,msg->toName) == 0) && p->isInChat == 1)
		{
			flag = 1;
			send(p->copyClientSocket,msg,sizeof(MsgData),0);
			break;
		}	
	}
	if(flag == 0)
	{
		msg->work = 4;
		send(serverSocket,msg,sizeof(MsgData),0);
	}
}

void linkDisplayInchatPeople()//打印在群聊中的人
{
	int num = 0;
	MyLink *p = pH->next;
	while(p != NULL)
	{
		if(p->isInChat == 1)
		{
			num++;
			printf("：%s\n",p->name);
		}
		p = p->next;
	}
}

void lookPeopleInChat(int clientSocket,MsgData *msg)//用户查看在群聊中的用户
{
	MyLink *p = pH;
	while(p->next != NULL)
	{
		p = p->next;
		if(p->isInChat == 1)
		{
			strcpy(msg->name,p->name);
			send(clientSocket,msg,sizeof(MsgData),0);
		}
	}
}
void isRoot(int clientSocket,MsgData *msg)   //root权限判断
{
	int nrow = 0;
	int ncol = 0;
	int rc = sqlite3_open("test.db", &db);
	if(rc)
	{
		printf("can't open db!\n");
		msg->flag = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		return;
	}
	char buf[200];
	sprintf(buf,"select root from info where account = '%s'",msg->account);
	if(SQLITE_OK != sqlite3_get_table(db, buf, &result, &nrow, &ncol, &errmsg))
	{
		printf("view error\n");
		msg->flag = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		return;
	}
	printf("result[ncol] = %s\n",result[ncol]);
	int temp = atoi(result[ncol]);
	if(temp == 1)
	{
		msg->flag = 1;			//1是root		
	}
	else
	{
		msg->flag = 0;			//0不是root
	}
	send(clientSocket,msg,sizeof(MsgData),0);
	sqlite3_free_table(result);
	sqlite3_close(db);
}


void makeSlient(int clientSocket,MsgData *msg)//禁言操作，基于数据库实现
{
	int nrow = 0;
	int ncol = 0;
	char tempName[30];
	int rc = sqlite3_open("test.db", &db);
	if(rc)
	{
		printf("can't open db!\n");
		msg->work = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		return;
	}
	char buf[200];
	sprintf(buf,"select name from info where name = '%s'",msg->toName);
	if(SQLITE_OK != sqlite3_get_table(db, buf, &result, &nrow, &ncol, &errmsg))
	{
		printf("view error\n");
		msg->work = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	strcpy(tempName,result[ncol]);
	sqlite3_free_table(result);
	printf("tempName = %s\n",tempName);	
	if(strcmp(tempName,msg->toName) != 0)
	{
		msg->work = 9;
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	memset(buf,0,sizeof(buf));
	sprintf (buf, "update info set slient = '%d' where name = '%s'", 1, msg->toName);
	if(SQLITE_OK != sqlite3_get_table( db, buf , &result, &nrow, &ncol, &errmsg))
	{
		printf("view error\n");
		msg->work = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	MyLink *p = pH->next;
	while(p != NULL)
	{
		if(strcmp(p->name,msg->toName) == 0 && p->isInChat == 1)
		{
			msg->work = 8;
			printf("p->name = %s\n",p->name);
			send(p->copyClientSocket,msg,sizeof(MsgData),0);
			break;
		}
		p = p->next;
	}
	msg->work = 10;
	send(clientSocket,msg,sizeof(MsgData),0);
	sqlite3_free_table(result);
	sqlite3_close(db);
}



void releaseSlient(int clientSocket,MsgData *msg)			//基于数据库的解除禁言操作
{
	int nrow = 0;
	int ncol = 0;
	int i = 0;
	int j = 0;
	char tempName[30];
	int rc = sqlite3_open("test.db", &db);
	if(rc)
	{
		printf("can't open db!\n");
		msg->work = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		return;
	}
	char buf[200];
	sprintf(buf,"select name from info where name = '%s'",msg->toName);
	if(SQLITE_OK != sqlite3_get_table(db, buf , &result, &nrow, &ncol, &errmsg))
	{
		printf("view error\n");
		msg->work = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	strcpy(tempName,result[ncol]);
	sqlite3_free_table(result);
	if(strcmp(tempName,msg->toName) != 0)
	{
		msg->work = 9;
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	memset(buf,0,sizeof(buf));
	sprintf (buf, "update info set slient = '%d' where name = '%s'", 0, msg->toName);
	if(SQLITE_OK != sqlite3_get_table(db,buf,&result,&nrow,&ncol,&errmsg))
	{
		printf("view error\n");
		msg->work = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	msg->work = 11;
	send(clientSocket,msg,sizeof(MsgData),0);
	MyLink *p = pH;
	while(p->next != NULL)//通知
	{
		p = p->next;
		if(strcmp(p->name,msg->toName) == 0)
		{
			msg->work = 19;
			send(p->copyClientSocket,msg,sizeof(MsgData),0);
			break;
		}
	}
	sqlite3_free_table(result);
	sqlite3_close(db);
}



void updatePassword(int clientSocket,MsgData *msg)   //修改密码
{
	if(checkPassword(clientSocket,msg) == 1)
	{
		msg->flag = 1;	
	}
	else
	{
		msg->flag = 0;
	}
	send(clientSocket,msg,sizeof(MsgData),0);	
	recv(clientSocket,msg,sizeof(MsgData),0);	
	if(msg->flag == 3)
	{
		int rc = sqlite3_open("test.db", &db);
		if(rc)
		{
			printf("can't open db!\n");
			msg->flag = -1;
		}
		else
		{
			msg->flag = 2;
			printf("open db success!\n");
			char buf[200];
			sprintf (buf, "update info set password = '%s' where account = '%s'", msg->pass, msg->account);
			rc = sqlite3_exec(db, buf, NULL, NULL, &errmsg);
			if (rc != SQLITE_OK)
			{
				printf ("数据库操作失败：%s\n", errmsg);
				sqlite3_close(db);
				msg->flag = -1;
			}
		}
		send(clientSocket,msg,sizeof(MsgData),0);	
	}
	else if(msg->flag == 2)
	{
		sqlite3_close(db);
		return;
	}
}


void updateName(int clientSocket,MsgData *msg)  //修改昵称
{
	if(checkPassword(clientSocket,msg) == 1)
	{
		msg->flag = 1;	
	}
	else
	{
		msg->flag = 0;
	}
	send(clientSocket,msg,sizeof(MsgData),0);
	recv(clientSocket,msg,sizeof(MsgData),0);
	if(msg->flag == 3)
	{
		int rc = sqlite3_open("test.db", &db);
		if(rc)
		{
			printf("can't open db!\n");
			msg->flag = -1;
		}
		else
		{
			msg->flag = 2;
			printf("open db success!\n");
			char buf[200];
			sprintf (buf, "update info set name = '%s' where account = '%s'", msg->name, msg->account);
			rc = sqlite3_exec(db, buf, NULL, NULL, &errmsg);
			if (rc != SQLITE_OK)
			{
				printf ("数据库操作失败：%s\n", errmsg);
				sqlite3_close(db);
				msg->flag = -1;
			}
		}
		send(clientSocket,msg,sizeof(MsgData),0);	
	}
	else if(msg->flag == 2)
	{
		sqlite3_close(db);
		return;
	}
}



void sendFile(int clientSocket,MsgData *msg) //发送文件，但是只能私聊可以发送，群聊不行
{
	int flag = 0;	
	char buf[1024] = {0};
	strcpy(buf, msg->mess);	
	MyLink *p = pH;
	while(p->next != NULL)				//找toname是否在群聊；
	{
		p = p->next;
		if((strcmp(p->name,msg->toName) == 0) && (p->isInChat == 1))
		{
			flag = 1;
			msg->work = 15;
			send(p->copyClientSocket,msg,sizeof(MsgData),0);	
		}
	}
	if(flag == 1)
	{
		msg->work = 12;
		send(clientSocket,msg,sizeof(MsgData),0);	
	}
	else//不在群聊不能发送文件
	{
		msg->work = 13;
		send(clientSocket,msg,sizeof(MsgData),0);
	}
}

void linkOffline(int clientSocket)  //下线
{
	int flog = 0;
	MyLink *p = pH;
	MyLink *pR = NULL;
	while(p->next!=NULL)
	{
		pR = p;
		p = p->next;
		if(p->copyClientSocket == clientSocket)
		{
			pR->next = p->next;
			printf("%s已下线！\n",p->name);
			flog = 1;
			free(p);
		}
	}
	if(flog == 0)//不会发生
	{
		printf("查无此人\n");
	}
}

void setRoot(int clientSocket,MsgData *msg)		//设置管理员权限
{
	int nrow = 0;
	int ncol = 0;
	char tempName[30];
	int rc = sqlite3_open("test.db", &db);
	if(rc)
	{
		printf("can't open db!\n");
		msg->work = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		return;
	}
	else
	{
		printf("open db success!\n");
	}
	char buf[200];
	sprintf(buf,"select name from info where name = '%s'",msg->name);
	if(SQLITE_OK != sqlite3_get_table( db, buf, &result, &nrow, &ncol, &errmsg))
	{
		printf("view error\n");
		msg->work = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	strcpy(tempName,result[ncol]);
	sqlite3_free_table(result);
	if(strcmp(tempName,msg->name) != 0)//判断此人是否存在
	{
		msg->work = 9;
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	memset(buf,0,sizeof(buf));
	sprintf (buf, "update info set root = '%d' where name = '%s'", 1, msg->name);
	rc = sqlite3_exec(db, buf, NULL, NULL, &errmsg);
	if (rc != SQLITE_OK)
	{
		printf ("数据库操作失败：%s\n", errmsg);
		msg->work = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	MyLink *p = pH->next;
	while(p != NULL)
	{
		if(strcmp(p->name,msg->toName) == 0 && p->isInChat == 1)
		{
			msg->work = 24;
			printf("p->name = %s\n",p->name);
			send(p->copyClientSocket,msg,sizeof(MsgData),0);
			break;
		}
		p = p->next;
	}
	msg->work = 18;
	send(clientSocket,msg,sizeof(MsgData),0);
	sqlite3_close(db);	
}

void removeRoot(int clientSocket,MsgData *msg)		//移除管理员权限
{
	int nrow = 0;
	int ncol = 0;
	char tempName[30];
	int rc = sqlite3_open("test.db", &db);
	if(rc)
	{
		printf("can't open db!\n");
		msg->work = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		return;
	}
	else
	{
		printf("open db success!\n");
	}
	char buf[200];
	sprintf(buf,"select name from info where name = '%s'",msg->name);
	if(SQLITE_OK != sqlite3_get_table( db, buf, &result, &nrow, &ncol, &errmsg))
	{
		printf("view error\n");
		msg->work = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	strcpy(tempName,result[ncol]);
	sqlite3_free_table(result);
	if(strcmp(tempName,msg->name) != 0)//判断此人是否存在
	{
		msg->work = 9;
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	memset(buf,0,sizeof(buf));
	sprintf (buf, "update info set root = '%d' where name = '%s'", 0, msg->name);
	rc = sqlite3_exec(db, buf, NULL, NULL, &errmsg);
	if (rc != SQLITE_OK)
	{
		printf ("数据库操作失败：%s\n", errmsg);
		msg->work = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	msg->work = 18;
	send(clientSocket,msg,sizeof(MsgData),0);
	sqlite3_close(db);	
}

void findTheManAndGetQuestion(int clientSocket,MsgData *msg)  //查找是否有此人并把问题发送cli
{	
	char buf[200];
	char tempName[30];
	int nrow = 0;
	int ncol = 0;
	int rc = sqlite3_open("test.db", &db);
	if(rc)
	{
		printf("can't open db!\n");
		msg->flag = -1;	
		send(clientSocket,msg,sizeof(MsgData),0);
		return;
	}
	else
	{
		printf("open db success!\n");
	}
	sprintf(buf,"select account from info where name = '%s'",msg->name);
	if(SQLITE_OK != sqlite3_get_table( db, buf , &result, &nrow, &ncol, &errmsg))
	{
		printf("view error\n");
		msg->flag = -1;	
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	strcpy(tempName,result[(nrow + 1)*ncol] - 1);
	sqlite3_free_table(result); 
	memset(buf,0,sizeof(buf));

	if(strcmp(tempName,msg->name) == 0)
	{
		msg->flag = 1;
		sprintf(buf,"select question from info where name = '%s'",msg->name);
		if(SQLITE_OK != sqlite3_get_table(db, buf ,&result, &nrow, &ncol, &errmsg))
		{
			printf("view error\n");
			msg->flag = -1;	
			send(clientSocket,msg,sizeof(MsgData),0);
			sqlite3_free_table(result); 
			sqlite3_close(db);
			return;
		}
		strcpy(msg->question,result[(nrow + 1)*ncol - 1]);
		sqlite3_free_table(result); 
		memset(buf,0,sizeof(buf));
	}
	else
	{
		msg->flag = 0;	
	}
	send(clientSocket,msg,sizeof(MsgData),0);
	sqlite3_close(db);
}


void retrieveAccountPassword(int clientSocket,MsgData *msg)	//查找账号密码（找回账号密码）
{
	char buf[200];
	char tempAnswer[1024];
	int nrow = 0;
	int ncol = 0;
	int rc = sqlite3_open("test.db", &db);		
	if(rc)
	{
		printf("can't open db!\n");
		msg->flag = -1;	//集中成一个函数
		send(clientSocket,msg,sizeof(MsgData),0);
		return;
	}
	else
	{
		printf("open db success!\n");
	}
	sprintf(buf,"select answer from info where question = '%s'",msg->question);
	if(SQLITE_OK != sqlite3_get_table(db, buf ,&result, &nrow, &ncol, &errmsg))
	{
		printf("view error\n");
		msg->flag = -1;	
		send(clientSocket,msg,sizeof(MsgData),0);
		sqlite3_close(db);
		return;
	}
	strcpy(tempAnswer,result[(nrow + 1)*ncol]);
	sqlite3_free_table(result); 
	memset(buf,0,sizeof(buf));
	if(strcmp(tempAnswer,msg->answer) == 0)
	{
		msg->flag = 1;
		sprintf(buf,"select account from info where question = '%s'",msg->question);//账号
		if(SQLITE_OK != sqlite3_get_table(db, buf , &result, &nrow, &ncol, &errmsg))
		{
			printf("view error\n");
			msg->flag = -1;	
			send(clientSocket,msg,sizeof(MsgData),0);
			sqlite3_free_table(result);		
			sqlite3_close(db);
			return;
		} 
		strcpy(msg->account,result[(nrow + 1)*ncol - 1]);
		sqlite3_free_table(result);		
		memset(buf,0,sizeof(buf));

		sprintf(buf,"select password from info where question = '%s'",msg->question);//密码
		if(SQLITE_OK != sqlite3_get_table(db, buf, &result, &nrow, &ncol, &errmsg))
		{
			printf("view error\n");
			msg->flag = -1;	
			send(clientSocket,msg,sizeof(MsgData),0);
			sqlite3_close(db);
			return;
		}
		strcpy(msg->pass,result[(nrow + 1)*ncol - 1]);
		sqlite3_free_table(result);		
		memset(buf,0,sizeof(buf));	
	}
	else
	{
		msg->flag = 0;
	}	
	send(clientSocket,msg,sizeof(MsgData),0);
	sqlite3_close(db);
}


void getOut(int clientSocket,MsgData *msg)  //踢人下线
{
	MyLink *p = pH;
	int flag = 0;
	while(p->next != NULL)
	{
		p = p->next;
		if(strcmp(p->name,msg->toName) == 0 && p->isInChat == 1)
		{
			flag = 1;			
		}
	}
	if(flag == 1)
	{
		msg->work = 18;
		informOutPeople(msg);
	}
	else
	{
		msg->work = 21;
	}
	send(clientSocket,msg,sizeof(MsgData),0);
	sqlite3_close(db);
}


void informOutPeople(MsgData *msg)  //通知被踢出的人（在getOut函数中）
{
	MyLink *p = pH;
	char buf[200];
	while(p->next != NULL)
	{
		p = p->next;
		if(strcmp(p->name,msg->toName) == 0)
		{
			msg->work = 20;
			strcpy(msg->mess,"你被管理员踢下线\n");
			linkOffline(p->copyClientSocket);
			send(p->copyClientSocket,msg,sizeof(MsgData),0);
		}
		if(p->isInChat == 1)
		{
			msg->work = 3;
			sprintf(buf,"%s被踢出群聊\n",msg->toName);
			strcpy(msg->mess,buf);
			send(p->copyClientSocket,msg,sizeof(MsgData),0);
		}
	}
}


void lookSlientPeople(int clientSocket,MsgData *msg) //查看被禁言人员
{
	char buf[200];
	int nrow = 0;  
	int ncol = 0;
	int rc = sqlite3_open("test.db", &db);
	if(rc)
	{
		printf("can't open db!\n");
		msg->flag = -1;
		send(clientSocket,msg,sizeof(MsgData),0);
		return;
	}
	sprintf(buf,"select name from info where slient = '%d'",1);//根据账号将昵称找出来
	if(SQLITE_OK != sqlite3_get_table(db, buf, &result, &nrow, &ncol, &errmsg))
	{
		printf("view error\n");
		sqlite3_close(db);	
		return;
	}
	if(ncol == 0)
	{
		msg->work = 23;
		send(clientSocket,msg,sizeof(MsgData),0);
		return;
	}
	int i = 0;
	int j = 0;
	for(i = ncol; i < (nrow + 1) * ncol; i++)
	{
		msg->work = 22;
		strcpy(msg->name, result[i]);
		printf("name = %s\n",msg->name);
		send(clientSocket,msg,sizeof(MsgData),0);
	}
	sqlite3_free_table(result);
}
