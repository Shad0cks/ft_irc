#include "../includes.hpp"

socklen_t Client::emptySizeSocks = sizeof(struct sockaddr);

Client::Client(void)
{
    
}
Client::~Client(void)
{
    close(this->socketFD);
}