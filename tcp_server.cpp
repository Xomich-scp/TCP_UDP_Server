

#include "Server_main.h"



void client_tcp:: sendall()
{
   // cout<<"!!!SOCK: "<<cli_sock<<endl;
    int i=0;
    for(auto it=msg_part.begin() ;it!=msg_part.end();it++, i++)
    {
        send(cli_sock,it->second.c_str(),it->second.size(),0);
      //  cout<<it->second.c_str()<<" size:"<<it->second.size()<<" #"<<i<<" index:"<<(int)it->second.at(it->second.size()-1)<<endl;
        it->first=true;
    }
}


void tcp_server()
{

    int tcp_sock;
    struct sockaddr_in addr;
    char *buf=(char*)calloc(MSG_SIZE,sizeof(char));
    int bytes_read;

    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);

    if(tcp_sock < 0 )
    {
        perror("socket");
        exit(1);
    }

    fcntl(tcp_sock, F_SETFL, O_NONBLOCK);


    string ip;
    int port=3000;

    init_lock.lock();
    cout<<"TCP:type no to change default addr "<<ip<<":"<<port;
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
    //addr.sin_addr.s_addr = INADDR_ANY;
    inet_aton(ip.c_str(), &addr.sin_addr);
    addr.sin_family = AF_INET;


        addr.sin_port = htons(port);
    if(bind(tcp_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }





    printf("Server info\nIP:%s\nPort:%d\n",inet_ntoa(addr.sin_addr) ,ntohs(addr.sin_port));
    listen(tcp_sock, 10);



    unordered_map<int, client_tcp> list_of_tcps;



    set<int> clients_tcp_sock;

    clients_tcp_sock.clear();



    while(1)
    {
        // Заполняем множество сокетов
        fd_set readset, writeset;
        FD_ZERO(&readset);
        FD_SET(tcp_sock, &readset);


        for(set<int>::iterator it = clients_tcp_sock.begin(); it != clients_tcp_sock.end(); it++)
            {
                FD_SET(*it, &readset);
                FD_SET(*it, &writeset);
            }

        // Задаём таймаут
        timeval timeout;
        timeout.tv_sec = 600;
        timeout.tv_usec = 0;

        // Ждём события в одном из сокетов
        int mx = max({tcp_sock, *max_element(clients_tcp_sock.begin(), clients_tcp_sock.end())});
        if(select(mx+1, &readset, &writeset, NULL, &timeout) <= 0)
        {
            perror("select");
            exit(3);
        }

        // Определяем тип события и выполняем соответствующие действия
        if(FD_ISSET(tcp_sock, &readset))
        {
            // Поступил новый запрос на соединение, используем accept
            int sock = accept(tcp_sock, NULL, NULL);
            if(sock < 0)
            {
                perror("accept");
                exit(3);
            }

            //cout<<"NEW TCP\n";
            client_tcp new_client(sock);

            list_of_tcps.insert(pair<int,client_tcp>(sock, new_client));
            clients_tcp_sock.insert(sock);
            fcntl(sock, F_SETFL, O_NONBLOCK);
        }




        for(set<int>::iterator it = clients_tcp_sock.begin(); it != clients_tcp_sock.end(); it++)
        {
            if(FD_ISSET(*it, &readset))
            {
                // Поступили данные от клиента, читаем их
                bzero(buf,MSG_SIZE);
                bytes_read = recv(*it, buf, MSG_SIZE, 0);


                if(bytes_read <= 0)
                {
                    // Соединение разорвано, удаляем сокет из множества
                    close(*it);
                    list_of_tcps.erase(*it);
                    clients_tcp_sock.erase(*it);

                    continue;
                }

                string new_data=string(buf);
               // cout<<"DATA_read "<<new_data<<" "<<bytes_read<<" "<<*new_data.end()<<endl;
                client_tcp *cli_ptr=&(list_of_tcps.find(*it))->second;
                switch(cli_ptr->state)
                {
                    case WAIT_SIZE: cli_ptr->msg_part.resize((int)buf[0]);cli_ptr->state=READ_DATA; send(*it,buf,1,0);break;
                    case READ_DATA: cli_ptr->put_in_msg(new_data);break;
                    case WAIT_REPLY: cli_ptr->state=SEND_DATA; break;

                    default: break;
                }
                cli_ptr->debug=true;


            }

            if(FD_ISSET(*it,&writeset))
            {
                client_tcp *cli_ptr=&(list_of_tcps.find(*it))->second;


                switch(cli_ptr->state)
                {
                    case READ_DATA: if(cli_ptr->is_recived()) {cli_ptr->calc_data(); cli_ptr->state=SEND_SIZE;} break;
                    case SEND_SIZE: bzero(buf,MSG_SIZE);buf[0]=(char)cli_ptr->msg_part.size();send(*it,buf,1,0); cli_ptr->state=WAIT_REPLY; break;
                    case SEND_DATA: cli_ptr->sendall();cli_ptr->state=WAIT_SIZE;cli_ptr->msg_part.clear();break;
                    default: break;
                }

            }





        }


    }

}


