#include<iostream>
#include<fstream>
#include<cstring>
#include"utility.h"

using namespace std;

char basedir_s[MAX_FILE_NAME];
char basedir_c[MAX_FILE_NAME];

void getbasedir()
{
getcwd(basedir_s,MAX_FILE_NAME);
}

int disassemble_file(char *file_name, long long &chunks)
{
char tmp_name[MAX_FILE_NAME];
char tmp_file_name[2*MAX_FILE_NAME];
char buf[max_buffer_size];
file_meta frag_meta;
sprintf(tmp_file_name,"%s/%s",basedir_s,file_name);
fstream file_in(tmp_file_name,ios::in|ios::binary|ios::ate);
if(!file_in){
cout<<"Invalid File Name\n";
return -1;
}

long long file_size=file_in.tellg();
int frag_size=max_buffer_size-sizeof(file_meta);
chunks=(file_size/frag_size)+1;
int bytes_read;
file_in.seekg(0,ios::beg);

for(long long i=1;i<=chunks;++i)
{
sprintf(tmp_name,"%s/.%s%c%lld%c%s",basedir_s,file_name,'$',i,'.',"tmp");
file_in.read(buf,frag_size);
bytes_read=file_in.gcount();

strcpy(frag_meta.name,file_name);
frag_meta.frag_no=i;
frag_meta.size=(i!=chunks)?max_buffer_size:(file_size-(file_size/frag_size)*frag_size)+sizeof(frag_meta);

fstream tmp_handle(tmp_name,ios::out);
if(!tmp_handle){
cout<<"Cannot Create file "<<tmp_name<<endl;
file_in.close();
return -1;
}

tmp_handle.write((char *)&frag_meta,sizeof(file_meta));
tmp_handle.write(buf,bytes_read);
tmp_handle.close();
}

file_in.close();
return 0;
}

void assemble_files(char *file_name,int chunks)
{
char tmp_name[MAX_FILE_NAME];
char out_file_name[2*MAX_FILE_NAME];
char buf[max_buffer_size];
int bytes_read;
fstream out_file;

sprintf(out_file_name,"%s/%s",basedir_c,file_name);
out_file.open(out_file_name,ios::out|ios::app|ios::binary);
for(long long i=1;i<=chunks;++i){
if(!out_file)
{
cout<<"Could Not create File for copying\n";
}

sprintf(tmp_name,"%s/.%s%c%lld%c%s",basedir_c,file_name,'$',i,'.',"tmp");
fstream tmp_handle(tmp_name,ios::in);
if(!tmp_handle){
cout<<"Could Not Read File\n";
}

tmp_handle.seekg(sizeof(file_meta),ios::beg);
tmp_handle.read(buf,max_buffer_size);
bytes_read=tmp_handle.gcount();

out_file.write(buf,bytes_read);
tmp_handle.close();
remove(tmp_name);
}
out_file.close();
}

//store the buffer recieved, as a file of the format *$id.tmp required for reassembly
void create_files(char *buf1)
{
file_meta *meta_buf;
char tmp_name[2*MAX_FILE_NAME];
meta_buf=(file_meta *)buf1;
sprintf(tmp_name,"%s/.%s%c%lld%c%s",basedir_c,meta_buf->name,'$',meta_buf->frag_no,'.',"tmp");
fstream file_out(tmp_name,ios::out|ios::binary);
file_out.write(buf1,meta_buf->size);
file_out.close();
}

long long recv_all(int sock,char *buf,size_t ttl_bytes,int flag)
{
long long bytes_read=0;
long long ttl_bytes_to_read=0;
bytes_read=recv(sock,buf,ttl_bytes,flag);
file_meta *meta=(file_meta *)buf;
ttl_bytes_to_read=meta->size;

while(bytes_read<ttl_bytes_to_read)
{
bytes_read+=recv(sock,buf+bytes_read,ttl_bytes-bytes_read,0);
}
create_files(buf);
return bytes_read;
}

void remove_files(char *file_name,long long chunks)
{
char tmp_name[2*MAX_FILE_NAME];
for(long long i=1;i<=chunks;++i)
{
sprintf(tmp_name,"%s/.%s%c%lld%c%s",basedir_s,file_name,'$',i,'.',"tmp");
remove(file_name);
}

}

