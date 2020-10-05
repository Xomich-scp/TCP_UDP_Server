#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <set>
#include <arpa/inet.h>
#include <mutex>
#include <thread>
#include <cstdlib>
#include <string>
#include <iostream>
#include <set>
#include <list>
#include <unordered_map>
#include <sstream>
#include <cstring>
#include <vector>
#define MSG_SIZE 2
using namespace std;

void tcp_server();
void udp_server();

extern mutex init_lock;
enum linkState{WAIT_SIZE,READ_DATA,WAIT_REPLY,SEND_SIZE,SEND_DATA,WAIT_FIN};

class Client
{
public:
        vector<pair<bool,string>> msg_part;
        void calc_data();
        bool is_active=false;
        bool is_recived();
        void put_in_msg(string part);
        char* size_send();
        bool debug=true;
        linkState state=WAIT_SIZE;
}
;




class client_udp: public Client
{
public:
        sockaddr_in cli_addr;
        void get_msg(char* input);
        client_udp(sockaddr_in value, string& input_data)
        {
            cli_addr=value;
            state=READ_DATA;
           // cout<<"CREATE CLI DATA= "<<input_data.at(0)<<" "<<(int)input_data.at(0)<<endl;
            msg_part.resize((int)input_data.at(0));
        }

};

class client_tcp: public Client
{
public:
        int cli_sock;
        void sendall();
        client_tcp(int sock)
        {
            cli_sock=sock;
        }
};
