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
	int work;					//cli¹¤×÷Ö¸Áî
	int flag;					//»ØÖ´flag
	char mess[1024];			//ÏûÏ¢
	char account[30];			//ÓÃ»§µÇÈëÕËºÅ
	char name[30];				//ÓÃ»§êÇ³Æ
	char pass[30];				//ÓÃ»§ÃÜÂë
	char online[30];			//ÔÚÏßÇé¿ö
	char time[30];				//»ñÈ¡Ê±¼ä
	int root;					//rootÈ¨ÏŞ	
	int jinyan;					//½ûÑÔflag
	char toName[30];			//·¢¸øË­
	char fromName[30];			//Ë­·¢µÄ
	char fileName[30];			//ÎÄ¼ş
	char question[256];		    //ÃÜ±£ÎÊÌâ
	char answer[256];			//ÃÜ±£´ğ°¸
};

class chatFile
{
public:
	char mess[1024];
	char fromName[30];
	char toName[30];
	char time[30];
	int flag;				    //ÅĞ¶ÏË½ÁÄ»¹ÊÇÈºÁÄ 1Ë½ÁÄ£¬0ÈºÁÄ
};

char IP[15];				    //·şÎñÆ÷µÄIP
short PORT = 7777;				//·şÎñÆ÷·şÎñ¶Ë¿Ú
int clientSocket;
char myName[30];                //êÇ³Æ
char myAccount[30];				//ID
int isChatOneOnline;			//ÅĞ¶ÏÊÇ·ñÔÚÏß
int slientFlag;					//½ûÑÔºó²»±£´æ±¾µØÁÄÌì¼ÇÂ¼
int rootFlag;					//ÅĞ¶Ï¹ÜÀíÔ±flag
int bossFlag;					//ÅĞ¶ÏÓÃ»§ÊÇ·ñÒªÍË³öµ½Ö÷³ÌĞò
int noOneSlientFlag;			//ÅĞ¶ÏÊÇ·ñÓĞÈË±»½ûÑÔ£¬ÓĞ=1


void registerNewAccount();//×¢²á
void saveGroupChat(MsgData *msg,int flag);
void chatAll();
char *getTime();
void firstMenu();
void chatManual();//°ïÖúÊÖ²á
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


