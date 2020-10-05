
#include "Server_main.h"


bool operator == (sockaddr_in a, sockaddr_in b)
{
    if(a.sin_addr.s_addr==b.sin_addr.s_addr && a.sin_port==b.sin_port) return 1;
    else return 0;
}

void client_udp::get_msg(char* input)
{
    strcpy(input,msg_part[input[0]-1].second.c_str());
}

void udp_server()
{
    int udp_sock;
    struct sockaddr_in addr;
    char *buf=(char*)calloc(MSG_SIZE,sizeof(char));



    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_sock<0)
    {
        perror("socket");
        exit(1);
    }


    fcntl(udp_sock, F_SETFL, O_NONBLOCK);

    string ip;
    int port=4000;

    init_lock.lock();
    cout<<"UDP:type no to change default addr"<<ip<<":"<<port;
    cin>>ip;
    if(ip.compare("no")==0)
    {
        cout<<"IP: ";
        cin>>ip;
        cout<<endl<<"Port: ";
        cin>>port;
        cout<<endl;
    }
    else

    ip="127.0.0.2";
    init_lock.unlock();
    inet_aton(ip.c_str(), &addr.sin_addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);


    if(bind(udp_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }



    printf("Server info\nIP:%s\nPort:%d\n",inet_ntoa(addr.sin_addr) ,ntohs(addr.sin_port));


    vector<client_udp> list_of_udps; //Better rewrite to map. need hash and < stuf





    while(1)
    {
        // Заполняем множество сокетов
        fd_set readset, writeset;
        FD_ZERO(&readset);
        FD_SET(udp_sock, &readset);

        // Задаём таймаут
        timeval timeout;
        timeout.tv_sec = 600;
        timeout.tv_usec = 0;


        if(select(udp_sock+1, &readset, &writeset, NULL, &timeout) <= 0)
        {
            perror("select");
            exit(3);
        }



    if(FD_ISSET(udp_sock, &readset))
        {

            sockaddr_in new_udp;



            client_udp *cli_ptr=NULL;
            unsigned int trash=sizeof(new_udp);
            bzero(buf,MSG_SIZE);
            recvfrom(udp_sock, buf, MSG_SIZE, 0, (struct sockaddr*)&new_udp, &trash);

            string new_data=string(buf);

            for(auto it=list_of_udps.begin(); it!=list_of_udps.end();it++)
            {
                if(it->cli_addr==new_udp)
                {
                    cli_ptr=&*it;

                    switch(cli_ptr->state)
                    {
                        case SEND_DATA:if(buf[0]==-1)list_of_udps.erase(it); else cli_ptr->get_msg(buf);break;

                        case SEND_SIZE:if(buf[0]==-1)cli_ptr->state=SEND_DATA;
                                        else
                                            {
                                                bzero(buf,MSG_SIZE);
                                                buf[0]=(char)cli_ptr->msg_part.size();
                                               // cout<<"SIZE SEND "<<cli_ptr->msg_part.size()<<" "<<buf[0]<<" "<<(int)buf[0]<<"<\n";
                                            }
                                            break;

                        case READ_DATA:cli_ptr->put_in_msg(new_data);
                                                                     if(cli_ptr->is_recived())
                                                                        {
                                                                            cli_ptr->calc_data();
                                                                            cli_ptr->state=SEND_SIZE;
                                                                        };
                                                                    break;
                    }


                   // cout<<"Will send>"<<string(buf)<<"<\n";
                    sendto(udp_sock, buf,MSG_SIZE,0,(struct sockaddr*)&new_udp,sizeof(new_udp));
                    break;
                }
            }


            if(cli_ptr==NULL)
            {
                cli_ptr=new client_udp(new_udp, new_data);

                list_of_udps.push_back(*cli_ptr);

                sendto(udp_sock, buf,MSG_SIZE,0,(struct sockaddr*)&new_udp,sizeof(new_udp));
            }


        }
    }

}
