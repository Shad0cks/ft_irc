#include "../includes.hpp"
#include <errno.h>


void Server::pass(std::string args, Client *User)
{
	if (!User->isLog && !User->setNick && !User->setUser && !User->setPass)
	
	if (args == this->password)
		User->setPass = true;
}

void Server::nick(std::string args, Client *User)
{
	if (!User->setPass)
		return ;
	User->setNick = true;
	User->setnickname(args);
	if (User->setUser)
		this->clientLog(User->socketFD);
}

void Server::user(std::string args, Client *User)
{
	if (!User->setPass)
		return ;
	User->setUser = true;
	User->setusername(args);
	if (User->setNick)
		this->clientLog(User->socketFD);
}

void Server::join(std::string args, Client *User)
{
	
}

void Server::quit(std::string args, Client *User)
{
	this->disconnectClient(User->socketFD);
}