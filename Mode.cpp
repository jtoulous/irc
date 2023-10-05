#include "Server.hpp"                                                      

static void execMode(String mode, Channel *channel, Client *Target, Client *Admin)
{                                                                            
  if (mode == '+i' || mode == '-i')                                          
    iMod();///////////////////////////////////////////////////              
  else if (mode == '+t' || mode == '-t')                               
    tMod();///////////////////////////////////////////////////             
  else if (mode == '+k' || mode == '-k')                                 
    kMod();///////////////////////////////////////////////////          
  else if (mode == '+o' || mode == '-o')                               
    oMod();//////////////////////////////////////////////////          
  else if (mode == '+l' || mode == '-l')                                
      lMod();//////////////////////////////////////////////////
  else
    sendMsg(ERR_NOTVALIDMOD(), Admin->getFd(), Admin->getNickname());
}

static void  parseModes(vector<String> &modes, String entry)
{
  char  operator = '\0';
  
  for (int i = 1; i <= entry.wordCount(); i++)
  {
    String word = entry.getWord(i);

    for (int j = 0; word)
    if (operator == '\0' && word[0] != '+' || word[0] != '-')
    {

    }
  }
}

void  Server::mode(Client *client, String &entry)
{
  vector<String>  modes; 
  int             channelIndex = Utils::findChannelIndex(entry.extractWord(2), channelList);
  int             fdTarget = -2;

  entry.rmWord(1);

  try
  {
    if (channelIndex == -1)//channel existe pas
      throw (Xception(ERR_BLABLA));
    
    if (!channelList[channelIndex]->FdIsAdmin(client->getFd()))//pas admin
      throw (Xception(ERR_BLABLA));

    if (entry.wordStartChar(1) != '+' && entry.wordStartChar(1) != '-')
    { 
      fdTarget = Utils::findClientFd(entry.extractWord(1), clientList);
      if (fdTarget == -1)//le client cible existe pas
        throw (Xception(ERR_BLABLA));
    }

    parseModes(modes, entry)//mode invalid

    for (int i = 0; i < (int)modes.size(); i++)//execution 1 par 1
      execMode(modes[i], channelList[channelIndex], clientList[Utils::findClientIndex(fdTarget)], client);
  }

  catch (Xception &e)
  {
    String err = e.what();
    sendMsg(err, client->getFd(), client->getNickname());
  } 
}