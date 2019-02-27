#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sqlite3.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>


class MsgData
{
public:
	int work;					//cli����ָ��
	int flag;					//��ִflag
	char mess[1024];			//��Ϣ
	char account[30];			//�û������˺�
	char name[30];				//�û��ǳ�
	char pass[30];				//�û�����
	char online[30];			//�������
	char time[30];				//��ȡʱ��
	int root;					//rootȨ��	
	int jinyan;					//����flag
	char toName[30];			//����˭
	char fromName[30];			//˭����
	char fileName[30];			//�ļ�
	char question[256];		    //�ܱ�����
	char answer[256];			//�ܱ���
};

class chatFile
{
public:
	char mess[1024];
	char fromName[30];
	char toName[30];
	char time[30];
	int flag;				    //�ж�˽�Ļ���Ⱥ�� 1˽�ģ�0Ⱥ��
};

char IP[15];				    //��������IP
short PORT = 7777;				//����������˿�
int clientSocket;
char myName[30];                //�ǳ�
char myAccount[30];				//ID
int isChatOneOnline;			//�ж��Ƿ�����
int slientFlag;					//���Ժ󲻱��汾�������¼
int rootFlag;					//�жϹ���Աflag
int bossFlag;					//�ж��û��Ƿ�Ҫ�˳���������
int noOneSlientFlag;			//�ж��Ƿ����˱����ԣ���=1


void registerNewAccount();//ע��
void saveGroupChat(MsgData *msg,int flag);
void chatAll();
char *getTime();
void firstMenu();
void chatManual();//�����ֲ�
void chatOne();
void lookOnlinePeople();
void isRoot();
void makeSlient();
void releaseSlient();
void thirdMenu();
void setRoot();
void removeRoot();
void getOut();
void rootActions();
void sendFile();
void viewLocalChat();
void linkOffline();
void secondMenuAndAction();
void anyKeyToNext();
int enterAccount();
char *getTime();
void* recvThread(void* _clientSocket);
void updateName();
void updatePassword();
void retrieveAccountPassword();
void actions();
void lookSlientPeople();
void saveFile(MsgData *msg);


void* recvThread(void* _clientSocket)				//һֱ���������
{
	int clientSocket = *(int*)_clientSocket;
	MsgData msg;
	while(1)
	{
		if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//ÿ�ζ��ж�
		{
			printf("�������Ͽ�����\n");
			exit(-1);
		}
		switch(msg.work)
		{
			case 3:
				printf("��Ⱥ�ģ�%s : %s\n",msg.name,msg.mess);
				saveGroupChat(&msg,1);
				break;	
			case 4:
				isChatOneOnline = 1;//û�ҵ�˽�ĵ��˻���������
				break;
			case 5: 
				if(isChatOneOnline != 1)
				{
					printf("��˽�ģ�%s͵͵����˵��%s\n",msg.fromName,msg.mess);
					saveGroupChat(&msg,0);
				}
				break;
			case 6:  //������Ա
				printf("%s\n",msg.name);
				break;			
			case 7: 
				if(msg.flag == 1)
				{
					rootFlag = 1;
					printf("����root�û����Ժ�\n");
				}
				else
				{
					printf("������root�û�\n���ڷ���Ⱥ��........\n");
				}
				break;
			case 8:
				slientFlag = 1;
				printf("���Ѿ���������\n");
				break;
			case 9:
				printf("����ʧ�ܣ��Է����ܲ����ڣ�������ѡ�����\n");
				break;
			case 10:
				printf("���� %s �ɹ�\n",msg.toName);
				break;
			case 11:
				printf("��� %s �Ľ��Գɹ�\n",msg.toName);
				break;
			case 12:
				printf("�ļ����ͳɹ�\n���ڷ���Ⱥ��........\n");
				break;
			case 13:
				printf("�ļ�����ʧ�ܣ����˲�Ⱥ���в��ܽ����ļ���\n���ڷ���Ⱥ��........\n");
				break;
			case 15:
				saveFile(&msg);
				printf("��%s�����յ�һ���ļ�\n",msg.fromName);
				break;
			case 16:
				bossFlag = 1;  //�˳���������
				break;
			case -1:
				printf("һЩ���벻���Ĵ�������\n");
				break;
			case 18:
				printf("���óɹ�\n");
				break;
			case 19:
				slientFlag = 0;
				printf("���ѱ��������\n");
				break;
			case 20:
				printf("%s\n",msg.mess);
				exit(0);
				break;
			case 21:
				printf("�Է������ڻ�����\n");
				break;
			case 22://��������Ա
				printf("%s\n",msg.name);
				break;
			case 23:
				noOneSlientFlag = 1;
				break;
			case 24:
				printf("�㱻��Ϊ����Ա\n");
				break;
			case 25:
				printf("�㱻�Ƴ�����Ա\n");
				break;
		}
		if(bossFlag == 1)
		{
			break;
		}
	}
}

