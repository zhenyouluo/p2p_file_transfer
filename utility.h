#define MAX_FILE_NAME 200
#define MAX_ARG_SIZE 200
#define MAX_IP_SIZE 20
#define SERVER_PORT "8999"
#define MAX_ALLOWED_CONN 10
#define server_port "8999"

//pakcet ID's
#define file_transfer 1
#define file_not_found 1<<1
#define buffer_size_rply 1<<2
#define no_of_chunks 1<<3
#define ready_to_recieve 1<<4
#define prev_packet_recieved 1<<5
#define data_trnsfr_done 1<<6
#define prev_request_ack 1<<7

int max_buffer_size=64*1024;
char server_addr[MAX_IP_SIZE]= "192.168.0.11";

enum pkt_type{req,info};

void create_files(char *buf1);

struct file_meta{
long long frag_no;
long long size;
char name[MAX_FILE_NAME];
};

struct control_packet{
int packet_type;
int packet_id;
char first_arg[MAX_ARG_SIZE];
char second_arg[MAX_ARG_SIZE];
};


