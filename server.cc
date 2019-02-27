#include"server.hpp"


void* serviceThread(void* _clientSocket) //线程的回调函数，创建线程后干的事情
{
	//int clientSocket = reinterpret_cast<int>(_clientSocket);
	int clientSocket = *(int*)_clientSocket;
	//delete _clientSocket;
	MsgData msg;
	printf("pthread = %d\n",clientSocket);
	while(1)
	{
		if (recv(clientSocket,&msg,sizeof(MsgData),0) <= 0)
		{												
			printf("%d已退出%s\n",clientSocket,msg.name); //通知群聊里用户此人下线
			linkDeleteNode(clientSocket,&msg);	  //此处应该写删除节点操作而不是置是否在聊天室中 为0不在
			break;
		}
		switch(msg.work)
		{
			case 1:registerNewAccount(clientSocket,&msg);break;		//注册插入数据库1
			case 2:enterAccount(clientSocket,&msg);break;		//登入验证2
			case 3:chatAll(clientSocket,&msg);break;		//进入群聊3
			case 4:getTheManState(clientSocket,&msg);break;		//检验对象的状态，在线或者是否接受私聊
			case 5:chatOne(clientSocket,&msg);break;					//进入私聊5
			case 6:lookPeopleInChat(clientSocket,&msg);break;	//查看在线用户
			case 7:isRoot(clientSocket,&msg);break;	//禁言权限判断
			case 8:makeSlient(clientSocket,&msg);break;	//禁言实际操作
			case 10:releaseSlient(clientSocket,&msg);break;	//解除禁言实际操作
			case 11:updatePassword(clientSocket,&msg);break;	//修改密码
			case 12:updateName(clientSocket,&msg);break;	//修改昵称
			case 13:sendFile(clientSocket,&msg);break;	//文件传输
			case 15:linkOffline(clientSocket);break;		//下线
			case 16:send(clientSocket,&msg,sizeof(MsgData),0);break;//返回主程序
			case 17:setRoot(clientSocket,&msg);break;		//设置管理员
			case 18:removeRoot(clientSocket,&msg);break;	//移除管理
			case 19:findTheManAndGetQuestion(clientSocket,&msg);break;	//检验昵称
			case 20:retrieveAccountPassword(clientSocket,&msg);break;	//找回账号和密码
			case 21:getOut(clientSocket,&msg);break;		//踢人下线
			case 22:lookSlientPeople(clientSocket,&msg);break;//查看被禁言人员
		}
	}
	close(clientSocket);
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
		if(ifIpError(argv[1]))					//IP地址是否合法的检测
		{
			printf("IP地址错误\n");
			exit(-1);
		}
		strcpy(IP,argv[1]);
		printf("IP =  %s\n",IP);		
	}
	pH = initLink();								//初始化链表
	int serverSocket = initSocket();				//初始化服务端
	while(1)
	{	
		int* clientSocketp = new int;
		*clientSocketp= myAccept(serverSocket);	//接收cli

		pthread_t id;							//创建线程
		pthread_create(&id, NULL, serviceThread, (void *)clientSocketp);
		pthread_detach(id);							//待线程结束后回收其资源
	}	
	close(serverSocket);	
	return 0;
}


