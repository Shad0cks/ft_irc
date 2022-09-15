#ifndef CHANNEL_CLASS_HPP
#define CHANNEL_CLASS_HPP

#include "../includes.hpp"

class channel
{
    private:
        std::map<int, Client *>     _connectedClient;
        std::map<int, Client *>     _connectedClientp;
        const std::string           _channelname;
    public:
        channel(std::string channelname, Client *firstclient);
        virtual ~channel(void);

        void        newuser(Client *user);
        std::string const getchannelname(void)      {return(this->_channelname);}
};

#endif