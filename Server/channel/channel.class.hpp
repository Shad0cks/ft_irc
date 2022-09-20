#ifndef CHANNEL_CLASS_HPP
#define CHANNEL_CLASS_HPP

#include "../client/client.class.hpp"
#include "../includes.hpp"
class Client;

class channel
{
    private:
        const std::string                               _channelname;
        bool                                            _cankick;
        bool                                            _cansendmsghc;
        bool                                            _limituser;
        bool                                            _needpassword;
        unsigned int                                    _limite;
        std::string                                     _password;
        int                                             _nbconnected;
        std::map<int, Client *>                         _connectedClientp;
    public:
        typedef  std::map<int, Client *>::iterator    channelIt;
        channel(std::string channelname, Client *firstclient);
        std::map<int, Client *>                         _connectedClient;
        virtual ~channel(void);

        std::string const getchannelname(void)      {return(this->_channelname);}
        bool        getcankick(void)                {return(this->_cankick);}
        bool        getcansendmsghc(void)           {return(this->_cansendmsghc);} 
        bool        getlimituser(void)              {return(this->_limituser);}
        bool        getneedpassword(void)           {return(this->_needpassword);}
        unsigned int getlimite(void)                {return(this->_limite);}
        std::string getpassword(void)               {return(this->_password);}
        int         getnbuser(void)                 {return(this->_nbconnected);}
        void        setpassword(std::string m)      {this->_password = m;}
        void        setneedpassword(bool m)         {this->_needpassword = m;}
        void        setlimite(unsigned int m)       {this->_limite = m;}
        void        setcansendmsghc(bool m)         {this->_cansendmsghc = m;}
        void        setcankick(bool m)              {this->_cankick = m;}
        void        setlimituser(bool m)            {this->_limituser = m;}
		std::string getClientNames(void);
        void        newuser(Client *user);
        void        part(Client *user);
        bool isInChannel(Client * user);

};

#endif