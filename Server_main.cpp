
#include "Server_main.h"






mutex init_lock;

void Client::calc_data()
{
    multiset<int> values;
    stringstream nums;

    string data;
    for(auto it=msg_part.begin(); it!=msg_part.end(); it++) data+=it->second;

    data.erase
    (remove_if(data.begin(), data.end(), [](unsigned char c)
                                                                {
                                                                    return !(isdigit(c)||isspace(c));
                                                                }
                ),
                    data.end());

    data+=" end";
    nums<<data;
    data.clear();
    for(int temp = 0; nums >> temp; )
    {
        values.insert(temp);
    }
    if(values.size()==0)
    {
        int i=1;
        for(auto it=msg_part.begin();it!=msg_part.end();it++)
        {
            it->second+=(char)i;
            i++;
        }
        return;
    }
    int sum=0;
    for(auto it=values.begin(); it!=values.end(); it++)
    {
        data+=to_string(*it)+" ";
        sum+=*it;
    }
    data+="\n"+to_string(sum);

 //   cout<<"REPLY: "<<data<<endl;
    sum=data.length()/(MSG_SIZE-1)+1;
    msg_part.clear();
    msg_part.resize(sum);

    for(int i=0; i<sum; i++)
    {
        msg_part[i]=make_pair(false,data.substr(i*(MSG_SIZE-1),(MSG_SIZE-1)) + char(i+1));
      //  cout<<msg_part[i].second<<" #"<<i<<" bacup:"<<msg_part[i].second.at(0)<<endl;
    }
}

bool Client:: is_recived()
{
/*
    if(debug)
    {
        for(auto it=msg_part.begin(); it!=msg_part.end(); it++)
        {
         //   cout<<it->second<<"\t"<<it->first<<endl;
        }
        debug=false;

    }
    cout<<"CHECK "<<msg_part.size()<<endl;
    */
    for(auto it=msg_part.begin(); it!=msg_part.end(); it++)
    {
        if(!it->first) return false;
    }
    this->state=SEND_SIZE;
    return true;
}


char* Client::size_send()
{
    char buf[2]={(char)msg_part.size(),(char)1};
    return buf;

}


void Client::put_in_msg(string part)
{
    unsigned int index=(unsigned int)part.at(part.length()-1)-1;
    if(index==254) return;
    part.pop_back();
    if(msg_part.size()<=index)
    {
        msg_part.resize(index+1);
      //  cout<<"Put: "<<part<<" index "<<index<<endl;
    }


    if(!msg_part[index].first)
    {
        msg_part[index].first=true;
        msg_part[index].second=part;
    }

}



int main()
{

    thread udp_thread(udp_server);
    thread tcp_thread(tcp_server);

    printf("Startup complete. Ready to go.\n");
    tcp_thread.join();
    udp_thread.join();

    return 0;
}



