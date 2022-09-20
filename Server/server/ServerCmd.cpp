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
	else
		this->sendMessage(User->socketFD, "Wrong password");	
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
		if (it->front() != '#' && it->size() <= 1)
			continue;
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
	std::vector<std::string> splitargs;

    tokenize(args, ':', splitargs);
	splitargs[0].erase(remove_if(splitargs[0].begin(), splitargs[0].end(), isspace));
	this->leaveChannel(splitargs[0], User);
	this->sendMessage(User->socketFD, ":" + User->getnickname() + "!" + User->getnickname() + "@" + inet_ntoa(User->clientAddr.sin_addr) + " PART " + splitargs[0] + " :" + splitargs[1]);	
}

void Server::privmsg(std::string args, Client *User)
{
	std::vector<std::string> splitargs;
	std::vector<std::string> channels;
	std::string message;
	std::string channelsString;

	tokenize(args, ':', splitargs);
	message = splitargs[1];
	splitargs[0].erase(remove_if(splitargs[0].begin(), splitargs[0].end(), isspace));
	tokenize(splitargs[0], ',', channels);

	for (size_t i = 0; i < channels.size(); i++)
    {
		this->sendMessageChannel(message, channels[i], User);
    }
}

void Server::pong(std::string args, Client *User)
{
	this->sendMessage(User->socketFD, ":" + User->getnickname() + "!" + User->getnickname() + "@" + inet_ntoa(User->clientAddr.sin_addr) + " PING ft_irc " + args);	
}