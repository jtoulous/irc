#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel(String n, String pwd, int adm): name(n), password(pwd), topic(""), owner(adm) {
    users.push_back(adm);
}

Channel::~Channel() {}
/* rajouter comande MODE avec les mode autorisés
    Et le faire pour les admin et user */

/*String    Channel::PrintCommandCanalForOwner() {

    String str = "All commands avalaible: \n"
                "KICK: Eject a client from the channel"
                "PASS: Is used to set a 'connection password.\n"
                "NICK: Is used to give user a nickname or change the previous one.\n"
                "USER: Is used to specify a user's details when connecting to an IRC server.\n"
                "JOIN:  is used to join or create a specific channel (chat room) on the IRC server.\n"
                "PRIVMSG: is used to send a private message to a specific user or a message to a channel.\n"
                "INVITE:  is used to invite a specific user to join a channel.\n" 
                "MODE: Change the channel's mode: \n"
                "-> i: Set/remove Invite-only channel\n"
                "-> t: Set/remove The restrictions of the TOPIC command to channel operator\n"
                "-> k: Set/remove The channel password\n"
                "-> o: Give/take Chanel operator privilege\n";
    return (str);
}*/

String      Channel::getName() {
    return (this->name);
}

String      Channel::getPassword() {
    return (this->password);
}

String      Channel::getTopic() {   
    return (this->topic);
}

bool        Channel::getInviteOnly() {
    return (this->invite_only);
}

void    Channel::setUserFd(int user_fd) {

    this->users.push_back(user_fd);
}

void    Channel::setAdminFd(int fd)
{
    if (!FdIsAdmin(fd))
        admin.push_back(fd);
}

vector<int> &Channel::getUsers()
{
    return (users);
}

vector<int> &Channel::getAdmins()
{
    return (admin);
}

int         &Channel::getOwner()
{
    return (owner);
}

int         Channel::getAdminWithIndex(int index_vec) {
    return (this->admin[index_vec]);
}

bool        Channel::FdIsAdmin(int fd) {

    if (this->admin.size() == 0)
        return (false);
    for (size_t i = 0; i < this->admin.size(); i++) {
        if (this->admin[i] == fd)
            return (true);
    }
    return (false);
}

bool    Channel::FdIsUser(int fd)
{
    for (int i = 0; i < (int)users.size(); i++)
        if (fd == users[i])
            return (true);
    return (false);
}

bool    Channel::FdIsOwner(int fd)
{
    if (fd == owner)
        return (true);
    return (false);
}

void    Channel::diffuseMsg(String msg, vector<Client *> clientList, int srcFd)
{
    for (int i = 0; i < (int)users.size(); i++)
        if (srcFd != users[i])
            sendMsg(msg, users[i], Utils::findClientNick(users[i], clientList));
}

void    Channel::setBoolInviteOnly(bool invite) {
    this->invite_only = invite;
}

void        Channel::setTopic(String topik) {
    this->topic = topik;
}

void    Channel::removeAdmin(int fd)
{
    for (int i = 0; i < (int)admin.size(); i++)
    {
        if (admin[i] == fd)
        {
            admin.erase(admin.begin() + i);
            return;
        }
    }
}