/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.class.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haitkadi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/07 19:01:05 by haitkadi          #+#    #+#             */
/*   Updated: 2023/05/07 19:01:10 by haitkadi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ircserv.head.hpp"

/* -------------------------------------------------------------------------- */

Channel::Channel(std::string channelName, std::string channelPass){
    this->_channel_name = channelName;
    this->_channel_pass = channelPass;
    this->_client_limit = 50;
    this->_modes = 0;
    this->_topic = "";
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

Channel::Channel(Channel const &src){
    _joinedClients = src._joinedClients;
    _admins = src._admins;
    _channel_name = src._channel_name;
    _channel_pass = src._channel_pass;
    _topic = src._topic;
    _client_limit = src._client_limit;
    _modes = src._modes;
} 

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

const Channel &Channel::operator=(Channel const &src){
    if (this != &src) {
        _joinedClients = src._joinedClients;
        _admins = src._admins;
        _channel_name = src._channel_name;
        _channel_pass = src._channel_pass;
        _topic = src._topic;
        _client_limit = src._client_limit;
        _modes = src._modes;
    }
    return *this;
}


/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

Channel::~Channel() {}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

bool   Channel::isPrivate() const {
    return (this->_modes & 0x01);
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

bool   Channel::isSecret() const {
    return (this->_modes & 0x02);
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

bool   Channel::isInviteOnly() const {
    return (this->_modes & 0x04);
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

bool  Channel::isOnlyVoiceAndOps() const {
    return (this->_modes & 0x08);
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

bool  Channel::isNoOutsideMessages() const {
    return (this->_modes & 0x10);
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

bool  Channel::isOnlyOpsChangeTopic() const {
    return (this->_modes & 0x20);
}

/* -------------------------------------------------------------------------- */

void Channel::setChannelName(const std::string &channelName){
    this->_channel_name = channelName;;
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

std::string Channel::getChannelName() const{
    return this->_channel_name;
}


uint64_t  Channel::getClientLimit() const {
    return (this->_client_limit);
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

void  Channel::setClientLimit(uint64_t _client_limit) {
    this->_client_limit = _client_limit;
}


std::string Channel::getChannelPass() const{
    return this->_channel_pass;
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

void        Channel::setClientPass(std::string channel_pass){
    this->_channel_pass = channel_pass;
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

std::string Channel::getChannelTopic() const{
    return this->_topic;
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

void Channel::notifyUsers(int fd){

    std::string notifyusers = ":" + this->_joinedClients.find(fd)->second.getNICKNAME() \
    + "!" + this->_joinedClients.find(fd)->second.getUSERNAME() + "@localhost JOIN " \
    + this->getChannelName() + "\n";
    for(std::map<int, Client>::iterator i = this->_joinedClients.begin(); i != this->_joinedClients.end();i++){
        if (i->first != fd){
            writemessagetoclients(i->first, notifyusers);
        }
    }
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

void Channel::welcomeUser(int fd){

// :irc.server.com 353 john = #example :@opUser +voiceUser regularUser
    if (this->_joinedClients.size() > 1){
        std::string welcome = ":IrcTheThreeMusketeers 353 " + \
        this->_joinedClients.find(fd)->second.getNICKNAME() + " = " + this->getChannelName() + \
        ":";
        for(std::map<int, Client>::iterator i = this->_joinedClients.begin(); i != this->_joinedClients.end();i++){
            if (i->first != fd){
                welcome += i->second.getNICKNAME() + " ";
            }
        }
        welcome += "\n";
        writemessagetoclients(fd, welcome);
    }
}


/* -------------------------------------------------------------------------- */\

void    Channel::joinChannel(Client &client, std::string &chPass, int fd){
    if (this->getChannelPass() != chPass){
        ERR_BADCHANNELKEY(fd, this->getChannelName());
    } else if (this->isInviteOnly()){
        ERR_INVITEONLYCHAN(fd, this->getChannelName());
    }else if (this->_joinedClients.size() >= this->getClientLimit()){
        ERR_CHANNELISFULL(fd, this->getChannelName());
    } else if (this->_joinedClients.find(fd) == this->_joinedClients.end()){
        this->_joinedClients.insert(std::make_pair(fd, client));
        if (this->getChannelTopic().empty()){
            RPL_NOTOPIC(fd, this->getChannelName());
        } else
            RPL_TOPIC(fd, this->getChannelName(), this->getChannelTopic());
        notifyUsers(fd);
        welcomeUser(fd);
    }
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */
void    Channel::addAdmin(int fd){
    this->_admins.insert(std::make_pair(fd, this->_joinedClients.find(fd)->second));
}

/* -------------------------------------------------------------------------- */
