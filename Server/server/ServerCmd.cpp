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
	std::string mod;
	tokenize(args, ',', splitargs);
	std::size_t found = splitargs[splitargs.size() - 1].find(' ');

	if (found != std::string::npos)
	{
		mod = splitargs[splitargs.size() - 1].substr(found, splitargs[splitargs.size() - 1].size());
		splitargs[splitargs.size() - 1] = splitargs[splitargs.size() - 1].substr(0, found);
	}
	for (std::vector<std::string>::iterator it = splitargs.begin(); it != splitargs.end(); it++)
	{
		if (it->front() != '#' || it->size() <= 1)
			continue;
		if (this->channelup.count(*it) == 0)
		{
			//create channel
			this->createChannel(*it, User, mod);
		}
		else
		{
			if (this->channelup[*it]->getneedpassword())
			{
				if(*++it == this->channelup[*--it]->getpassword())
				{
					if (this->channelup[*it]->getlimituser())
					{
						if (this->channelup[*it]->getlimite() > this->channelup[*it]->getnbuser() + 1)
							this->joinChannel(*it, User, mod);
					}
					else
						this->joinChannel(*it, User, mod);
				}
			}
			else if (this->channelup[*it]->getlimituser())
			{
				if (this->channelup[*it]->getlimite() > this->channelup[*it]->getnbuser() + 1)
					this->joinChannel(*it, User, mod);
			}
			else
				this->joinChannel(*it, User, mod);
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
		if (this->channelup.count(channels[i]) > 0 && (this->channelup[channels[i]]->isInChannel(User) > 0 || this->channelup[channels[i]]->getcansendmsghc()))
			this->sendMessageChannel(message, channels[i], User);
	}
}

void Server::mode(std::string args, Client *User)
{
	std::vector<std::string> splitargs;
	tokenize(args, ' ', splitargs);
	if (splitargs[0].front() == '#')
	{
		splitargs[0].erase(remove_if(splitargs[0].begin(), splitargs[0].end(), isspace));
		if (splitargs.size() == 1)
			return;
		if(splitargs[1][0] == '+' && splitargs.size() != 1)
		{
			if (splitargs.size() == 2)
				return;
			else
			{
				for (int i = 0; i < splitargs[2].size(); i++)
				{
					if (splitargs[2][i] == 'n')
						this->channelup[splitargs[0]]->setcansendmsghc(true);
					else if (splitargs[2][i] == 'Q')
						this->channelup[splitargs[0]]->setcankick(true);
					else if (splitargs[2][i] == 'k')
						{
							if (splitargs[2].size() != 1 || splitargs.size() == 3)
								return;
							else
							{
								this->channelup[splitargs[0]]->setneedpassword(true);
								this->channelup[splitargs[0]]->setpassword(splitargs[3]);
							}	
						}
					else if (splitargs[2][i] == 'l')
					{
							if (splitargs[2].size() != 1 || splitargs.size() == 3)
								return;
							else
							{
								if (std::stoi(splitargs[3]) > 0)
								{
									this->channelup[splitargs[0]]->setlimituser(true);
									this->channelup[splitargs[0]]->setlimite(std::stoi(splitargs[3]));
								}
								else
									return;
							}	
					}
					else
						return;
				}
			}
		}
		else if(splitargs[1][0] == '-' && splitargs.size() != 1)
		{
			if (splitargs.size() == 2)
				return;
			else
			{
				for (int i = 0; i < splitargs[2].size(); i++)
				{
					if (splitargs[2][i] == 'n')
					{
						this->channelup[splitargs[0]]->setcansendmsghc(false);
					}
					else if (splitargs[2][i] == 'Q')
						this->channelup[splitargs[0]]->setcankick(false);
					else if (splitargs[2][i] == 'k')
						this->channelup[splitargs[0]]->setneedpassword(false);
					else if (splitargs[2][i] == 'l')
						this->channelup[splitargs[0]]->setlimituser(false);
					else
						return;
				}
			}
		}
		else
			return;
	}
}

void Server::pong(std::string args, Client *User)
{
	this->sendMessage(User->socketFD, ":" + User->getnickname() + "!" + User->getnickname() + "@" + inet_ntoa(User->clientAddr.sin_addr) + " PING ft_irc " + args);	
}

void Server::kick(std::string args, Client *User)
{
	std::string message;
	std::vector<std::string> splitargs;
    tokenize(args, ' ', splitargs);
	for (size_t i = 2; i < splitargs.size(); i++)
	{
		message += splitargs[i];
		if (i + 1 != splitargs.size())
			message += " ";
	}	
	Client * target = this->channelup[splitargs[0]]->getClientByName(splitargs[1]);
	if (target && this->channelup[splitargs[0]]->isModo(User))
	{
		if (message.empty())
			message = ":bye kicked one";
		for (std::map<int, Client *>::iterator it = this->channelup[splitargs[0]]->_connectedClient.begin(); it != this->channelup[splitargs[0]]->_connectedClient.end(); it++)
		{
			this->sendMessage(it->first, ":" + User->getnickname() + "!" + User->getnickname() + "@" + inet_ntoa(User->clientAddr.sin_addr) + " KICK " + splitargs[0] + " " + target->getnickname() + " " + message);	
		}
		this->leaveChannel(splitargs[0], target);
	}

}