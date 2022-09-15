#ifndef CLIENT_CLASS_HPP
#define CLIENT_CLASS_HPP

#include "../includes.hpp"

class Client
{   
    private:
        std::string                 _username;
        std::string                 _nickname;

    public:
        struct sockaddr_in 	       	clientAddr;
        int                             socketFD;
        static socklen_t                emptySizeSocks;
	bool			        isLog;

        void    setnickname(std::string name)   {this->_nickname = name;}
        void    setusername(std::string name)   {this->_username = name;}

        std::string    getnickname(void)   {return(this->_nickname);}

        Client();
        ~Client();
};

#endif