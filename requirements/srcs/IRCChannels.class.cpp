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

IRCChannel::IRCChannel(std::string channelName, std::string channelPass){
    this->_channel_name = channelName;
    this->_channel_pass = channelPass;
    this->_client_limit = 50;
    this->_modes = 0;
    this->_topic = "";
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

IRCChannel::IRCChannel(IRCChannel const &src){
    _joinedUsers = src._joinedUsers;
    _admins = src._admins;
    _channel_name = src._channel_name;
    _channel_pass = src._channel_pass;
    _topic = src._topic;
    _client_limit = src._client_limit;
    _modes = src._modes;
} 

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

const IRCChannel &IRCChannel::operator=(IRCChannel const &src){
    if (this != &src) {
        _joinedUsers = src._joinedUsers;
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

IRCChannel::~IRCChannel() {}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

bool   IRCChannel::isPrivate() const {
    return (this->_modes & 0x01);
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

bool   IRCChannel::isSecret() const {
    return (this->_modes & 0x02);
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

bool   IRCChannel::isInviteOnly() const {
    return (this->_modes & 0x04);
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

bool  IRCChannel::isOnlyVoiceAndOps() const {
    return (this->_modes & 0x08);
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

bool  IRCChannel::isNoOutsideMessages() const {
    return (this->_modes & 0x10);
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

bool  IRCChannel::isOnlyOpsChangeTopic() const {
    return (this->_modes & 0x20);
}

/* -------------------------------------------------------------------------- */

void IRCChannel::setChannelName(const std::string &channelName){
    this->_channel_name = channelName;;
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

std::string IRCChannel::getChannelName() const{
    return this->_channel_name;
}


uint64_t  IRCChannel::getClientLimit() const {
    return (this->_client_limit);
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

void  IRCChannel::setClientLimit(uint64_t _client_limit) {
    this->_client_limit = _client_limit;
}


std::string IRCChannel::getChannelPass() const{
    return this->_channel_pass;
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

void        IRCChannel::setClientPass(std::string channel_pass){
    this->_channel_pass = channel_pass;
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

std::string IRCChannel::getChannelTopic() const{
    return this->_topic;
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

void IRCChannel::notifyUsers(int fd){

    std::string notifyusers = ":" + this->_joinedUsers.find(fd)->second.getNICKNAME() \
    + "!" + this->_joinedUsers.find(fd)->second.getUSERNAME() + "@localhost.ip JOIN " \
    + this->getChannelName() + "\n";
    // :Guest75045!~hh@5c8c-aff4-7127-3c3-1c20.230.197.ip JOIN :#general
    for(std::map<int, Client>::iterator i = this->_joinedUsers.begin(); i != this->_joinedUsers.end();i++){
        writeMessageToClient(i->first, notifyusers);
    }
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

void IRCChannel::welcomeUser(int fd){

        std::string nickName = this->_joinedUsers.find(fd)->second.getNICKNAME();
        std::string membersList;
        for(std::map<int, Client>::iterator i = this->_joinedUsers.begin(); i != this->_joinedUsers.end();i++){
            if (this->_admins.find(i->first) != this->_admins.end()){
                membersList += "@" + i->second.getNICKNAME() + " ";
            }else
                membersList += i->second.getNICKNAME() + " ";
        }
        RPL_NAMREPLY(fd, nickName,  this->getChannelName(), membersList);
        RPL_ENDOFNAMES(fd, nickName,  this->getChannelName());

}


/* -------------------------------------------------------------------------- */\

void    IRCChannel::joinChannel(Client &client, std::string &chPass, int fd){
    if (this->getChannelPass() != chPass){
        ERR_BADCHANNELKEY(fd, this->getChannelName());
    } else if (this->isInviteOnly()){
        ERR_INVITEONLYCHAN(fd, this->getChannelName());
    }else if (this->_joinedUsers.size() >= this->getClientLimit()){
        ERR_CHANNELISFULL(fd, this->getChannelName());
    } else if (this->_joinedUsers.find(fd) == this->_joinedUsers.end()){
        this->_joinedUsers.insert(std::make_pair(fd, client));
        this->notifyUsers(fd);
        this->welcomeUser(fd);
    }
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */
void    IRCChannel::addAdmin(int fd){
    this->_admins.insert(std::make_pair(fd, this->_joinedUsers.find(fd)->second));
}

/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */
void    IRCChannel::messagToChannel(int fd, std::string &msg){
    std::map<int, Client>::iterator client = this->_joinedUsers.find(fd);
    if (client != this->_joinedUsers.end() && this->_bannedUsers.find(fd) == this->_bannedUsers.end()){
        if (!this->isOnlyVoiceAndOps() || (this->isOnlyVoiceAndOps() && this->_admins.find(fd) != this->_admins.end())){
            std::string fullMsg = ":" + client->second.getNICKNAME() \
            + "!" + client->second.getUSERNAME() + "@localhost.ip PRIVMSG " \
            + this->getChannelName() + " :" + msg + "\n";

            for(std::map<int, Client>::iterator i = this->_joinedUsers.begin(); i != this->_joinedUsers.end();i++){
                if (i->first != fd)
                    writeMessageToClient(i->first, fullMsg);
            }
        } else
            ERR_CANNOTSENDTOCHAN(fd, this->getChannelName());
    } else
        ERR_CANNOTSENDTOCHAN(fd, this->getChannelName());
}


/* -------------------------------------------------------------------------- */