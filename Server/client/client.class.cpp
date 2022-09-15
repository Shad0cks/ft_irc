#include "../includes.hpp"

socklen_t Client::emptySizeSocks = sizeof(struct sockaddr);

Client::Client(void)
{
    this->isLog = false;
	this->setUser = false;
	this->setNick = false;
	this->setPass = false;
}
Client::~Client(void)
{
    
}