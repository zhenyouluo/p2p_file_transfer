#include<iostream>
#include<fstream>
#include<unistd.h> //for close
#include<sys/types.h>
#include<sys/socket.h>
#include<memory.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/ip.h>
#include<netinet/tcp.h> //for IPROTO_TCP
#include"utility1.h"
#include<cstdlib>
#include<cstring>

using namespace std;

static struct addrinfo hints,*list;
static int server_socket=-1;
static int clear_value=1;
int send_frags(int sock,char *buf);
int handle_request(int sock,char *buf,size_t size,int flag);

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
int listen_st=listen(server_socket,MAX_ALLOWED_CONN);
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
getbasedir();
struct sockaddr_in client_addr;
socklen_t size_client=sizeof(client_addr);
int socket_2_talk;
int ttl_connection=0;
int bytes_read=0;

while(1)
{
cout<<"Ready to accept new connections\n";
char ip[20];
socket_2_talk=accept(server_socket,(struct sockaddr*)&client_addr,&size_client);
if(socket_2_talk==-1){
cout<<"Connection not accepted\n";
continue;
}
inet_ntop(AF_INET,&client_addr.sin_addr,ip,INET_ADDRSTRLEN);
setsockopt(socket_2_talk,SOL_SOCKET,SO_REUSEADDR,&clear_value,sizeof(int));
if(socket_2_talk!=-1)
{
cout<<"connection_created with client "<<ip<<"\n";
}
char buf[max_buffer_size];
bytes_read=recv(socket_2_talk,buf,max_buffer_size,0);
if(bytes_read==-1)
{
cout<<"Error in reading buffer\n";
close(socket_2_talk);
return;
}
int st=handle_request(socket_2_talk,buf,max_buffer_size,0);
if(st==-1){
cout<<"Error in handling request\n";
close(socket_2_talk);
continue;
}

close(socket_2_talk);

}

close(server_socket);
}

int send_frags(int sock,char *file_name)
{
fstream out_file(file_name,ios::in);
file_meta meta;
int bytes_to_send;

out_file.read((char *)&meta,sizeof(file_meta));
bytes_to_send=((file_meta*)&meta)->size;

char *buf=(char*)calloc(1,sizeof(char)*(bytes_to_send+1));
if(!buf){
cout<<"memory not allocated\n";
}
out_file.seekg(0,ios::beg);
out_file.read(buf,bytes_to_send);
int bytes_sent=send(sock,buf,bytes_to_send,0);
free(buf);
out_file.close();
return bytes_sent;
}

void print_packet(control_packet *pkt)
{
cout<<"Packet_type "<<ntohl(pkt->packet_type)<<endl;
cout<<"Packet_id "<<ntohl(pkt->packet_id)<<endl;
cout<<"First Arg "<<pkt->first_arg<<endl;
cout<<"Second Arg "<<pkt->second_arg<<endl;
}

int handle_request(int sock,char *buf,size_t size,int flag)
{
char file_name[MAX_FILE_NAME];
char arg1_rcvd[MAX_ARG_SIZE];
char arg2_rcvd[MAX_ARG_SIZE];
char buf1[max_buffer_size];
long long chunks;
int bytes_sent,bytes_read;
control_packet *pkt;
control_packet pkt_to_send;
int pkt_type;
int pkt_id;
int req1=-1,req2=-1;

pkt=(control_packet*)buf;
pkt_type=ntohl(pkt->packet_type);
pkt_id=ntohl(pkt->packet_id);

for(int i=0;i<8;++i)
{
if((pkt_id & 1<<i) && req1==-1) req1=i;
else req2=i; 
}

if(req1==0)
{
strcpy(arg1_rcvd,pkt->first_arg);
strcpy(arg2_rcvd,pkt->second_arg);
strcpy(file_name,arg1_rcvd);
//max_buffer_size=atoi(arg2_rcvd);
int st=disassemble_file(file_name,chunks);
if(st==-1){
cout<<"Disassemble failed\n";
return -1;
}

//send no of chunks
memset(&pkt_to_send,0,sizeof(control_packet));
pkt_to_send.packet_type=htonl(info);
pkt_to_send.packet_id=htonl(no_of_chunks);
sprintf(pkt_to_send.first_arg,"%lld",chunks);

bytes_sent=send(sock,(char*)&pkt_to_send,sizeof(pkt_to_send),0);
if(bytes_sent==-1)
{
remove_files(file_name,chunks);
cout<<"Error while sending no of chunks to client\n";
return -1;
}

//check if client is ready to recieve
bytes_read=recv(sock,buf1,sizeof(buf1),0);
pkt=(control_packet*)buf1;
if(ntohl(pkt->packet_id)!=ready_to_recieve)
{
remove_files(file_name,chunks);
cout<<"Client not ready to recieve\n";
return -1;
}
else {
//start sending chunks
char tmp_name[2*MAX_FILE_NAME];
for(long long i=1;i<=chunks;++i)
{
sprintf(tmp_name,"%s/.%s$%lld.tmp",basedir_s,file_name,i);
bytes_sent=send_frags(sock,tmp_name);
if(bytes_sent==-1) {
remove_files(file_name,chunks);
cout<<"Error while sending file\n";
return -1;
}
memset(buf1,0,sizeof(buf1));
bytes_read=recv(sock,buf1,sizeof(control_packet),0);
if(bytes_read==-1)
{
remove_files(file_name,chunks);
cout<<"Error while Recieving\n";
return -1;
}
else{
pkt=(control_packet*)buf1;
if(ntohl(pkt->packet_id)==prev_packet_recieved)
{
remove(tmp_name);
}
else {
cout<<"Error while transmitting file chunks\n";
return -1;
}

}

}

}

}
else {
cout<<"Handshaking not done\n";
return -1;
}
cout<<"Request fulfilled\n";
return 0;
}

int main()
{
run_server();

}
