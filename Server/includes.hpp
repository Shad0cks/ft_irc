#ifndef INCLUDES_HPP
#define INCLUDES_HPP

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fcntl.h>
#include <poll.h>

#include "client/client.class.hpp"
#include "server/server.class.hpp"

int strfcr(std::string src, char c);

#endif