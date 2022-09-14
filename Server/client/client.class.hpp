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
        int                         socketFD;
        static socklen_t            emptySizeSocks;

        void    setnickname(std::string name)   {this->_nickname = name;}
        void    setusername(std::string name)   {this->_username = name;}

        std::string    getnickname(std::string name)   {return(this->_nickname);}
        std::string    getusername(std::string name)   {return(this->_username);}

        Client();
        ~Client();
};

#endif