int ifIpError(char *ip)
{
	int i = 0;
	int j = 0;
	int flagi = 0;
	int last = 0;								//ipÿһ�εĳ�ʼλ���±�
	char temp[50];								//���ip��ÿ��
	int ipTemp = 0;								//ÿ��ipת��Ϊint�������ж�
	int tempi = 0;
	int num = 0;
	while(ip[i] != 0)
	{
		if(ip[i] == ' ')							//����ո�Ĵ���
		{
			return 1;
		}
		if(ip[i] == '.' || ip[i + 1] == 0)
		{
			flagi = i;
			if(ip[i + 1] == 0)					//�ܹ��ж����һ��
			{
				flagi++;
			}	
			num++;								//����.����������ȷ��Ӧ��3
			for(j = last; j < flagi; j++)
			{
				temp[tempi++] = ip[j];			//ÿ�θ��Ƹ�temp
			}
			temp[tempi] = 0;
			last = flagi + 1;						//����ÿ��.��ip�εĿ�ʼλ��
			tempi = 0;
			ipTemp = atoi(temp);					//tempתΪint
			if(!(ipTemp >= 0 && ipTemp <= 255))
			{
				return 1;						//������Χֱ�ӷ���
			}			
		}		
		i++;
	}
	if(num != 4)
	{
		return 1;
	}
	return 0;
}

void firstMenu()
{
	printf("\n");
	printf("\t������demo\t\n");
	printf("\t*****************\n");
	printf("\t1.ע��\n");
	printf("\t2.����\n");
	printf("\t3.�޸�����\n");
	printf("\t4.�޸��ǳ�\n");
	printf("\t5.�һ�����\n");
	printf("\t6.�˳�\n");
	printf("\tȺ��������ע���ǳ�Ϊroot\n");
	printf("\t*****************\n");
}

void chatManual()  //Ⱥ���ֲ�
{
	system("clear");
	printf("********************************************\n");
	printf("\tȺ�Ĳ���ָ��\n");
	printf("q2������˽��ģʽ��˽��ģʽ����back����Ⱥ�ģ�\n");
	printf("q3���鿴������Ա\n");
	printf("q4���鿴���������¼\n");
	printf("q5���������Ա����\n");
	printf("q6�������ļ�\n");
	printf("Ⱥ��ģʽ������bye�˳�Ⱥ�ķ���������\n");
	printf("********************************************\n");
}

void chatOne()		//˽��
{
	MsgData msg;	
	char tempMess[1024];
	char oneName[30];
	lookOnlinePeople();
	sleep(1);
	while(1)
	{
		isChatOneOnline = 0;		//ÿ��while��isChatOneOnline����Ϊ0��
		msg.work = 4;
		printf("��������Ҫ˽�ĵ��ˣ�(back�˳�˽�ķ�Ⱥ��)\n");
		gets(oneName);
		if(strcmp(oneName,"back") == 0)
		{
			printf("���ڷ���Ⱥ��........\n");		
			return;
		}
		strcpy(msg.toName,oneName);
		strcpy(msg.fromName,myName);
		if(strcmp(msg.toName, msg.fromName) == 0)
		{
			printf("������Լ�˽�ģ���\n");
			sleep(1);
			continue;
		}
		printf("��������ȴ�........\n");
		send(clientSocket,&msg,sizeof(MsgData),0);
		sleep(1);		
		if(isChatOneOnline == 1)		//isChatOneOnline�����޴��˾Ͳ���ʼ˽��
		{
			printf("������û���ҵ�����\n");		
		}
		else
		{
			printf("�����Կ�ʼ˽����\n");
			break;		
		}
		memset(oneName,0,strlen(oneName));
	}
	msg.work = 5;//˽����5
	while(1)
	{
		gets(tempMess);
		strcpy(msg.mess,tempMess);
		strcpy(msg.time,getTime());
		if(strcmp(tempMess,"back") == 0)
		{
			printf("���ڷ���Ⱥ��........\n");		
			return;
		}
		else
		{
			if(isChatOneOnline == 1)
			{
				printf("�Է��Ѳ���Ⱥ��\n");
				break;
			}
			printf("��� %s ͵͵˵�ˣ�%s\n",msg.toName,msg.mess);		
			send(clientSocket,&msg,sizeof(MsgData),0);
		}
		memset(tempMess,0,sizeof(tempMess));
	}
}

