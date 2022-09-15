#include "../includes.hpp"
#include <errno.h>


void Server::pass(std::string args, Client *User)
{
	if (User->isLog)
	{
		std::cout << "421 " << User->getnickname() << " PASS :Unknown command" << std::endl;
		return ;
	}
    
	if (args.compare(this->password) == 0)
		User->setPass = true;
}

void Server::nick(std::string args, Client *User)
{
	if (User->isLog)
		this->sendMessage(User->socketFD, ":" + User->getnickname() + "!" + User->getnickname() + "@" + inet_ntoa(User->clientAddr.sin_addr) + " NICK :" + args);	
	if (!User->setPass)
		return ;
	User->setNick = true;
	if (User->setUser && !User->isLog)
		this->clientLog(User->socketFD);
	User->setnickname(args);
	std::cout << "nick : " << args << std::endl;
}

void Server::user(std::string args, Client *User)
{
	if (!User->setPass)
		return ;
	if (User->isLog)
	{
		std::cout << "421 " << User->getnickname() << " USER :Unknown command" << std::endl;
		return ;
	}
	User->setUser = true;
	User->setusername(args);
	if (User->setNick)
		this->clientLog(User->socketFD);
	std::cout << "user : " << args << std::endl;
}

void Server::join (std::string args, Client *User)
{
    std::vector<std::string> splitargs;

    tokenize(args, ',', splitargs);
    for (size_t i = 0; i < splitargs.size(); i++)
    {
        std::cout << splitargs[i] << std::endl;
    }
	for (std::vector<std::string>::iterator it = splitargs.begin(); it != splitargs.end(); it++)
	{
		if (this->channelup.count(*it) == 0)
		{
			//create channel
			this->createChannel(*it, User);
		}
		else
		{
			//join channel
			this->joinChannel(*it, User);
		}
	}
}

channel * Server::isConnected(Client * user)
{
	for (channelIt it = this->channelup.begin(); it != this->channelup.end(); it++)
	{
		// if (it->second->isInChannel(user))
		// 	return ()
	}
	return NULL;
}

void Server::quit(std::string args, Client *User)
{
	this->disconnectClient(User->socketFD);
}

void Server::ping(std::string args, Client *User)
{
	this->sendMessage(User->socketFD, ":" + User->getnickname() + "!" + User->getnickname() + "@" + inet_ntoa(User->clientAddr.sin_addr) + " PONG ft_irc " + args);	
}

void Server::part(std::string args, Client *User)
{
	this->leaveChannel(args, User);
	this->sendMessage(User->socketFD, ":" + User->getnickname() + "!" + User->getnickname() + "@" + inet_ntoa(User->clientAddr.sin_addr) + " PART " + args);	
}
