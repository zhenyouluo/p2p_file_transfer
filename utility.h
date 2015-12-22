#define MAX_FILE_NAME 200
#define SERVER_PORT "8999"
#define max_buffer_size 64*1024
#define server_addr "192.168.0.11"
#define server_port "8999"
void create_files(char *buf1);

struct file_meta{
long long frag_no;
long long size;
char name[MAX_FILE_NAME];
};

struct control_packet{
int actions;
int length;
char name[MAX_FILE_NAME];
};
