#include "channel.class.hpp"

channel::channel (std::string channelname, Client *firstuser):_channelname(channelname)
{
    this->_connectedClient.insert(std::make_pair(firstuser->socketFD, firstuser));
    this->_connectedClientp.insert(std::make_pair(firstuser->socketFD, firstuser));
    this->_cankick = false;
    this->_cansendmsghc = false;
    this->_limituser = false;
}

void channel::newuser(Client *User)
{
    this->_connectedClient.insert(std::make_pair(User->socketFD, User));
    return;
}

channel::~channel(void)
{

}


bool channel::isInChannel(Client * user)
{
    return (this->_connectedClient.count(user->socketFD) >= 1);
}

void        channel::part(Client *user)
{
    if (this->_connectedClientp.count(user->socketFD) > 0)
        this->_connectedClientp.erase(user->socketFD);
    if (this->_connectedClient.size() == 0)
        return ;
    if (this->_connectedClientp.size() > 0)
        return ;
    channelIt first = this->_connectedClient.begin();
    this->_connectedClientp.insert(std::make_pair(first->first, first->second));
}

std::string channel::getClientNames(void)
{
	std::string namesCompile;
	for (channelIt it = this->_connectedClient.begin(); it != this->_connectedClient.end(); it++)
	{
		namesCompile +=  "@" + it->second->getnickname() + " ";
	}
	return (namesCompile);
}