void lookOnlinePeople()							//�鿴��Ⱥ���е���Ա
{
	MsgData msg;
	msg.work = 6;
	printf("������������\n");
	send(clientSocket,&msg,sizeof(MsgData),0);	
}

void isRoot()							//rootȨ���ж�
{
	MsgData msg;
	msg.work = 7;
	strcpy(msg.account,myAccount);	
	send(clientSocket,&msg,sizeof(MsgData),0);		
}

void makeSlient()
{	
	sleep(1);				//��Ϊsend�����̴߳������߻���������ߣ�����Ҫsleep
	MsgData msg;			//���߳��Ƕ������ꣻ
	msg.work = 8;
	char tempName[30];
	printf("�������������");
	gets(tempName);
	strcpy(msg.toName,tempName);
	send(clientSocket,&msg,sizeof(MsgData),0);
}

void releaseSlient()
{
	MsgData msg;
	msg.work = 10;
	char tempName[30];	
	printf("������������");
	gets(tempName);
	strcpy(msg.toName,tempName);
	send(clientSocket,&msg,sizeof(MsgData),0);
}

void thirdMenu()
{
	printf("	  root�û�����        \n");
	printf("\n");
	printf("1,����		2,�������\n");
	printf("3,���ù���Ա	4,�Ƴ�����Ա\n");
	printf("5,����		6,����Ⱥ��\n");
	printf("\n");
}

void setRoot()//���ù���ԱȨ��( Ⱥ��Ȩ��)
{
	MsgData msg;
	msg.work = 17;
	printf("��������Ҫ���ù�����˵��ǳƣ�");
	gets(msg.name);
	send(clientSocket,&msg,sizeof(MsgData),0);	
}

void removeRoot()
{
	MsgData msg;
	msg.work = 18;
	printf("��������Ҫ�Ƴ�������˵��ǳƣ�");
	gets(msg.name);
	send(clientSocket,&msg,sizeof(MsgData),0);	
}

void getOut()
{
	MsgData msg;
	msg.work = 21;	
	printf("����������");
	gets(msg.toName);
	send(clientSocket,&msg,sizeof(MsgData),0);	
}

void lookSlientPeople()//�鿴��������Ա
{
	MsgData msg;
	msg.work = 22;
	send(clientSocket,&msg,sizeof(MsgData),0);	
	printf("��������Ա����:\n");
}

void rootActions()
{
	char getWork[5];
	int actions;
	int flag = 0;
	while(1)
	{
		thirdMenu();
		printf("�������:");
		gets(getWork);
		actions = atoi(getWork);
		switch(actions)
		{
			case 1:
				lookOnlinePeople();
				printf("�ȴ�...\n");
				sleep(1);
				makeSlient();
				sleep(1);
				break;
			case 2:
				lookSlientPeople();
				noOneSlientFlag = 0;			
				sleep(1);
				if(noOneSlientFlag == 1)
				{
					printf("Ŀǰû���˱�����\n");
					break;
				}
				releaseSlient();
				sleep(1);
				break;
			case 3:
				lookOnlinePeople();
				printf("�ȴ�...\n");
				sleep(1);
				setRoot();
				sleep(1);
				break;
			case 4:
				lookOnlinePeople();
				printf("�ȴ�...\n");
				sleep(1);
				removeRoot();
				sleep(1);
				break;
			case 5:
				lookOnlinePeople();
				printf("�ȴ�...\n");
				sleep(1);
				getOut();
				sleep(1);
				break;
			case 6:
				flag = 1;
				printf("���ڷ���Ⱥ��\n");
				break;
		}
		if(flag == 1)
		{
			break;
		}
	}
}

