#include<iostream>
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
using namespace std;

struct addrinfo hints,*list;
int client_socket=-1;
long long max_socket_buf_size;

void print_packet(control_packet *pkt)
{
cout<<"Packet_type "<<ntohl(pkt->packet_type)<<endl;
cout<<"Packet_id "<<ntohl(pkt->packet_id)<<endl;
cout<<"First Arg "<<pkt->first_arg<<endl;
cout<<"Second Arg "<<pkt->second_arg<<endl;
}

int init_trnsfr(int sock,char *file_name,long long sock_size)
{
long long bytes_sent,bytes_rcvd;
char buf[max_buffer_size];
long long file_chunks;
control_packet pkt,*pkt_rcvd;
memset(&pkt,0,sizeof(control_packet));

pkt.packet_type=htonl(req);
pkt.packet_id=htonl(file_transfer);
strcpy(pkt.first_arg,file_name);
sprintf(pkt.second_arg,"%lld",sock_size);

bytes_sent=send(sock,(char*)&pkt,sizeof(pkt),0);
if(bytes_sent==-1)
{
cout<<"Error In sending control packet\n";
return -1;
}
bytes_rcvd=recv(sock,buf,sizeof(control_packet),0);
pkt_rcvd=(control_packet*)buf;

if(ntohl(pkt_rcvd->packet_id)!=no_of_chunks)
{
cout<<"Error getting no_of_chunks from server\n";
return -1;
}
file_chunks=atoi(pkt_rcvd->first_arg);

//send ready to recieve packet
pkt.packet_type=htonl(info);
pkt.packet_id=htonl(ready_to_recieve);

bytes_sent=send(sock,(char *)&pkt,sizeof(control_packet),0);
if(bytes_sent==-1){
cout<<"Error sending ready_to_recieve_packet";
return -1;
}

//recieving file segments
for(int i=1;i<=file_chunks;++i)
{
bytes_rcvd=recv_all(sock,buf,max_buffer_size,0);
if(bytes_rcvd==-1)
{
cout<<"Error Recieving file segments\n";
return -1;
}

pkt.packet_type=htonl(info);
pkt.packet_id=htonl(prev_packet_recieved);
sprintf(pkt.first_arg,"%lld",bytes_rcvd);

bytes_sent=send(sock,(char*)&pkt,sizeof(pkt),0);
if(bytes_sent==-1)
{
cout<<"Could not acknowledge packet delivery\n";
//TODO some error handling can be done
return -1;
}
create_files(buf);

}
assemble_files(file_name,file_chunks);

}

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
unsigned int size_unsigned=sizeof(long long);
getsockopt(client_socket,SOL_SOCKET,SO_RCVBUF,(void *)&max_socket_buf_size,&size_unsigned);

if(connect_st==-1)
{
cout<<"Problem Connecting\n";
continue;
}
}
char file_name[MAX_FILE_NAME];
cout<<"Enter File Name to copy\n";
cin>>file_name;
cout<<"Enter Directory to store file in\n";
cin>>basedir_c;
init_trnsfr(client_socket,file_name,max_socket_buf_size);

close(client_socket);
}

int main()
{
create_socket();
}
