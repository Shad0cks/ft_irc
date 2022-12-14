#include "../includes.hpp"
#include <errno.h>
#define RED "\e[0;31m"

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
	std::size_t found = args.find('#');
	std::size_t found2 = args.find('@');
	if (found != std::string::npos || found2 != std::string::npos)
		return;

	if (User->isLog)
		this->sendMessage(User->socketFD, ":" + User->getnickname() + "!" + User->getnickname() + "@" + inet_ntoa(User->clientAddr.sin_addr) + " NICK :" + args);	
	if (!User->setPass)
		return ;
	User->setNick = true;
	if (User->setUser && !User->isLog)
		this->clientLog(User->socketFD);
	User->setnickname(args);
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
		{
			this->sendMessage(User->socketFD, "403 " + User->getnickname() + " " + args + ":No such channel");
			continue;
		}
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
	//QUIT :bye
	//:pdeshaye!pdeshaye@127.0.0.1 QUIT pdeshaye: :bye
	std::string message;
	std::vector<std::string> splitargs;
    tokenize(args, ' ', splitargs);
	for (size_t i = 1; i < splitargs.size(); i++)
	{
		message += splitargs[i];
		if (i + 1 != splitargs.size())
			message += " ";
	}
	if (message.empty())
		message = ": bye";
	for (clientIt it = this->connectedClient.begin(); it != this->connectedClient.end(); it++)
		this->sendMessage(it->first, ":" + User->getnickname() + "!" + User->getnickname() + "@" + inet_ntoa(User->clientAddr.sin_addr) + " QUIT " + User->getnickname() + ": " + message);	
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
	if (this->channelup.count(splitargs[0]) == 0)
	{
		this->sendMessage(User->socketFD, "403 " + User->getnickname() + " " + args + ":No such channel");
		return;
	}
	this->leaveChannel(splitargs[0], User);
}

void Server::privmsg(std::string args, Client *User)
{
	std::vector<std::string> splitargs;
	std::vector<std::string> channels;
	std::string message;
	std::string channelsString;
	Client * target;

	tokenize(args, ':', splitargs);
	message = splitargs[1];
	splitargs[0].erase(remove_if(splitargs[0].begin(), splitargs[0].end(), isspace));
	tokenize(splitargs[0], ',', channels);

	for (size_t i = 0; i < channels.size(); i++)
    {
		if (channels[i].front() == '#')
		{
			if (this->channelup.count(channels[i]) == 1)
			{
				if (this->channelup[channels[i]]->isInChannel(User) > 0 || !this->channelup[channels[i]]->getcansendmsghc())
					this->sendMessageChannel(message, channels[i], User);
			}
			else
				this->sendMessage(User->socketFD, "403 " + User->getnickname() + " " + args + ":No such channel");
		}
		else
		{
			target = this->getClientByName(channels[i]);
			if (target)
			{
				this->sendMessage(target->socketFD, ":" + User->getnickname() + "!" + User->getnickname() + "@" + inet_ntoa(User->clientAddr.sin_addr) + " PRIVMSG " + target->getnickname() + " :" + message);	
				this->sendMessage(target->socketFD, message);
			}
			else
				this->sendMessage(User->socketFD, "401 " + User->getnickname() + " " + args + ":No such nickname");
		}
	}
}

void Server::notice(std::string args, Client *User)
{
	std::vector<std::string> splitargs;
	std::vector<std::string> channels;
	std::string message;
	std::string channelsString;
	Client * target;

	tokenize(args, ':', splitargs);
	message = splitargs[1];
	splitargs[0].erase(remove_if(splitargs[0].begin(), splitargs[0].end(), isspace));
	tokenize(splitargs[0], ',', channels);
	if (this->channelup.count(splitargs[0]) == 0)
	{
		this->sendMessage(User->socketFD, "403 " + User->getnickname() + " " + args + ":No such channel");
		return;
	}
	for (size_t i = 0; i < channels.size(); i++)
    {
		if (channels[i].front() == '#')
		{
			
			if (this->channelup.count(channels[i]) > 0 && (this->channelup[channels[i]]->isInChannel(User) > 0 || !this->channelup[channels[i]]->getcansendmsghc()))
				this->sendNoticeChannel(message, channels[i], User);
		}
		else
		{
			target = this->getClientByName(channels[i]);
			if (target)
			{
				this->sendMessage(target->socketFD, ":" + User->getnickname() + "!" + User->getnickname() + "@" + inet_ntoa(User->clientAddr.sin_addr) + " NOTICE " + target->getnickname() + " :" + message);	
				this->sendMessage(target->socketFD, message);
			}
		}
	}
}

