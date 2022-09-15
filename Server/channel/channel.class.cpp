#include "channel.class.hpp"

channel::channel (std::string channelname, Client *firstuser):_channelname(channelname)
{
    this->_connectedClient.insert(std::make_pair(firstuser->socketFD, firstuser));
    this->_connectedClientp.insert(std::make_pair(firstuser->socketFD, firstuser));
}

void channel::newuser(Client *User)
{
    this->_connectedClient.insert(std::make_pair(User->socketFD, User));
    return;
}

channel::~channel(void)
{
}