void sendFile()
{
	MsgData msg;
	msg.work = 13;
	char buf[1024] = {0};
	char tempName[30];
	char txt[30];
	printf("������Ҫ���͵Ķ����ǳ�\n");
	gets(tempName);
	strcpy(msg.toName,tempName);
	printf("����Ҫ�����ļ�(�Ӻ�׺)\n");
	gets(txt);
	strcpy(msg.fileName,txt);
	sprintf(buf,"%s",txt);
	int pd = open(buf, O_RDONLY|O_CREAT);
	if (pd == -1)
	{
		perror ("error");
		return;
	}	
	int ret = 0;	
	memset(buf,0,sizeof(buf));	
	ret = read(pd, buf, 1024);
	buf[ret] = '\0';
	strcpy(msg.mess,buf);	
	strcpy(msg.fromName,myName);
	send(clientSocket,&msg,sizeof(MsgData),0);	
	close(pd);	
}

void saveGroupChat(MsgData *msg,int flag)		//���������¼�����֣�Ⱥ��˽�ķֿ���Ⱥ�ģ�
{
	FILE *fp = fopen("localChat.txt","a+");
	chatFile temp;
	if(flag == 1)
	{
		strcpy(temp.toName,"All");
		strcpy(temp.fromName,msg->name);
	}
	else
	{
		strcpy(temp.toName,"you");
		strcpy(temp.fromName,msg->fromName);
	}
	strcpy(temp.mess,msg->mess);
	strcpy(temp.time,getTime());
	fwrite(&temp,sizeof(chatFile),1,fp);//���ṹ��ֱ��д���ļ� ��bug��������

	fclose(fp);
}

void viewLocalChat()				//���������¼�����֣�Ⱥ��˽�ķֿ���˽�ģ�
{
	FILE *fp = fopen("localChat.txt","r");
	chatFile temp;
	int ret = fread(&temp,sizeof(chatFile),1,fp);
	while(ret > 0)
	{
		printf("%s:",temp.time);
		printf("\t%s:",temp.fromName);
		printf(" ��%s:",temp.toName);
		printf("˵��%s\n",temp.mess);	
		ret = fread(&temp,sizeof(chatFile),1,fp);
	}
	fclose(fp);
}

void saveFile(MsgData *msg)
{
	char buf[1024] = {0};
	sprintf(buf,"%s",msg->fileName);
	int pd = open(buf, O_WRONLY|O_CREAT);		
	if (pd == -1)
	{
		perror ("error");
		return;
	}
	memset(buf,0,sizeof(buf));
	strcpy(buf, msg->mess);
	write(pd,buf,strlen(buf));
	close(pd);	
}

void chatAll()				//Ⱥ�Ĺ���
{
	MsgData msg;	
	char tempMess[1024];	
	msg.work = 3;
	msg.flag = 1;
	strcpy(msg.name,myName);
	system("clear");
	printf("Ⱥ��\n");
	sprintf(msg.mess,"������Ⱥ��");	
	send(clientSocket,&msg,sizeof(MsgData),0);	
	memset(msg.mess,0,strlen(msg.mess));		
	while(1)
	{
		msg.flag = 0;
		gets(tempMess);		
		strcpy(msg.mess,tempMess);		
		strcpy(msg.time,getTime());	
		if (strcmp(msg.mess,"bye") == 0)					//���� bye ��ʾ�˳�Ⱥ��
		{
			memset(tempMess,0,sizeof(tempMess));		
			sprintf(msg.mess,"�˳���Ⱥ��\n");
			send(clientSocket,&msg,sizeof(MsgData),0);		//���˳�����Ϣ��������
			memset(msg.mess,0,strlen(msg.mess));	
			strcpy(msg.mess,"bye");						
			send(clientSocket,&msg,sizeof(MsgData),0);
			break;
		}
		else if(strcmp(msg.mess,"q2") == 0)				//q2����˽��
		{
			chatOne();	
			sleep(1);
			printf("���ѷ���Ⱥ��\n");			
		}
		else if(strcmp(msg.mess,"q3") == 0)				//�鿴������Ա
		{
			lookOnlinePeople();
			sleep(1);
			printf("\n���ѷ���Ⱥ��\n");	
		}
		else if(strcmp(msg.mess,"q4") == 0)				//�鿴�����¼
		{
			viewLocalChat();								

			sleep(1);
			printf("\n���ѷ���Ⱥ��\n");	
		}
		else if(strcmp(msg.mess,"q5") == 0)				//����Ա����
		{
			isRoot();    //rootȨ���ж�	
			sleep(1);
			if(rootFlag == 1)  //�ж��Ƿ�Ϊroot�û�
			{
				rootActions();
			}
			printf("���ѷ���Ⱥ��\n");	
		}
		else if(strcmp(msg.mess,"q6") == 0)			//�����ļ�
		{
			sendFile();
			sleep(1);
			printf("���ѷ���Ⱥ��\n");
		}
		else if(strcmp(msg.mess,"help") == 0)	
		{
			chatManual();							//�ֲ�
			sleep(1);
			printf("���ѷ���Ⱥ��\n");
		}			
		else
		{
			send(clientSocket,&msg,sizeof(MsgData),0);
			sleep(1);
		}
		memset(msg.mess,0,strlen(msg.mess));	
		memset(msg.time,0,strlen(msg.time));	
		memset(tempMess,0,sizeof(tempMess));	
	}	
}

