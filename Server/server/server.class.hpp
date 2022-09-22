#ifndef SERVER_CLASS_HPP
#define SERVER_CLASS_HPP

#include "../includes.hpp"
class Client;
class channel;
class Server
{
    private:


        typedef std::map<int, Client *>::iterator   clientIt;
        typedef std::map<std::string, channel *>::iterator    channelIt;
        std::map<int, Client *>                 connectedClient;
        std::map<std::string, channel *>        channelup;

        struct sockaddr_in          serverAddr;
        int                         socketFD;
       
        bool                        isRunning;
        std::vector<struct pollfd>  ufds;
        std::string                 password;
        int                         port;
        static                      std::string comp[];


        void                    ExitFailure(std::string message);
        void                    ExitFailure(std::string message, int closedSocket);
    
    public:


        Server(std::string port, std::string password);
        ~Server(void);
        
		void sendMessage(int fd, std::string msg);
        void catchClient(void); 
		void disconnectClient(int fd); 
        void runningServer(void);
        void switchcommande(std::string message, Client *User);
		int receveMessage(int fd, std::string buffer);
        std::string retcommande(std::string message);  // only return the commande in string
        std::string retcommandearg(std::string message);  // only return the arg after the command in string
		void clientLog(int fd);
        void createChannel(std::string name, Client * owner, std::string mod);
        void joinChannel(std::string name, Client * user, std::string mod);
        channel * isConnected(Client * user);
        void leaveChannel(std::string name, Client * user);
		void	sendMessageChannel(std::string message, std::string channel, Client * user);
		Client * 	getClientByName(std::string name);

		//commands
		void nick(std::string args, Client *User);
		void user(std::string args, Client *User);
		void join(std::string args, Client *User);
		void quit(std::string args, Client *User);
		void pass(std::string args, Client *User);
        void ping(std::string args, Client *User);
        void part(std::string args, Client *User);
		void privmsg(std::string args, Client *User);
        void mode(std::string args, Client *User);
		void pong(std::string args, Client *User);
        void names(std::string args, Client *User);
		void kick(std::string args, Client *User);
		
};

#endif