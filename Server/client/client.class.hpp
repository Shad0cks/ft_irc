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
<<<<<<< HEAD
        static socklen_t            emptySizeSocks;

        void    setnickname(std::string name)   {this->_nickname = name;}
        void    setusername(std::string name)   {this->_username = name;}

        std::string    getnickname(std::string name)   {return(this->_nickname);}
        std::string    getusername(std::string name)   {return(this->_username);}
=======
        static socklen_t            emptySizeSocks; 
		std::string					hostname;
		std::string					nickname;
		bool						isLog;
>>>>>>> 562d998f62ed61be730f165d7b228b5e98763557

        Client();
        ~Client();
};

#endif