void linkOffline()
{
	MsgData msg;	
	msg.work = 15;	
	strcpy(msg.name,myName);	
	send(clientSocket,&msg,sizeof(MsgData),0);	
}

void secondMenuAndAction()
{
	char getWork[5];
	int actions;
	int flag = 0;
	while(1)
	{	
		sleep(1);
		printf("\t****************\n");
		printf("\t3.����Ⱥ��\n");
		printf("\t6.������Ϣ����\n");
		printf("\t5.�˳���������\n");
		printf("\t****************\n");

		gets(getWork);
		actions = atoi(getWork);

		switch(actions)
		{
			case 3:printf("actions = %d\n",actions);chatAll();break;
			case 5:linkOffline();flag = 1;break;
		}

		if(flag == 1)
		{		
			bossFlag = 1;  //��Ҫ�˳�����������ô���������Ϊ1
			MsgData msg;
			msg.work = 16;
			send(clientSocket,&msg,sizeof(MsgData),0);
			break;
		}
	}
}

void anyKeyToNext()
{
	char str[50];
	printf("�����������\n");
	gets(str);
	return;
}

void registerNewAccount()				//ע��
{
	MsgData msg;
	msg.work = 1;
	int accountTemp;	
	char tempName[30];
	char tempAccount[30];
	char tempPassword[30];	
	srand((unsigned) time(NULL));				//ʱ�亯���������
	accountTemp = rand()%10000;
	sprintf(msg.account,"%d",accountTemp);				//����ת���ַ������??�ַ���	
	printf("������ע�ᣬ��һ����������out���ɷ���\n");
	printf("��������ǳƣ�");
	gets(tempName);
	if(strcmp(tempName,"out") == 0)
	{
		return ;
	}
	printf("����������룺");
	gets(tempPassword);
	if(strcmp(tempPassword,"out") == 0)
	{
		return ;
	}
	printf("�������ܱ�����: ");
	gets(msg.question);
	if(strcmp(msg.question,"out") == 0)
	{
		return ;
	}
	printf("�������ܱ���: ");
	gets(msg.answer);	
	if(strcmp(msg.answer,"out") == 0)
	{
		return ;
	}
	strcpy(msg.name,tempName);
	strcpy(msg.pass,tempPassword);	
	if(strcmp(msg.name,"root") == 0)			
	{
		msg.root = 1;
		strcpy(msg.account,"111");
	}
	else
	{
		msg.root = 0;
	}
	send(clientSocket,&msg,sizeof(MsgData),0);		//���͸��ܿ���̨;	
	if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//ÿ�ζ��ж�
	{
		printf("�������Ͽ�����\n");
		exit(-1);
	}		
	if(msg.flag == 1)
	{
		printf("ע��ɹ�\n");
		printf("\n���ĵ����˺�Ϊ��%s ������μ�\n",msg.account);
	}
	else if(msg.flag == 0)
	{
		printf("����������ע��ʧ��\n");
	}
	else if(msg.flag == 3)
	{
		printf("�ǳ��ظ�������ע��\n");
	}
} 

