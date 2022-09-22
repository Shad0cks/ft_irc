#ifndef CLIENT_CLASS_HPP
#define CLIENT_CLASS_HPP

#include "../includes.hpp"

class Client
{   
    private:
        std::string                 	_username;
        std::string                 	_nickname;
        bool                            _operator;

    public:
        struct sockaddr_in 	       	    clientAddr;
        int                             socketFD;
        static socklen_t                emptySizeSocks;
		bool			        		isLog;
		bool			        		setNick;
		bool			        		setUser;
		bool							setPass;
		std::string						cmdBuffer;

        void    setnickname(std::string name)   {this->_nickname = name;}
        void    setusername(std::string name)   {this->_username = name;}
        void    setoperator(bool m)             {this->_operator = m;}

        std::string    getnickname(void)   {return(this->_nickname);}
        bool           getoperator(void)   {return(this->_operator);}

        Client();
        ~Client();
};

#endif