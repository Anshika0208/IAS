#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h> // for the structures
#include <fcntl.h> // for open
#include <unistd.h> // for close
// for inet_ntohs
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <cstdio>
#include <bits/stdc++.h>
#include <string>
using namespace std;

string parseString(string req)
{
	cout<<req<<endl;
	int f1=req.find("/");
	int f2=req.find("HTTP");
	string filename=req.substr(f1+1,f2-6);
	//cout<<filename<<endl;
	return filename;
}

int transfer(int csock,string fname)
{
  int sent = 0;
	FILE *fd;
	

	fd=fopen(fname.c_str(),"r");
	if(!fd)
	{
		string reply="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-length: 18\r\n\r\n<h1>FILE NOT FOUND</h1>";
		reply+="\0";
		send(csock,reply.c_str(),96,0);
		return -1;
	}
	ssize_t nread=0;
	string filetype=fname.substr(fname.find(".")+1);
	string content;
	
	if(filetype=="c" || filetype=="cpp" || filetype=="txt")
		content="text/plain";
	else if(filetype=="mp4" || filetype=="3gp" || filetype=="mkv")
		content="video/html";
	else if(filetype=="img" || filetype=="jpg" || filetype=="png")
		content="image/png";
	else content="application/"+filetype;
	
	fseek(fd,0L,SEEK_END);
	int size=ftell(fd);
	rewind(fd);
	char buffer[4096];
	//cout<<filetype<<" "<<size<<endl;
	string s=to_string(size);
	string buff="HTTP/1.1 200 OK\r\nContent-Type:"+content+"\r\nContent-length:"+s+"\r\n\r\n";
	buff+="\0";
	strcpy(buffer,buff.c_str());
	cout<<buffer<<endl;
	sent = send(csock, buffer, strlen(buffer),0);
	while(!feof(fd))
	{	
		nread = fread(buffer, 1, sizeof(buffer), fd);
		
    		//cout<<"Bytes read:" << nread << "\n";
    		sent = send(csock, buffer, nread,0);
		if (sent == 0)
			break;
	}
	
	fclose(fd);
	return -1;
}

int main(int argc,char* argv[])
{
	int ssock, csock;
	struct sockaddr_in server, client;
	unsigned int len;//input for bind and accept calls
	char buffer[1024];
  int server_port = atoi("3000");
  string shared_file_name;
	string root_directory=argv[1];
	ssock = socket(AF_INET, SOCK_STREAM, 0);
	if(ssock == -1)
	{
		cout<<"socket error ";
		return -1;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(server_port);
	server.sin_addr.s_addr = INADDR_ANY; // all avail interf on machine
	bzero(&server.sin_zero, 0);	
	
	len = sizeof(struct sockaddr_in);
	if((bind(ssock, (struct sockaddr *)&server, len) == -1))
	{
		cout<<"bind error ";
		return -1;
	}
	
	if(listen(ssock, 5) == -1)
	{
		cout<<"listen error ";
		return -1;
	}
while(1)
{	
  //wait for an incoming connection
  if((csock = accept(ssock, (struct sockaddr*)&client, &len)) == -1)
		{
			cout<<"accept ";
			exit(-1);	
		}
		cout<<"Connection Accepted"<<endl;
		read(csock, buffer, 1024 - 1);
	
		shared_file_name=root_directory+"/"+parseString(buffer);
		//cout<<shared_file_name;
		transfer(csock,shared_file_name);
		//cout<<"File read"<<endl;
}
    close(csock);
    close(ssock);
return 0;
}
