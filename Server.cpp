#include "Server.hpp"
#include "Define.hpp"

/////////////////////////////////////////////////
//                 CONSTRUCTORS                //
/////////////////////////////////////////////////

Server::Server()
{}

Server::Server(char **argv)
: Hostname("Server")
{
    sockaddr_in serverAddr;
    int reuse = 1;

    EntrySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (EntrySocket == -1)
        throw (Xception("Error: entry socket not created"));
    
    setsockopt(EntrySocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    if (bind(EntrySocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        throw (Xception("Error: binding entry socket"));

    password = argv[2];
    
    std::cout << "\n================================\n"
              <<   "|       SERVER ACTIVE          |\n" 
              <<   "================================\n" 
              << std::endl;
}

Server::~Server()
{
    close (EntrySocket);
}


////////////////////////////////////////////////
///             GETTERS                       //
////////////////////////////////////////////////

int     Server::getFdMax()
{
    int Max = EntrySocket;

    for (int i = 0; i < (int)clientList.size(); i++)
        if (clientList[i]->getFd() > Max)
            Max = clientList[i]->getFd();
    return (Max);
}

int     Server::getEntrySocket()
{
    return (EntrySocket);
}

//////////////////////////////////////////////////////
//                  METHODS                         //
//////////////////////////////////////////////////////

void    Server::servCheckSockets(fd_set &sockets)
{
    if (FD_ISSET(EntrySocket, &sockets))
        servNewConnection();
    for (unsigned int i = 0; i < clientList.size(); i++)
        if (FD_ISSET(clientList[i]->getFd(), &sockets))
            servTreatClient(clientList[i]);
}

void    Server::servNewConnection()
{
    int             newClient;
    sockaddr_in     clientAddr;
    socklen_t       clientSize = sizeof(clientAddr);

    if (clientList.size() == 0)
        listen(EntrySocket, 10);
    newClient = accept(EntrySocket, (struct sockaddr *)&clientAddr, &clientSize);
    if (newClient == -1)
        std::cout << RED << "Error: accept()" << DEFAULT << std::endl;
    else
    {
        clientList.push_back(new Client);
        clientList[clientList.size() - 1]->setFd(newClient);
        clientList[clientList.size() - 1]->setNb(clientList.size() - 1);
    }
}

void    Server::servTreatClient(Client *client)
{
    String  entry;
    String  cmd;

    servReceive(client);
    
    while (client->buffer.find('\n') != NPOS)
    {
        entry = client->buffer.substr(0, client->buffer.find('\n'));
        client->buffer.erase(0, client->buffer.find('\n') + 1);
        
        if (entry.find(' ') == NPOS)//a remplacer par un ptite gestion d'erreur pour la commande recu
            sendMsg("bad input\n", client->getFd(), client->getNickname());//remplacer par le RPL qu il faut
        else
        {
            cmd = entry.getWord(1);

            if (cmd == "PASS" || cmd == "pass")
                pass(client, entry);
            else if (cmd == "NICK" || cmd == "nick")
                nick(client, entry);
            else if (cmd == "USER" || cmd == "user")
                user(client, entry);
            else if (cmd == "JOIN" || cmd == "join")
                join(client, entry);
            //else if (cmd == "INVITE")
              //  invite(client, cmd, entry);
            else if (cmd == "PRIVMSG" || cmd == "privmsg")
                privMsg(client, entry);
            
            //else if (CMD == "INVITE")  
        } 
    }
}

void    Server::servReceive(Client *client)
{
    int     size = BUFFER_SIZE;
    char    buff[BUFFER_SIZE];

    while (size == BUFFER_SIZE)
    {
        size = recv(client->getFd(), buff, 100, 0);   
        if (size == -1)
        {
            std::cout << "recv problem for " << client->getUsername() << std::endl;
            return;
        }
        buff[size] = '\0';
        client->buffer = client->buffer + buff;
    }
    client->buffer -= "\r";
    std::cout << GREEN << "\n" << client->buffer << DEFAULT << std::endl;
}


/////////////////////////////////////////////////////
//              IRC COMMANDS                       //
////////////////////////////////////////////////////


void    Server::pass(Client *client, String &entry)
{
    String  entryPwd = entry.getWord(2);

    if (entryPwd == password)
    {
        client->setLoggedIn(1);
        sendMsg(RPL_WELCOME(client->getNickname()), client->getFd(), client->getNickname());
    }
    //else //il degage
    //{
    //    close (client->getFd());
    //    Utils::removeClient(client->getFd(), clientList);
    //}
}

void    Server::nick(Client *client, String &entry)
{
    String  nickname = entry.getWord(2);

    if (!Utils::nicknameAvailable(nickname, clientList))
        sendMsg(ERR_NICKALREADYUSED(nickname), client->getFd(), client->getNickname());
    else
    {
        client->setNickname(nickname);
        sendMsg(RPL_NICKCHANGE(nickname), client->getFd(), client->getNickname());
    }
}

void    Server::user(Client *client, String &entry)
{
    String  username = entry.getWord(2);

    if (!Utils::usernameAvailable(username, clientList))
        sendMsg(ERR_NICKALREADYUSED(username), client->getFd(), client->getNickname());
    else
    {
        client->setUsername(username);
        sendMsg("Username changed successfully\r\n", client->getFd(), client->getNickname());
    }
}

//void    Server::invite()
//{}

//////////////////////////////////////////////////////////////////////////

void    sendMsg(String msg, int fd, String nick)
{
    send(fd, msg.c_str(), msg.size(), 0);
    msg -= "\r\n";
    if (!nick.empty())
        std::cout << "=====> " << ORANGE << "to " << nick << ": " << DEFAULT << msg << std::endl;
}