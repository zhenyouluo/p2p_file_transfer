#include<iostream>
#include<unistd.h> //for close
#include<sys/types.h>
#include<sys/socket.h>
#include<memory.h>
#include<netdb.h>
#include <arpa/inet.h>
#include<netinet/ip.h>
#include<netinet/tcp.h> //for IPROTO_TCP
#include"utility1.h"

using namespace std;

struct addrinfo hints,*list;
int client_socket=-1;

void create_socket()
{
int connect_st=-1; 
int bytes_read;
memset(&hints,0,sizeof(struct addrinfo));
hints.ai_family=AF_INET;
hints.ai_socktype=SOCK_STREAM;
hints.ai_protocol=IPPROTO_TCP;

int st=getaddrinfo(server_addr,server_port,&hints,&list);
if(st!=0) {
cout<"Error in getaddrinfo()\n";
}

for(;list;list=list->ai_next){
client_socket=socket(list->ai_family,list->ai_socktype,list->ai_protocol);
if(client_socket==-1){
cout<<"Error in creating socket\n";
return;
}

connect_st=connect(client_socket,list->ai_addr,list->ai_addrlen);
int max_socket_buf_size;
unsigned int size_unsigned=sizeof(int);
getsockopt(client_socket,SOL_SOCKET,SO_RCVBUF,(void *)&max_socket_buf_size,&size_unsigned);
cout<<"max socket buf "<<max_socket_buf_size<<endl;;

if(connect_st==-1)
{
cout<<"Problem Connecting\n";
continue;
}
}
char buf[64*1024];
bytes_read=recv(client_socket,buf,sizeof(buf),MSG_WAITALL);
cout<<bytes_read<<endl;
create_files(buf);

close(client_socket);
}

int main()
{
create_socket();
}
