#include "../includes.hpp"
#include <errno.h>


std::string counter_nl(std::string str)
{ 
	std::size_t found = str.find(13);
	std::string commande;

    if (found != std::string::npos)
        commande = str.substr(0, found);
	else
        commande = str;
	return commande;
}

Server::Server(std::string port, std::string pasword)
{
    int opt = 1;
    int flags;
    this->isRunning = true;
    this->port = std::stoi(port);
    this->password = pasword;
    if((this->socketFD = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        this->ExitFailure("could not create TCP listening socket");
    
    if(setsockopt(this->socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        this->ExitFailure("setsockopt", 1);

    if((flags = fcntl(this->socketFD, F_GETFL)) == -1)
        this->ExitFailure("could not get flags on TCP listening socket", 1);

    if(fcntl(this->socketFD, F_SETFL, flags | O_NONBLOCK))
        this->ExitFailure("could not set TCP listening socket to be non-blocking", 1);

    this->serverAddr.sin_port = htons(this->port);
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_addr.s_addr = INADDR_ANY;
        
    if (bind(this->socketFD, (struct sockaddr *)&this->serverAddr, sizeof(this->serverAddr)) < 0)
        this->ExitFailure("bind fail", 1);

    if (listen(this->socketFD, 3) < 0)
        this->ExitFailure("listen fail", 1);
}

Server::~Server(void)
{
    this->connectedClient.clear();
    this->ufds.clear();
    close(this->socketFD);
}

void Server::ExitFailure(std::string message)
{
    perror(message.c_str());
    exit(EXIT_FAILURE);
}

void Server::ExitFailure(std::string message, int closedSocket)
{
    perror(message.c_str());
    close(this->socketFD);
    exit(EXIT_FAILURE);
}

void Server::catchClient()
{
    int tmpFD;
    
    struct sockaddr_in client_addr;
    socklen_t sizeC = sizeof(client_addr);
    if ((tmpFD = accept(this->socketFD, (struct sockaddr *)&this->serverAddr, &Client::emptySizeSocks)) < 0)
    {    
        if (errno != EWOULDBLOCK)
            this->ExitFailure("error when accepting connection");
    }
    else
    { 			
		//add new socket to array of sockets 
		Client * newClient = new Client;
		newClient->socketFD = tmpFD;
		newClient->clientAddr = client_addr;
		this->connectedClient.insert(std::make_pair(tmpFD, newClient));
    }
}

void Server::disconnectClient(int fd)
{  
	
	if (this->connectedClient[fd]->isLog)
		std::cout << "Host disconnected , socket fd is "<< fd << ", ip is : " << inet_ntoa(this->connectedClient[fd]->clientAddr.sin_addr) << " , port : " << ntohs(this->serverAddr.sin_port)<< std::endl;
	delete this->connectedClient[fd];
	this->connectedClient.erase(fd);
	close(fd);
}

void Server::runningServer(void)
{
    int new_socket , activity, valread , sd, max_sd;  
	char buffer[1024];
         
    //set of socket descriptors 
    fd_set readfds; 

	while (this->isRunning) 
	{
		//clear the socket set 
        FD_ZERO(&readfds);  
     
        //add master socket to set 
        FD_SET(this->socketFD, &readfds);  
        max_sd = this->socketFD;  
             
        //add child sockets to set 
        for (clientIt it = this->connectedClient.begin(); it != this->connectedClient.end(); it++)  
        {  
            //socket descriptor 
            sd = it->first;  
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET(sd, &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);  
       
        if ((activity < 0) && (errno != EINTR))  
            printf("select error");
             
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(this->socketFD, &readfds))  
            this->catchClient();
		
        //else its some IO operation on some other socket
        for (clientIt it = this->connectedClient.begin(); it != this->connectedClient.end(); it++)  
        {  
            sd = it->first;  
                 
            if (FD_ISSET(sd, &readfds))  
            {  
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read(sd , buffer, 1024)) > 0)  
                {  
					buffer[valread] = '\0';
					if (receveMessage(it->first, buffer) == 1)
						break;
                    //Close the socket and mark as 0 in list for reuse 
                }  
                     
                //Echo back the message that came in 
                else 
                {  
                    //set the string terminating NULL byte on the end 
                    //of the data read 
                    buffer[valread] = '\0';  
                    send(sd , buffer , strlen(buffer) , 0 );  
                }  
				valread = 0;
				memset(buffer, 0, 1024);
            }  
        }  
	}
}

void Server::sendMessage(int fd, std::string msg)
{
	std::string nl = msg + "\n";
	std::cout << nl;
	if (send(fd, nl.c_str(), nl.length(), 0) != nl.length())
		perror("send"); 	
}

int Server::receveMessage(int fd, char * buffer)
{
	std::string message = counter_nl(buffer);
	if (!this->connectedClient[fd]->isLog)
	{
		std::string cmd = retcommande(message);
		if (cmd == "USER" || cmd == "NICK" || cmd == "PASS")
			switchcommande(message, this->connectedClient[fd]);
		return (0);
	}

	switchcommande(message, this->connectedClient[fd]);
	//std::cout << this->connectedClient[fd] << " : " << buffer << std::endl;
	return (1);
}

std::string Server::comp[] =
{
        "NICK",
        "USER",
        "JOIN",
        "QUIT",
		"PASS",
        "PING",
        "PART",
		"PRIVMSG",
        "MODE",
        "PONG",
		"KICK"
};

void    Server::switchcommande(std::string message, Client *User)
{
	std::cout << "COMMAND  : " << message << std::endl;
    std::string commande;
    std::string arg;
    std::size_t found = message.find(' ');
    bool    check;

    check = false;
    commande = retcommande(message);
    arg = retcommandearg(message);

    void	(Server::*fonction[])(std::string args, Client *User) = {
            &Server::nick,
			&Server::user,
			&Server::join,
			&Server::quit,
			&Server::pass,
            &Server::ping,
            &Server::part,
			&Server::privmsg,
            &Server::mode,
			&Server::pong,
			&Server::kick
	};
    for(int i = 0; i < 11; i++)
    {
        void (Server::*commands)(std::string args, Client *User) = fonction[i];
        if (commande == this->comp[i])
        {
            (this->*commands)(arg, User);
            check = true;
        }
    }
    if (!check)
        std::cout << "421 " << User->getnickname() << " " << commande << " :Unknown command" << std::endl;
}

std::string Server::retcommande(std::string message)
{
    std::string commande;
    std::size_t found = message.find(' ');
    // std::cout << "[" << message.find(' ') << "]\n";
    if (found != std::string::npos)
        commande = message.substr(0, found);            //Only return the command whitout the arg
    else
        commande = message;
    // std::cout << "[" << commande << "]\n";
    return (commande);
}

std::string Server::retcommandearg(std::string message)
{
    std::string commande;
    std::size_t found = message.find(' ');
    // std::cout << "[" << message.find(' ') << "]\n";
    if (found != std::string::npos)
        commande = message.substr(found + 1, message.size());  //Only return argument after the commande
    else
        commande = message;
    // std::cout << "[" << commande << "]\n";
    return (commande);
}

void Server::clientLog(int fd)
{
    Client * tmp = this->connectedClient[fd];
	tmp->isLog = true;
	std::cout << "New connection , socket fd is "<< fd << ", ip is : " << inet_ntoa(tmp->clientAddr.sin_addr) << " , port : " << ntohs(this->serverAddr.sin_port) << std::endl; 
	//send new connection greeting message 
	this->sendMessage(fd,  "001 " + std::string(tmp->getnickname() + " :Welcome to the irc Network, " + tmp->getnickname()));
	this->sendMessage(tmp->socketFD, "002 " + tmp->getnickname() + " :Your host is ft_irc, running version -1");
    this->sendMessage(tmp->socketFD, "003 " + tmp->getnickname() + " :This server was created: way too long ago");
    this->sendMessage(tmp->socketFD, "004 " + tmp->getnickname() + " " + inet_ntoa(tmp->clientAddr.sin_addr) + " ft_irc -1 io ovimpst");
    this->sendMessage(fd, "221 " + tmp->getnickname() + " +");
	std::cout << "Welcome message sent successfully\n";
}

void Server::createChannel(std::string name, Client * owner)
{
    channel * newChannel = new channel(name, owner);
    this->channelup[name] = newChannel;
    this->sendMessage(owner->socketFD, ":" + owner->getnickname() + "!" + owner->getnickname() + "@" + inet_ntoa(owner->clientAddr.sin_addr) + " JOIN " + name);	
	this->sendMessage(owner->socketFD, "324 " + owner->getnickname() + " " + name + " +tn");
    this->sendMessage(owner->socketFD, "353 " + owner->getnickname() + " " + name + " :" + this->channelup[name]->getClientNames());
    this->sendMessage(owner->socketFD, "366 " + owner->getnickname() + " " + name + " :End of NAMES list");
}

void Server::joinChannel(std::string name, Client * user)
{
    this->channelup[name]->newuser(user);
	for (std::map<int, Client *>::iterator it = this->channelup[name]->_connectedClient.begin(); it != this->channelup[name]->_connectedClient.end(); it++)
	{
		this->sendMessage(it->second->socketFD, ":" + user->getnickname() + "!" + user->getnickname() + "@" + inet_ntoa(user->clientAddr.sin_addr) + " JOIN " + name);	
		this->sendMessage(it->second->socketFD, "324 " + user->getnickname() + " " + name + " +tn");
		this->sendMessage(it->second->socketFD, "353 " + user->getnickname() + " " + name + " :" + this->channelup[name]->getClientNames());
		this->sendMessage(it->second->socketFD, "366 " + user->getnickname() + " " + name + " :End of NAMES list");
	}
}

void Server::leaveChannel(std::string name, Client * user)
{
	name.erase(remove_if(name.begin(), name.end(), isspace));
	for (std::map<int, Client *>::iterator it = this->channelup[name]->_connectedClient.begin(); it != this->channelup[name]->_connectedClient.end(); it++)
	{
		this->sendMessage(it->first, ":" + user->getnickname() + "!" + user->getnickname() + "@" + inet_ntoa(user->clientAddr.sin_addr) + " PART " + name + " :" + user->getnickname());	
	}
	this->channelup[name]->part(user);
    // if (this->channelup[name]->_connectedClient.size() == 0)
    //     this->channelup.erase(name);
}

void	Server::sendMessageChannel(std::string message, std::string channel, Client * user)
{
	for (std::map<int, Client *>::iterator it = this->channelup[channel]->_connectedClient.begin(); it != this->channelup[channel]->_connectedClient.end(); it++)
	{
		if (it->first == user->socketFD)
			continue;
		this->sendMessage(it->first, message);
		this->sendMessage(it->first, ":" + user->getnickname() + "!" + user->getnickname() + "@" + inet_ntoa(user->clientAddr.sin_addr) + " PRIVMSG " + channel + " :" + message);
	}
}