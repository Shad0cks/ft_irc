#include "../includes.hpp"

socklen_t Client::emptySizeSocks = sizeof(struct sockaddr);

Client::Client(void)
{
    this->isLog = false;
}
Client::~Client(void)
{
    
}