int enterAccount()									//����
{
	MsgData msg;	
	msg.work = 2;	
	char tempAccount[30];
	char tempPassword[30];	//���һ��Сbug����һ�û����볬��30�����ˣ�������
	printf("��������˺ţ�");
	gets(tempAccount);
	printf("����������룺");
	gets(tempPassword);
	strcpy(msg.account,tempAccount);
	strcpy(msg.pass,tempPassword);	
	send(clientSocket,&msg,sizeof(MsgData),0);	
	if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0) //ÿ�ζ��ж�
	{
		printf("�������Ͽ�����\n");
		exit(-1);
	}	
	if(msg.flag == 1)
	{
		printf("\n");
		printf("��ӭ��%s\n",msg.name);
		printf("\n");
		strcpy(myName,msg.name);	
		strcpy(myAccount,msg.account);
	}
	else if(msg.flag == 2)
	{
		printf("���˺��ѵ���\n");
		return 0;
	}
	else if(msg.flag == 3)
	{
		printf("�������\n");
		return 0;
	}
	else if(msg.flag == 5)
	{
		printf("�޴��û�\n");
		return 0;
	}
	else
	{
		printf("ϵͳ����,������\n");
		return 0;
	}
	return 1;
}

char *getTime()							//��ȡʱ�亯��
{
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	return asctime(timeinfo);
}

void updateName()
{
	MsgData msg;
	msg.work = 12;	
	char tempAccount[30];
	char tempPassword[30];
	char tempName[30];
	printf("��֤�˺ź�����\n");
	printf("��������˺ţ�");
	gets(tempAccount);
	printf("����������룺");
	gets(tempPassword);
	strcpy(msg.account,tempAccount);
	strcpy(msg.pass,tempPassword);	
	send(clientSocket,&msg,sizeof(MsgData),0);	
	if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//ÿ�ζ��ж�
	{
		printf("�������Ͽ�����\n");
		exit(-1);
	}	
	if(msg.flag == 0)
	{
		printf("�˺Ż��������\n");
		msg.flag = 2;	
		send(clientSocket,&msg,sizeof(MsgData),0);
		return;
	}
	else if(msg.flag == 1)
	{
		printf("�˺�������ȷ\n");
		printf("������Ҫ�޸ĳɵ��ǳ�:");
		gets(tempName);
		strcpy(msg.name,tempName);
		msg.flag = 3;	
		send(clientSocket,&msg,sizeof(MsgData),0);
		printf("�������\n");
	}
}

void updatePassword()
{
	MsgData msg;	
	msg.work = 11;	
	char tempAccount[30];
	char tempPassword[30];
	printf("��֤�˺ź�����\n");
	printf("��������˺ţ�");
	gets(tempAccount);
	printf("����������룺");
	gets(tempPassword);
	strcpy(msg.account,tempAccount);
	strcpy(msg.pass,tempPassword);	
	send(clientSocket,&msg,sizeof(MsgData),0);	
	if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//ÿ�ζ��ж�
	{
		printf("�������Ͽ�����\n");
		exit(-1);
	}	
	if(msg.flag == 0)
	{
		printf("�˺Ż��������\n");
		msg.flag = 2;	
		send(clientSocket,&msg,sizeof(MsgData),0);
		return;
	}
	else if(msg.flag == 1)
	{
		printf("�˺�������ȷ\n");
		printf("������Ҫ�޸ĳɵ�����:");
		gets(tempPassword);
		strcpy(msg.pass,tempPassword);
		msg.flag = 3;	
		send(clientSocket,&msg,sizeof(MsgData),0);
		if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//ÿ�ζ��ж�
		{
			printf("�������Ͽ�����\n");
			exit(-1);
		}		
		if(msg.flag == -1)
		{
			printf("����ʧ��\n");
		}
		else
		{
			printf("�������\n");
		}		
	}
}

void retrieveAccountPassword()		//�һ��˺�����
{
	MsgData msg;
	msg.work = 19;	
	printf("�����ǳƣ�");
	gets(msg.name);
	send(clientSocket,&msg,sizeof(MsgData),0);	
	if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//ÿ�ζ��ж�
	{
		printf("�������Ͽ�����\n");
		exit(-1);
	}		
	if(msg.flag != 1)
	{
		printf("���޴���\n");
		return;		
	}
	msg.work = 20;
	printf("�ܱ����⣺%s\n",msg.question);
	printf("������𰸣�");
	gets(msg.answer);	
	send(clientSocket,&msg,sizeof(MsgData),0);
	if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//ÿ�ζ��ж�
	{
		printf("�������Ͽ�����\n");
		exit(-1);
	}		
	if(msg.flag == 1)
	{
		printf("����˺�Ϊ%s\n",msg.account);
		printf("�������Ϊ%s\n",msg.pass);
	}
	else
	{
		printf("�𰸴���\n");
	}
}


//���ڻ���ͼ�ν���û�У��÷�


