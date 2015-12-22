#include<iostream>
#include<fstream>
#include<unistd.h> //for close
#include<sys/types.h>
#include<sys/socket.h>
#include<memory.h>
#include<netdb.h>
#include <arpa/inet.h>
#include<netinet/ip.h>
#include<netinet/tcp.h> //for IPROTO_TCP
#include"utility.h"
#include<cstdlib>
using namespace std;

static struct addrinfo hints,*list;
static int server_socket=-1;
static int clear_value=1;
void send_frags(int sock,char *buf);

void create_socket()
{
memset(&hints,0,sizeof(struct addrinfo));
hints.ai_flags=AI_PASSIVE;
hints.ai_family=AF_INET;
hints.ai_socktype=SOCK_STREAM;
hints.ai_protocol=IPPROTO_TCP;

int st=getaddrinfo(NULL,SERVER_PORT,&hints,&list);
if(st!=0) {
cout<"Error in getaddrinfo()\n";
}
for(;list;list=list->ai_next) {
server_socket=socket(list->ai_family,list->ai_socktype,list->ai_protocol);
if(server_socket==-1){
cout<<"Error in creating socket\n";
continue;;
}
setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&clear_value,sizeof(int));

int bind_st=bind(server_socket,list->ai_addr,list->ai_addrlen);
if(bind_st==-1)
{
close(server_socket);
cout<<"Error in bind()\n";
continue;
}
cout<<"Binding Done\n";

cout<<"Starting to Listen\n";
int listen_st=listen(server_socket,10);
if(listen_st==-1)
{
close(server_socket);
cout<<"Problem in Listening\n";
continue;
}

}

freeaddrinfo(list);
}

void run_server()
{
create_socket();
struct sockaddr_in client_addr;
socklen_t size_client=sizeof(client_addr);
int socket_2_talk;
int ttl_connection=0;
int bytes_read=0;
while(1)
{
cout<<"while start\n";
socket_2_talk=accept(server_socket,(struct sockaddr*)&client_addr,&size_client);
if(socket_2_talk==-1){
cout<<"socket not created\n";
return;
}
setsockopt(socket_2_talk,SOL_SOCKET,SO_REUSEADDR,&clear_value,sizeof(int));
if(socket_2_talk!=-1)
{
cout<<"connection_created with client \n";
cout<< ntohs(client_addr.sin_port)<<endl;
}
char file_name[MAX_FILE_NAME+28];
cout<<"Enter file to transmit";
cin>>file_name;
send_frags(socket_2_talk,file_name);
remove(file_name);
close(socket_2_talk);
}
close(server_socket);
}

void send_frags(int sock,char *file_name)
{
fstream out_file(file_name,ios::in);
file_meta meta;
int bytes_to_send;

out_file.read((char *)&meta,sizeof(file_meta));
cout<<((file_meta*)&meta)->name<<endl;
bytes_to_send=((file_meta*)&meta)->size;
cout<<bytes_to_send<<endl;

char *buf=(char*)calloc(1,sizeof(char)*(bytes_to_send+1));
if(!buf){
cout<<"memory not allocated\n";
}
out_file.seekg(0,ios::beg);
out_file.read(buf,bytes_to_send);
send(sock,buf,bytes_to_send,0);
free(buf);
}

int main()
{
run_server();

}