void* recvThread(void* _clientSocket)				//Ò»Ö±ÔÚÕâÀï½ÓÊÕ
{
	int clientSocket = *(int*)_clientSocket;
	MsgData msg;
	while(1)
	{
		if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//Ã¿´Î¶¼ÅĞ¶Ï
		{
			printf("·şÎñÆ÷¶Ï¿ªÁ´½Ó\n");
			exit(-1);
		}
		switch(msg.work)
		{
			case 3:
				printf("£¨ÈºÁÄ£©%s : %s\n",msg.name,msg.mess);
				saveGroupChat(&msg,1);
				break;	
			case 4:
				isChatOneOnline = 1;//Ã»ÕÒµ½Ë½ÁÄµÄÈË»òÒÑÏÂÏÂÏß
				break;
			case 5: 
				if(isChatOneOnline != 1)
				{
					printf("£¨Ë½ÁÄ£©%sÍµÍµ¶ÔÄãËµ£º%s\n",msg.fromName,msg.mess);
					saveGroupChat(&msg,0);
				}
				break;
			case 6:  //ÔÚÏßÈËÔ±
				printf("%s\n",msg.name);
				break;			
			case 7: 
				if(msg.flag == 1)
				{
					rootFlag = 1;
					printf("ÄúÊÇrootÓÃ»§ÇëÉÔºó\n");
				}
				else
				{
					printf("Äú²»ÊÇrootÓÃ»§\nÕıÔÚ·µ»ØÈºÁÄ........\n");
				}
				break;
			case 8:
				slientFlag = 1;
				printf("ÄúÒÑ¾­±»½ûÑÔÁË\n");
				break;
			case 9:
				printf("²Ù×÷Ê§°Ü£¨¶Ô·½¿ÉÄÜ²»´æÔÚ£©ÇëÖØĞÂÑ¡Ôñ²Ù×÷\n");
				break;
			case 10:
				printf("½ûÑÔ %s ³É¹¦\n",msg.toName);
				break;
			case 11:
				printf("½â³ı %s µÄ½ûÑÔ³É¹¦\n",msg.toName);
				break;
			case 12:
				printf("ÎÄ¼ş·¢ËÍ³É¹¦\nÕıÔÚ·µ»ØÈºÁÄ........\n");
				break;
			case 13:
				printf("ÎÄ¼ş·¢ËÍÊ§°Ü£¨´ËÈË²»ÈºÁÄÖĞ²»ÄÜ½ÓÊÕÎÄ¼ş£©\nÕıÔÚ·µ»ØÈºÁÄ........\n");
				break;
			case 15:
				saveFile(&msg);
				printf("´Ó%s´¦½ÓÊÕµ½Ò»¸öÎÄ¼ş\n",msg.fromName);
				break;
			case 16:
				bossFlag = 1;  //ÍË³öµ½Ö÷³ÌĞò
				break;
			case -1:
				printf("Ò»Ğ©ÒâÏë²»µ½µÄ´íÎó·¢ÉúÁË\n");
				break;
			case 18:
				printf("ÉèÖÃ³É¹¦\n");
				break;
			case 19:
				slientFlag = 0;
				printf("ÄúÒÑ±»½â³ı½ûÑÔ\n");
				break;
			case 20:
				printf("%s\n",msg.mess);
				exit(0);
				break;
			case 21:
				printf("¶Ô·½²»´æÔÚ»ò²»ÔÚÏß\n");
				break;
			case 22://±»½ûÑÔÈËÔ±
				printf("%s\n",msg.name);
				break;
			case 23:
				noOneSlientFlag = 1;
				break;
			case 24:
				printf("Äã±»ÉèÎª¹ÜÀíÔ±\n");
				break;
			case 25:
				printf("Äã±»ÒÆ³ı¹ÜÀíÔ±\n");
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
	int last = 0;								//ipÃ¿Ò»¶ÎµÄ³õÊ¼Î»ÖÃÏÂ±ê
	char temp[50];								//´æ·ÅipµÄÃ¿¶Î
	int ipTemp = 0;								//Ã¿¶Îip×ª»»ÎªintĞÍÓÃÀ´ÅĞ¶Ï
	int tempi = 0;
	int num = 0;
	while(ip[i] != 0)
	{
		if(ip[i] == ' ')							//ÊäÈë¿Õ¸ñµÄ´íÎó
		{
			return 1;
		}
		if(ip[i] == '.' || ip[i + 1] == 0)
		{
			flagi = i;
			if(ip[i + 1] == 0)					//ÄÜ¹»ÅĞ¶Ï×îºóÒ»¶Î
			{
				flagi++;
			}	
			num++;								//¼ÆËã.µÄÊıÁ¿£¬ÕıÈ·µÄÓ¦ÓĞ3
			for(j = last; j < flagi; j++)
			{
				temp[tempi++] = ip[j];			//Ã¿¶Î¸´ÖÆ¸øtemp
			}
			temp[tempi] = 0;
			last = flagi + 1;						//±£´æÃ¿´Î.ºóip¶ÎµÄ¿ªÊ¼Î»ÖÃ
			tempi = 0;
			ipTemp = atoi(temp);					//temp×ªÎªint
			if(!(ipTemp >= 0 && ipTemp <= 255))
			{
				return 1;						//³¬³ö·¶Î§Ö±½Ó·µ»Ø
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
	printf("\tÁÄÌìÊÒdemo\t\n");
	printf("\t*****************\n");
	printf("\t1.×¢²á\n");
	printf("\t2.µÇÈë\n");
	printf("\t3.ĞŞ¸ÄÃÜÂë\n");
	printf("\t4.ĞŞ¸ÄêÇ³Æ\n");
	printf("\t5.ÕÒ»ØÃÜÂë\n");
	printf("\t6.ÍË³ö\n");
	printf("\tÈºÖ÷ĞèÇÀÏÈ×¢²áêÇ³ÆÎªroot\n");
	printf("\t*****************\n");
}

void chatManual()  //ÈºÁÄÊÖ²á
{
	system("clear");
	printf("********************************************\n");
	printf("\tÈºÁÄ²Ù×÷Ö¸ÄÏ\n");
	printf("q2£º½øÈëË½ÁÄÄ£Ê½£¨Ë½ÁÄÄ£Ê½ÏÂÊäback·µ»ØÈºÁÄ£©\n");
	printf("q3£º²é¿´ÔÚÏßÈËÔ±\n");
	printf("q4£º²é¿´±¾µØÁÄÌì¼ÇÂ¼\n");
	printf("q5£º½øÈë¹ÜÀíÔ±²Ù×÷\n");
	printf("q6£º·¢ËÍÎÄ¼ş\n");
	printf("ÈºÁÄÄ£Ê½ÏÂÊäÈëbyeÍË³öÈºÁÄ·µ»ØÖ÷³ÌĞò\n");
	printf("********************************************\n");
}

void chatOne()		//Ë½ÁÄ
{
	MsgData msg;	
	char tempMess[1024];
	char oneName[30];
	lookOnlinePeople();
	sleep(1);
	while(1)
	{
		isChatOneOnline = 0;		//Ã¿´Îwhile½«isChatOneOnlineÖØÖÃÎª0£»
		msg.work = 4;
		printf("ÊäÈëÄãÏëÒªË½ÁÄµÄÈË£º(backÍË³öË½ÁÄ·µÈºÁÄ)\n");
		gets(oneName);
		if(strcmp(oneName,"back") == 0)
		{
			printf("ÕıÔÚ·µ»ØÈºÁÄ........\n");		
			return;
		}
		strcpy(msg.toName,oneName);
		strcpy(msg.fromName,myName);
		if(strcmp(msg.toName, msg.fromName) == 0)
		{
			printf("ÇëÎğºÍ×Ô¼ºË½ÁÄ£¡£¡\n");
			sleep(1);
			continue;
		}
		printf("¼ìÑéÖĞÇëµÈ´ı........\n");
		send(clientSocket,&msg,sizeof(MsgData),0);
		sleep(1);		
		if(isChatOneOnline == 1)		//isChatOneOnline£ºÈôÎŞ´ËÈË¾Í²»¿ªÊ¼Ë½ÁÄ
		{
			printf("ÔÚÏßÖĞÃ»ÓĞÕÒµ½´ËÈË\n");		
		}
		else
		{
			printf("Äú¿ÉÒÔ¿ªÊ¼Ë½ÁÄÁË\n");
			break;		
		}
		memset(oneName,0,strlen(oneName));
	}
	msg.work = 5;//Ë½ÁÄÓÃ5
	while(1)
	{
		gets(tempMess);
		strcpy(msg.mess,tempMess);
		strcpy(msg.time,getTime());
		if(strcmp(tempMess,"back") == 0)
		{
			printf("ÕıÔÚ·µ»ØÈºÁÄ........\n");		
			return;
		}
		else
		{
			if(isChatOneOnline == 1)
			{
				printf("¶Ô·½ÒÑ²»ÔÚÈºÁÄ\n");
				break;
			}
			printf("Äã¶Ô %s ÍµÍµËµÁË£º%s\n",msg.toName,msg.mess);		
			send(clientSocket,&msg,sizeof(MsgData),0);
		}
		memset(tempMess,0,sizeof(tempMess));
	}
}

void lookOnlinePeople()							//²é¿´ÔÚÈºÁÄÖĞµÄÈËÔ±
{
	MsgData msg;
	msg.work = 6;
	printf("ÔÚÏßÃûµ¥ÈçÏÂ\n");
	send(clientSocket,&msg,sizeof(MsgData),0);	
}

void isRoot()							//rootÈ¨ÏŞÅĞ¶Ï
{
	MsgData msg;
	msg.work = 7;
	strcpy(msg.account,myAccount);	
	send(clientSocket,&msg,sizeof(MsgData),0);		
}

void makeSlient()
{	
	sleep(1);				//ÒòÎªsend½»¸øÏß³Ì´¦Àíºó£¬Õâ±ß»á¼ÌĞøÍùÏÂ×ß£»ËùÒÔÒªsleep
	MsgData msg;			//µÈÏß³ÌÄÇ¶ù´¦ÀíÍê£»
	msg.work = 8;
	char tempName[30];
	printf("ÊäÈë½ûÑÔĞÕÃû£º");
	gets(tempName);
	strcpy(msg.toName,tempName);
	send(clientSocket,&msg,sizeof(MsgData),0);
}

void releaseSlient()
{
	MsgData msg;
	msg.work = 10;
	char tempName[30];	
	printf("ÊäÈë½â½ûĞÕÃû£º");
	gets(tempName);
	strcpy(msg.toName,tempName);
	send(clientSocket,&msg,sizeof(MsgData),0);
}

void thirdMenu()
{
	printf("	  rootÓÃ»§²Ù×÷        \n");
	printf("\n");
	printf("1,½ûÑÔ		2,½â³ı½ûÑÔ\n");
	printf("3,ÉèÖÃ¹ÜÀíÔ±	4,ÒÆ³ı¹ÜÀíÔ±\n");
	printf("5,ÌßÈË		6,·µ»ØÈºÁÄ\n");
	printf("\n");
}

void setRoot()//ÉèÖÃ¹ÜÀíÔ±È¨ÏŞ( ÈºÖ÷È¨ÏŞ)
{
	MsgData msg;
	msg.work = 17;
	printf("ÊäÈëÄãÏëÒªÉèÖÃ¹ÜÀíµÄÈËµÄêÇ³Æ£º");
	gets(msg.name);
	send(clientSocket,&msg,sizeof(MsgData),0);	
}

void removeRoot()
{
	MsgData msg;
	msg.work = 18;
	printf("ÊäÈëÄãÏëÒªÒÆ³ı¹ÜÀíµÄÈËµÄêÇ³Æ£º");
	gets(msg.name);
	send(clientSocket,&msg,sizeof(MsgData),0);	
}

void getOut()
{
	MsgData msg;
	msg.work = 21;	
	printf("ÊäÈëĞÕÃû£º");
	gets(msg.toName);
	send(clientSocket,&msg,sizeof(MsgData),0);	
}

void lookSlientPeople()//²é¿´±»½ûÑÔÈËÔ±
{
	MsgData msg;
	msg.work = 22;
	send(clientSocket,&msg,sizeof(MsgData),0);	
	printf("±»½ûÑÔÈËÔ±ÈçÏÂ:\n");
}

void rootActions()
{
	char getWork[5];
	int actions;
	int flag = 0;
	while(1)
	{
		thirdMenu();
		printf("ÊäÈë²Ù×÷:");
		gets(getWork);
		actions = atoi(getWork);
		switch(actions)
		{
			case 1:
				lookOnlinePeople();
				printf("µÈ´ı...\n");
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
					printf("Ä¿Ç°Ã»ÓĞÈË±»½ûÑÔ\n");
					break;
				}
				releaseSlient();
				sleep(1);
				break;
			case 3:
				lookOnlinePeople();
				printf("µÈ´ı...\n");
				sleep(1);
				setRoot();
				sleep(1);
				break;
			case 4:
				lookOnlinePeople();
				printf("µÈ´ı...\n");
				sleep(1);
				removeRoot();
				sleep(1);
				break;
			case 5:
				lookOnlinePeople();
				printf("µÈ´ı...\n");
				sleep(1);
				getOut();
				sleep(1);
				break;
			case 6:
				flag = 1;
				printf("ÕıÔÚ·µ»ØÈºÁÄ\n");
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
	printf("ÇëÊäÈëÒª·¢ËÍµÄ¶ÔÏóêÇ³Æ\n");
	gets(tempName);
	strcpy(msg.toName,tempName);
	printf("ÊäÈëÒª´«µÄÎÄ¼ş(¼Óºó×º)\n");
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

void saveGroupChat(MsgData *msg,int flag)		//±£´æÁÄÌì¼ÇÂ¼·ÖÁ©ÖÖ£¬ÈºÁÄË½ÁÄ·Ö¿ª£¨ÈºÁÄ£©
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
	fwrite(&temp,sizeof(chatFile),1,fp);//½«½á¹¹ÌåÖ±½ÓĞ´ÈëÎÄ¼ş ÓĞbug¡£²»¹ÜÁË

	fclose(fp);
}

void viewLocalChat()				//±£´æÁÄÌì¼ÇÂ¼·ÖÁ©ÖÖ£¬ÈºÁÄË½ÁÄ·Ö¿ª£¨Ë½ÁÄ£©
{
	FILE *fp = fopen("localChat.txt","r");
	chatFile temp;
	int ret = fread(&temp,sizeof(chatFile),1,fp);
	while(ret > 0)
	{
		printf("%s:",temp.time);
		printf("\t%s:",temp.fromName);
		printf(" ¶Ô%s:",temp.toName);
		printf("Ëµ£º%s\n",temp.mess);	
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

void chatAll()				//ÈºÁÄ¹¦ÄÜ
{
	MsgData msg;	
	char tempMess[1024];	
	msg.work = 3;
	msg.flag = 1;
	strcpy(msg.name,myName);
	system("clear");
	printf("ÈºÁÄ\n");
	sprintf(msg.mess,"½øÈëÁËÈºÁÄ");	
	send(clientSocket,&msg,sizeof(MsgData),0);	
	memset(msg.mess,0,strlen(msg.mess));		
	while(1)
	{
		msg.flag = 0;
		gets(tempMess);		
		strcpy(msg.mess,tempMess);		
		strcpy(msg.time,getTime());	
		if (strcmp(msg.mess,"bye") == 0)					//ÊäÈë bye ±íÊ¾ÍË³öÈºÁÄ
		{
			memset(tempMess,0,sizeof(tempMess));		
			sprintf(msg.mess,"ÍË³öÁËÈºÁÄ\n");
			send(clientSocket,&msg,sizeof(MsgData),0);		//½«ÍË³öµÄÏûÏ¢·¢¸ø¹«ÆÁ
			memset(msg.mess,0,strlen(msg.mess));	
			strcpy(msg.mess,"bye");						
			send(clientSocket,&msg,sizeof(MsgData),0);
			break;
		}
		else if(strcmp(msg.mess,"q2") == 0)				//q2½øÈëË½ÁÄ
		{
			chatOne();	
			sleep(1);
			printf("ÄúÒÑ·µ»ØÈºÁÄ\n");			
		}
		else if(strcmp(msg.mess,"q3") == 0)				//²é¿´ÔÚÏßÈËÔ±
		{
			lookOnlinePeople();
			sleep(1);
			printf("\nÄúÒÑ·µ»ØÈºÁÄ\n");	
		}
		else if(strcmp(msg.mess,"q4") == 0)				//²é¿´ÁÄÌì¼ÇÂ¼
		{
			viewLocalChat();								

			sleep(1);
			printf("\nÄúÒÑ·µ»ØÈºÁÄ\n");	
		}
		else if(strcmp(msg.mess,"q5") == 0)				//¹ÜÀíÔ±²Ù×÷
		{
			isRoot();    //rootÈ¨ÏŞÅĞ¶Ï	
			sleep(1);
			if(rootFlag == 1)  //ÅĞ¶ÏÊÇ·ñÎªrootÓÃ»§
			{
				rootActions();
			}
			printf("ÄúÒÑ·µ»ØÈºÁÄ\n");	
		}
		else if(strcmp(msg.mess,"q6") == 0)			//·¢ËÍÎÄ¼ş
		{
			sendFile();
			sleep(1);
			printf("ÄúÒÑ·µ»ØÈºÁÄ\n");
		}
		else if(strcmp(msg.mess,"help") == 0)	
		{
			chatManual();							//ÊÖ²á
			sleep(1);
			printf("ÄúÒÑ·µ»ØÈºÁÄ\n");
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
		printf("\t3.½øÈëÈºÁÄ\n");
		printf("\t6.ÀëÏßÏûÏ¢½ÓÊÕ\n");
		printf("\t5.ÍË³öµ½Ö÷³ÌĞò\n");
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
			bossFlag = 1;  //ÏëÒªÍË³öµ½Ö÷³ÌĞò£¬ÄÇÃ´½«Õâ¸öÉèÖÃÎª1
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
	printf("°´ÈÎÒâ¼ü·µ»Ø\n");
	gets(str);
	return;
}

void registerNewAccount()				//×¢²á
{
	MsgData msg;
	msg.work = 1;
	int accountTemp;	
	char tempName[30];
	char tempAccount[30];
	char tempPassword[30];	
	srand((unsigned) time(NULL));				//Ê±¼äº¯ÊıÉú³ÉËæ»ú
	accountTemp = rand()%10000;
	sprintf(msg.account,"%d",accountTemp);				//ÕûĞÎ×ª»»×Ö·û´®µÄä??ÖÖ·½·¨	
	printf("Èô²»Ïë×¢²á£¬ÈÎÒ»ÊäÈëÏÂÊäÈëout¼´¿É·µ»Ø\n");
	printf("ÊäÈëÄãµÄêÇ³Æ£º");
	gets(tempName);
	if(strcmp(tempName,"out") == 0)
	{
		return ;
	}
	printf("ÊäÈëÄãµÄÃÜÂë£º");
	gets(tempPassword);
	if(strcmp(tempPassword,"out") == 0)
	{
		return ;
	}
	printf("ÇëÊäÈëÃÜ±£ÎÊÌâ: ");
	gets(msg.question);
	if(strcmp(msg.question,"out") == 0)
	{
		return ;
	}
	printf("ÇëÊäÈëÃÜ±£´ğ°¸: ");
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
	send(clientSocket,&msg,sizeof(MsgData),0);		//·¢ËÍ¸ø×Ü¿ØÖÆÌ¨;	
	if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//Ã¿´Î¶¼ÅĞ¶Ï
	{
		printf("·şÎñÆ÷¶Ï¿ªÁ´½Ó\n");
		exit(-1);
	}		
	if(msg.flag == 1)
	{
		printf("×¢²á³É¹¦\n");
		printf("\nÄúµÄµÇÈëÕËºÅÎª£º%s ÇëÎñ±ØÀÎ¼Ç\n",msg.account);
	}
	else if(msg.flag == 0)
	{
		printf("·şÎñÆ÷´íÎó×¢²áÊ§°Ü\n");
	}
	else if(msg.flag == 3)
	{
		printf("êÇ³ÆÖØ¸´ÇëÖØĞÂ×¢²á\n");
	}
} 

int enterAccount()									//µÇÈë
{
	MsgData msg;	
	msg.work = 2;	
	char tempAccount[30];
	char tempPassword[30];	//Õâ¿éÒ»µãĞ¡bug£¬ÍòÒ»ÓÃ»§ÃÜÂë³¬¹ı30£¬ËãÁË£¬²»¹ÜÁË
	printf("ÊäÈëÄãµÄÕËºÅ£º");
	gets(tempAccount);
	printf("ÊäÈëÄãµÄÃÜÂë£º");
	gets(tempPassword);
	strcpy(msg.account,tempAccount);
	strcpy(msg.pass,tempPassword);	
	send(clientSocket,&msg,sizeof(MsgData),0);	
	if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0) //Ã¿´Î¶¼ÅĞ¶Ï
	{
		printf("·şÎñÆ÷¶Ï¿ªÁ´½Ó\n");
		exit(-1);
	}	
	if(msg.flag == 1)
	{
		printf("\n");
		printf("»¶Ó­Äã%s\n",msg.name);
		printf("\n");
		strcpy(myName,msg.name);	
		strcpy(myAccount,msg.account);
	}
	else if(msg.flag == 2)
	{
		printf("¸ÃÕËºÅÒÑµÇÈë\n");
		return 0;
	}
	else if(msg.flag == 3)
	{
		printf("ÃÜÂë´íÎó\n");
		return 0;
	}
	else if(msg.flag == 5)
	{
		printf("ÎŞ´ËÓÃ»§\n");
		return 0;
	}
	else
	{
		printf("ÏµÍ³´íÎó,ÇëÖØÊÔ\n");
		return 0;
	}
	return 1;
}

char *getTime()							//»ñÈ¡Ê±¼äº¯Êı
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
	printf("ÑéÖ¤ÕËºÅºÍÃÜÂë\n");
	printf("ÊäÈëÄãµÄÕËºÅ£º");
	gets(tempAccount);
	printf("ÊäÈëÄãµÄÃÜÂë£º");
	gets(tempPassword);
	strcpy(msg.account,tempAccount);
	strcpy(msg.pass,tempPassword);	
	send(clientSocket,&msg,sizeof(MsgData),0);	
	if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//Ã¿´Î¶¼ÅĞ¶Ï
	{
		printf("·şÎñÆ÷¶Ï¿ªÁ´½Ó\n");
		exit(-1);
	}	
	if(msg.flag == 0)
	{
		printf("ÕËºÅ»òÃÜÂë´íÎó\n");
		msg.flag = 2;	
		send(clientSocket,&msg,sizeof(MsgData),0);
		return;
	}
	else if(msg.flag == 1)
	{
		printf("ÕËºÅÃÜÂëÕıÈ·\n");
		printf("ÊäÈëÏëÒªĞŞ¸Ä³ÉµÄêÇ³Æ:");
		gets(tempName);
		strcpy(msg.name,tempName);
		msg.flag = 3;	
		send(clientSocket,&msg,sizeof(MsgData),0);
		printf("²Ù×÷Íê³É\n");
	}
}

void updatePassword()
{
	MsgData msg;	
	msg.work = 11;	
	char tempAccount[30];
	char tempPassword[30];
	printf("ÑéÖ¤ÕËºÅºÍÃÜÂë\n");
	printf("ÊäÈëÄãµÄÕËºÅ£º");
	gets(tempAccount);
	printf("ÊäÈëÄãµÄÃÜÂë£º");
	gets(tempPassword);
	strcpy(msg.account,tempAccount);
	strcpy(msg.pass,tempPassword);	
	send(clientSocket,&msg,sizeof(MsgData),0);	
	if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//Ã¿´Î¶¼ÅĞ¶Ï
	{
		printf("·şÎñÆ÷¶Ï¿ªÁ¬½Ó\n");
		exit(-1);
	}	
	if(msg.flag == 0)
	{
		printf("ÕËºÅ»òÃÜÂë´íÎó\n");
		msg.flag = 2;	
		send(clientSocket,&msg,sizeof(MsgData),0);
		return;
	}
	else if(msg.flag == 1)
	{
		printf("ÕËºÅÃÜÂëÕıÈ·\n");
		printf("ÊäÈëÏëÒªĞŞ¸Ä³ÉµÄÃÜÂë:");
		gets(tempPassword);
		strcpy(msg.pass,tempPassword);
		msg.flag = 3;	
		send(clientSocket,&msg,sizeof(MsgData),0);
		if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//Ã¿´Î¶¼ÅĞ¶Ï
		{
			printf("·şÎñÆ÷¶Ï¿ªÁ´½Ó\n");
			exit(-1);
		}		
		if(msg.flag == -1)
		{
			printf("²Ù×÷Ê§°Ü\n");
		}
		else
		{
			printf("²Ù×÷Íê³É\n");
		}		
	}
}

void retrieveAccountPassword()		//ÕÒ»ØÕËºÅÃÜÂë
{
	MsgData msg;
	msg.work = 19;	
	printf("ÊäÈëêÇ³Æ£º");
	gets(msg.name);
	send(clientSocket,&msg,sizeof(MsgData),0);	
	if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//Ã¿´Î¶¼ÅĞ¶Ï
	{
		printf("·şÎñÆ÷¶Ï¿ªÁ´½Ó\n");
		exit(-1);
	}		
	if(msg.flag != 1)
	{
		printf("²éÎŞ´ËÈË\n");
		return;		
	}
	msg.work = 20;
	printf("ÃÜ±£ÎÊÌâ£º%s\n",msg.question);
	printf("ÇëÊäÈë´ğ°¸£º");
	gets(msg.answer);	
	send(clientSocket,&msg,sizeof(MsgData),0);
	if(recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)//Ã¿´Î¶¼ÅĞ¶Ï
	{
		printf("·şÎñÆ÷¶Ï¿ªÁ´½Ó\n");
		exit(-1);
	}		
	if(msg.flag == 1)
	{
		printf("ÄãµÄÕËºÅÎª%s\n",msg.account);
		printf("ÄãµÄÃÜÂëÎª%s\n",msg.pass);
	}
	else
	{
		printf("´ğ°¸´íÎó\n");
	}
}


//ÏÖÔÚ»¹²îÍ¼ĞÎ½çÃæÃ»ÓĞ£¬ºÃ·³


