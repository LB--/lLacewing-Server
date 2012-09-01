#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../ConfigPanel.hpp"
#include "../Logger.hpp"
#include "../StringConvert.hpp"

#include "Lacewing.h"

Logger *cLog, *cErr;
std::string Init(Logger &Log, Logger &Err)
{
	cLog = &Log;
	cErr = &Err;

	static const std::string Name = "Defualt Component";
	return(Name);
}
#define Log (*cLog)
#define Err (*cErr)

unsigned long version()
{
	return(0);
}

void Name(Lacewing::RelayServer::Client &Client)
{
	if(*Client.Name())
	{
		Log << " (" << Client.Name() << ")";
	}
}

bool OnConnect(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client)
{
	Log << "Client " << Client.ID() << " connected from " << Client.GetAddress().ToString() << std::endl;
	return(true);
}
void OnDisconnect(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client)
{
	Log << "Client " << Client.ID();
	Name(Client);
	Log << " disconnected" << std::endl;
}

void OnServerMessage(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, bool Blasted, int Subchannel, char * Data, int Size, int Variant)
{
	Log << "Client " << Client.ID();
	Name(Client);
	if(Blasted)
	{
		Log << " blasted ";
	}
	else
	{
		Log << " sent ";
	}
	Log << Size << " bytes on subchannel " << Subchannel << std::endl;
}
bool OnChannelMessage(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, Lacewing::RelayServer::Channel &Channel, bool Blasted, int Subchannel, char * Data, int Size, int Variant)
{
	return(true);
}
bool OnPeerMessage(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, Lacewing::RelayServer::Channel &Channel, Lacewing::RelayServer::Client &TargetClient, bool Blasted, int Subchannel, char * Packet, int Size, int Variant)
{
	return(true);
}

bool OnJoinChannel(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, Lacewing::RelayServer::Channel &Channel)
{
	return(true);
}
bool OnLeaveChannel(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, Lacewing::RelayServer::Channel &Channel)
{
	return(true);
}

bool OnSetName(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, const char * Name)
{
	Log << "Client " << Client.ID();
	::Name(Client);
	Log << " changed their name to \"" << Name << "\"" << std::endl;
	return(true);
}

std::vector<ConfigPanel::Element*> *Config;

std::vector<ConfigPanel::Element*> WebserverConfigPanel(Lacewing::RelayServer &Server)
{
	if(Config)
	{
		while(!Config->empty())
		{
			delete Config->back();
			Config->pop_back();
		}
		delete Config;
	}
	Config = new std::vector<ConfigPanel::Element*>;
	ConfigPanel::Select *users = new ConfigPanel::Select;
	users->Name = "Users";
	users->ID = "Userlist";
	users->Multiple = true;
	users->OptGroups.push_back(ConfigPanel::Select::OptGroup());
	users->OptGroups.back().ID = "Users";
	users->OptGroups.back().Label = "Users";
	Lacewing::RelayServer::Client *client = Server.FirstClient();
	while(client)
	{
		users->OptGroups.back().Options.push_back(ConfigPanel::Select::OptGroup::Option());
		users->OptGroups.back().Options.back().ID = ConvTo<std::string>::f(client->ID());
		users->OptGroups.back().Options.back().Label = client->Name();
		users->OptGroups.back().Options.back().Text = client->Name();
		users->OptGroups.back().Options.back().Value = ConvTo<std::string>::f(client->ID());
		client = client->Next();
	}
	Config->push_back(users);
	ConfigPanel::ISubmit *submit = new ConfigPanel::ISubmit;
	submit->ID = "Submit";
	submit->Name = "Submit";
	submit->Value = "Submit!";
	Config->push_back(submit);
	return(*Config);
}
void ConfigPanelData(Lacewing::RelayServer &Server, std::map<std::string, std::string> &PostData)
{
	for(std::map<std::string, std::string>::iterator it = PostData.begin(); it != PostData.end(); ++it)
	{
		Log << it->first << " = " << it->second << std::endl;
	}
}

void ReleaseData()
{
	if(Config)
	{
		while(!Config->empty())
		{
			delete Config->back();
			Config->pop_back();
		}
		delete Config;
	}
}

