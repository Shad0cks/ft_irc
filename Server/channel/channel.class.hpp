#ifndef CHANNEL_CLASS_HPP
#define CHANNEL_CLASS_HPP

#include "../client/client.class.hpp"
#include "../includes.hpp"
class Client;

class channel
{        
        
    public:
        typedef  std::map<int, Client *>::iterator    channelIt;
        std::map<int, Client *>     _connectedClient;
        std::map<int, Client *>     _connectedClientp;
        const std::string           _channelname;
        channel(std::string channelname, Client *firstclient);
        virtual ~channel(void);

        void        newuser(Client *user);
        void        part(Client *user);
        std::string const getchannelname(void)      {return(this->_channelname);}
        bool isInChannel(Client * user);

};

#endif