void Server::mode(std::string args, Client *User)
{
	std::vector<std::string> splitargs;
	tokenize(args, ' ', splitargs);
	splitargs[0].erase(remove_if(splitargs[0].begin(), splitargs[0].end(), isspace));
	Client *target;
	if (splitargs[0].front() == '#')
	{
		if (this->channelup.count(splitargs[0]) == 0)
		{
			this->sendMessage(User->socketFD, "403 " + User->getnickname() + " " + args + ":No such channel");
			return;
		}
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
									// std::cout << "Sizeqweqweqweqw: " <<  "\n";
									this->channelup[splitargs[0]]->setlimituser(true);
									this->channelup[splitargs[0]]->setlimite(std::stoi(splitargs[3]));
								}
								else
									return;
							}	
					}
					else if (splitargs[2][i] == 'o' && splitargs.size() == 4)
					{
						target = getClientByName(splitargs[3]);
						if (target)
							this->channelup[splitargs[0]]->newuserp(target);
						else
						{
							this->sendMessage(User->socketFD, "401 " + User->getnickname() + " " + args + ":No such nickname");
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
						this->channelup[splitargs[0]]->setcansendmsghc(false);
					else if (splitargs[2][i] == 'Q')
						this->channelup[splitargs[0]]->setcankick(false);
					else if (splitargs[2][i] == 'k')
						this->channelup[splitargs[0]]->setneedpassword(false);
					else if (splitargs[2][i] == 'l')
						this->channelup[splitargs[0]]->setlimituser(false);
					else if (splitargs[2][i] == 'o' && splitargs.size() == 4)
					{
						target = getClientByName(splitargs[3]);
						if (target)
							this->channelup[splitargs[0]]->eraseop(target);
						else
						{
							this->sendMessage(User->socketFD, "401 " + User->getnickname() + " " + args + ":No such nickname");
							return;
						}
					}
					else
						return;
				}
			}
		}
		else
			return;
	}
	else
	{
		target = getClientByName(splitargs[0]);
		if (!target)
		{
			this->sendMessage(User->socketFD, "401 " + User->getnickname() + " " + args + ":No such nickname");
			return;
		}
		if (splitargs.size() == 1)
			return;
		else if (splitargs.size() == 2)
			return;
		if(splitargs[1][0] == '+' && splitargs.size() != 1)
		{
			for (int i = 0; i < splitargs[2].size(); i++)
			{
				if (splitargs[2][i] == 'o')
				{
					target->setoperator(true);
				}
			}
		}
		else if (splitargs[1][0] == '-' && splitargs.size() != 1)
		{
			for (int i = 0; i < splitargs[2].size(); i++)
			{
				if (splitargs[2][i] == 'o')
					target->setoperator(false);
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

void Server::names(std::string args, Client *User)
{
	args.erase(remove_if(args.begin(), args.end(), isspace));
	std::string buffer;
	if (this->channelup.count(args) == 0)
	{
		this->sendMessage(User->socketFD, "366 " + User->getnickname() + " " + args + " :End of NAMES list");
		return;
	}
	for (std::map<int, Client *>::iterator it = this->channelup[args]->_connectedClient.begin(); it != this->channelup[args]->_connectedClient.end(); it++)
	{
		if (this->channelup[args]->isModo(it->second))
			buffer = buffer + "@" + it->second->getnickname() + " ";
		else
			buffer = buffer + "" + it->second->getnickname() + " ";
	}
	this->sendMessage(User->socketFD, "353 " + User->getnickname() + " " + args + " :" + buffer);
	this->sendMessage(User->socketFD, "366 " + User->getnickname() + " " + args + " :End of NAMES list");
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
	Client * target = this->channelup[splitargs[0]]->getClientByNameInChannel(splitargs[1]);
	if (!target)
	{
		this->sendMessage(User->socketFD, "401 " + User->getnickname() + " " + args + ":No such nickname");
		return;
	}
	if (this->channelup[splitargs[0]]->isModo(User))
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


void Server::kill(std::string args, Client *User)
{
	std::string message;
	std::vector<std::string> splitargs;
    tokenize(args, ' ', splitargs);
	for (size_t i = 1; i < splitargs.size(); i++)
	{
		message += splitargs[i];
		if (i + 1 != splitargs.size())
			message += " ";
	}
	Client * target = this->getClientByName(splitargs[0]);
	if (!target)
	{
		this->sendMessage(User->socketFD, "401 " + User->getnickname() + " " + args + ":No such nickname");
		return;
	}
	if (User->getoperator())
	{
		if (message.empty())
			message = ":bye kill one";
		for (clientIt it = this->connectedClient.begin(); it != this->connectedClient.end(); it++)
		{
			this->sendMessage(it->first, ":" + User->getnickname() + "!" + User->getnickname() + "@" + inet_ntoa(User->clientAddr.sin_addr) + " KILL " + splitargs[0] + " " + target->getnickname() + " " + message);	
		}
		this->quit("QUIT :Client kicked by " + User->getnickname(), target);
	}
}

void Server::topic(std::string args, Client *User)
{
	//TOPIC #hey :salut a tous
	//sent : :pdeshaye!pdeshaye@127.0.0.1 TOPIC #hey :salut a tous
	std::vector<std::string> splitargs;
    tokenize(args, ':', splitargs);
	splitargs[0].erase(remove_if(splitargs[0].begin(), splitargs[0].end(), isspace));
	if (this->channelup.count(splitargs[0]) == 0)
	{
		this->sendMessage(User->socketFD, "403 " + User->getnickname() + " " + args + ":No such channel");
		return;
	}
	for (std::map<int, Client *>::iterator it = this->channelup[splitargs[0]]->_connectedClient.begin(); it != this->channelup[splitargs[0]]->_connectedClient.end(); it++)
	{
		this->sendMessage(it->first, ":" + User->getnickname() + "!" + User->getnickname() + "@" + inet_ntoa(User->clientAddr.sin_addr) + " TOPIC " + splitargs[0] + " :" + splitargs[1]);	
	}
	this->channelup[splitargs[0]]->WelcomeMessage = splitargs[1];
}