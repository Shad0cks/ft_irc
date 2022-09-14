#ifndef CLIENT_CLASS_HPP
#define CLIENT_CLASS_HPP

#include "../includes.hpp"

class Client
{   
    public:
        struct sockaddr_in 	       	clientAddr;
        int                         socketFD;
        static socklen_t            emptySizeSocks; 

        Client();
        ~Client();
};

#endif