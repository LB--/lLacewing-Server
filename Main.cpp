#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <exception>
#include <Windows.h>

#include "Lacewing.h"
#include "INI.hpp"
#include "Logger.hpp"
#include "TagStructs.hpp"
#include "Component.hpp"
#include "StringConvert.hpp"

bool OnConnect(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client);
void OnDisconnect(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client);

void OnError(Lacewing::RelayServer &Server, Lacewing::Error &Error);
void OnError(Lacewing::FlashPolicy &FlashPolicy, Lacewing::Error &Error);

void OnServerMessage(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, bool Blasted, int Subchannel, char * Data, int Size, int Variant);
bool OnChannelMessage(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, Lacewing::RelayServer::Channel &Channel, bool Blasted, int Subchannel, char * Data, int Size, int Variant);
bool OnPeerMessage(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, Lacewing::RelayServer::Channel &Channel, Lacewing::RelayServer::Client &TargetClient, bool Blasted, int Subchannel, char * Packet, int Size, int Variant);

bool OnJoinChannel(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, Lacewing::RelayServer::Channel &Channel);
bool OnLeaveChannel(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, Lacewing::RelayServer::Channel &Channel);

bool OnSetName(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, const char * Name);


void OnGet(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request);
void OnPost(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request);
void OnHead(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request);

void OnDisconnect(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request);

void OnError(Lacewing::Webserver &Webserver, Lacewing::Error &Error);

void OnUploadStart(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request, Lacewing::Webserver::Upload &Upload);
void OnUploadChunk(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request, Lacewing::Webserver::Upload &Upload, const char * Data, int Size);
void OnUploadDone(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request, Lacewing::Webserver::Upload &Upload);
void OnUploadPost(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request, Lacewing::Webserver::Upload * Uploads[], int UploadCount);


Logger Log (Logger::Log, "log.log");
Logger Err (Logger::Error, "log.log");

INI Ini;	bool DefaultTo(INI &Ini, const std::string &group, const std::string &item, const std::string &value);
std::vector<Component> Components;

Lacewing::EventPump EventPump;
Lacewing::RelayServer RelayServer (EventPump);
Lacewing::FlashPolicy FlashPolicy (EventPump);
Lacewing::Webserver Webserver (EventPump);
bool Run = true;

int __stdcall EmergencyCleanup(unsigned long CtrlType);

int main()
{
	RelayServer.Tag = new RelayServerTag;
	FlashPolicy.Tag = new FlashPolicyTag;

	SetConsoleCtrlHandler(&EmergencyCleanup, TRUE); //For unexpected closes

	SetConsoleTitle(((std::string() += "lLacewing Server - ") += Lacewing::Version()).c_str());
	Log << "";
	Logger::SetLogColor(15);
	Log << "lLacewing Server by LB - ";
	Logger::SetLogColor(2);
	Log << Lacewing::Version() << std::endl;

	{
		std::ifstream ini ("Config.ini", std::ifstream::in|std::ifstream::binary);
		if(!ini.is_open())
		{
			Err << "Config.ini not found or does not have read access, using default settings." << std::endl;
			Ini("Relay Server", "Port") = "6121";
			Ini("Relay Server", "Channel Listing") = "true";
			Ini("Relay Server", "Welcome Message") = std::string("lLacewing Server - ") += Lacewing::Version();
			Ini("Flash Policy Server", "Enabled") = "true";
			Ini("Flash Policy Server", "Crossdomain XML File") = "crossdomain.xml";
			Ini("Flash Policy Server", "Port") = "843";
			Ini("Webserver", "Enabled") = "true";
			Ini("Webserver", "Port") = "80";
			Ini("Webserver", "Password") = "Restart the server after changing this!";
			Ini("Components", "Enabled") = "true";
			Ini("Components", "Ignore") = "example,list,of,DLLs,to,ignore,for,instance,the,Default,component,is,now,ignored";
	
			std::string Default (Ini);
			std::ofstream ini ("Config.ini", std::ofstream::out|std::ofstream::binary|std::ofstream::trunc);
			if(ini.is_open())
			{
				ini.write(Default.c_str(), Default.length());
				Log << "Saved default configuration file, you should edit it and restart" << std::endl;
			}
			else
			{
				Err << "Could not write default settings to Config.ini" << std::endl;
			}
		}
		else
		{
			std::string strini;
			int ch;
			while((ch = ini.get()) != EOF)
			{
				strini += char(ch);
			}
			Ini = strini;

			bool update = false;
			update |= DefaultTo(Ini, "Relay Server", "Port", "6121");
			update |= DefaultTo(Ini, "Relay Server", "Channel Listing", "true");
			update |= DefaultTo(Ini, "Relay Server", "Welcome Message", std::string("lLacewing Server - ") += Lacewing::Version());
			update |= DefaultTo(Ini, "Flash Policy Server", "Enabled", "true");
			update |= DefaultTo(Ini, "Flash Policy Server", "Crossdomain XML File", "crossdomain.xml");
			update |= DefaultTo(Ini, "Flash Policy Server", "Port", "843");
			update |= DefaultTo(Ini, "Webserver", "Enabled", "true");
			update |= DefaultTo(Ini, "Webserver", "Port", "80");
			update |= DefaultTo(Ini, "Webserver", "Password", "Restart the server after changing this!");
			update |= DefaultTo(Ini, "Components", "Enabled", "true");
			update |= DefaultTo(Ini, "Components", "Ignore", "example,list,of,DLLs,to,ignore,for,instance,the,Default,component,is,now,ignored");
			if(update)
			{
				std::string Default (Ini);
				std::ofstream ini ("Config.ini", std::ofstream::out|std::ofstream::binary|std::ofstream::trunc);
				if(ini.is_open())
				{
					ini.write(Default.c_str(), Default.length());
					Log << "Updated Config.ini" << std::endl;
				}
				else
				{
					Err << "Could not update Config.ini" << std::endl;
				}
			}

			if(Ini("Webserver", "Enabled") == "true")
			{
				if(Ini("Webserver", "Password") == "Restart the server after changing this!")
				{
					Err << "Please change the webserver password" << std::endl;
				}
				else if(Ini("Webserver", "Password") == "")
				{
					Err << "The webserver password cannot be blank" << std::endl;
				}
			}
		}
	} //ini ifstream is closed & destroyed

	if(Ini("Components", "Enabled") == "true")
	{
		std::vector<std::string> Ignore; Ignore.push_back("");
		for(std::string::iterator it = Ini("Components", "Ignore").begin(); it != Ini("Components", "Ignore").end(); ++it)
		{
			if(*it == ',')
			{
				Ignore.push_back("");
				continue;
			}
			Ignore.back() += *it;
		}
		WIN32_FIND_DATA fd;
		HANDLE file = FindFirstFile(".\\Components\\*.dll", &fd);
		do
		{
			if(file != INVALID_HANDLE_VALUE)
			{
				bool ignore = false;
				for(unsigned long i = 0; i < Ignore.size(); ++i)
				{
					if(!Ignore[i].empty() && std::string(fd.cFileName).find(Ignore[i]+".dll") != std::string::npos)
					{
						Log << "Ignoring component/DLL: " << Ignore[i]+".dll" << std::endl;
						ignore = true;
						break;
					}
				}
				if(ignore)
				{
					continue;
				}
				HMODULE lib = LoadLibrary((std::string(".\\Components\\") += fd.cFileName).c_str());
				if(lib)
				{
					try
					{
						Components.push_back(lib);
						unsigned long Version = Components.back().Version();
						if(Version < 0)
						{
							Err << fd.cFileName << " is out of date (Version: " << Version << ")" << std::endl;
						}
						else if(Version > 0)
						{
							Err << fd.cFileName << " is of a newer version (Version: " << Version << ")" << std::endl;
						}
						Log << "Loaded component: " << Components.back().Name << std::endl;
					}
					catch(Component::WindowsException e)
					{
						Err << "Error loading component functions: " << fd.cFileName << std::endl
							<< "Windows Error Numbers: " << e.what() << std::endl;
					}
					catch(std::exception e)
					{
						Err << "Error loading component functions: " << fd.cFileName << std::endl
							<< "Exception: " << e.what() << std::endl;
					}
					catch(...)
					{
						Err << "Error loading component functions: " << fd.cFileName << std::endl;
					}
				}
				else
				{
					Err << "Error loading component: " << fd.cFileName << std::endl
						<< "Windows Error Number: " << GetLastError() << std::endl;
				}
			}
			else
			{
				Err << "Error searching for components" << std::endl
					<< "Windows Error Number: " << GetLastError() << std::endl;
				break;
			}
		}while(FindNextFile(file, &fd) != 0);
		FindClose(file);
	}
	else
	{
		Log << "Components disabled by Configuration file" << std::endl;
	}

	RelayServer.onConnect(OnConnect);
	RelayServer.onDisconnect(OnDisconnect);

	RelayServer.onError(OnError);
	FlashPolicy.onError(OnError); //different overload

	RelayServer.onServerMessage(OnServerMessage);
	RelayServer.onChannelMessage(OnChannelMessage);
	RelayServer.onPeerMessage(OnPeerMessage);

	RelayServer.onJoinChannel(OnJoinChannel);
	RelayServer.onLeaveChannel(OnLeaveChannel);

	RelayServer.onSetName(OnSetName);


	Webserver.onGet(OnGet);
	Webserver.onPost(OnPost);
	Webserver.onHead(OnHead);
	
	Webserver.onDisconnect(OnDisconnect);
	
	Webserver.onError(OnError);

	Webserver.onUploadStart(OnUploadStart);
	Webserver.onUploadChunk(OnUploadChunk);
	Webserver.onUploadDone(OnUploadDone);
	Webserver.onUploadPost(OnUploadPost);


	((RelayServerTag*)RelayServer.Tag)->ChannelListing = Ini("Relay Server", "Channel Listing") == "true";
	RelayServer.SetChannelListing(((RelayServerTag*)RelayServer.Tag)->ChannelListing);
	((RelayServerTag*)RelayServer.Tag)->MOTD = Ini("Relay Server", "Welcome Message");
	RelayServer.SetWelcomeMessage(((RelayServerTag*)RelayServer.Tag)->MOTD.c_str());
	RelayServer.Host(ConvTo<int>::f(Ini("Relay Server", "Port")));
	if(RelayServer.Hosting())
	{
		Log << "Hosting Relay Server on port " << RelayServer.Port() << std::endl;
	}

	if(Ini("Flash Policy Server", "Enabled") == "true")
	{
		((FlashPolicyTag*)FlashPolicy.Tag)->HostingPort = ConvTo<int>::f(Ini("Flash Policy Server", "Port"));
		((FlashPolicyTag*)FlashPolicy.Tag)->XML = Ini("Flash Policy Server", "Crossdomain XML File");
		FlashPolicy.Host(((FlashPolicyTag*)FlashPolicy.Tag)->XML.c_str(), ((FlashPolicyTag*)FlashPolicy.Tag)->HostingPort);
	}
	if(FlashPolicy.Hosting())
	{
		Log << "Hosting Flash Policy on port " << Ini("Flash Policy Server", "Port") << std::endl;
	}

	if(Ini("Webserver", "Enabled") == "true")
	{
		Webserver.Host(ConvTo<int>::f(Ini("Webserver", "Port")));
	}
	if(Webserver.Hosting())
	{
		Log << "Hosting Webserver on port " << Webserver.Port() << std::endl;
	}

	try
	{
		//EventPump.StartEventLoop();
		Lacewing::Error *e;
		while(Run)
		{
			if(e = EventPump.Tick())
			{
				Err << "Error: " << e->ToString() << std::endl;
				delete e; //required: http://lacewing-project.org/docs/eventpump/Tick.html
			}
		}
	}
	catch(std::exception e)
	{
		Err << "Exception caught: " << e.what() << std::endl << "Press enter to close the server";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	catch(...)
	{
		Err << "Unkown Exception caught, press enter to close the server";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	delete (RelayServerTag*)RelayServer.Tag;
	delete (FlashPolicyTag*)FlashPolicy.Tag;
}

bool DefaultTo(INI &Ini, const std::string &group, const std::string &item, const std::string &value)
{
	static bool changed = false;
	if(!Ini.ItemExists(group, item))
	{
		Ini(group, item) = value;
		changed = true;
		Log << "Missing \"" << group << "\", \"" << item << "\" from Config.ini - using default" << std::endl;
	}
	return(changed);
}

int __stdcall EmergencyCleanup(unsigned long CtrlType)
{
	if(CtrlType == 0)
	{
		Log << "Closed by Ctrl+C" << std::endl;
	}
	else if(CtrlType == 1)
	{
		Log << "Closed by Ctrl+Break" << std::endl;
	}
	else if(CtrlType == 2)
	{
		Log << "Closed by Close button" << std::endl;
	}

	if(Webserver.Hosting()){ Webserver.Unhost(); }
	if(FlashPolicy.Hosting()){ FlashPolicy.Unhost(); }
	if(RelayServer.Hosting()){ RelayServer.Unhost(); }

	delete (RelayServerTag*)RelayServer.Tag;
	delete (FlashPolicyTag*)FlashPolicy.Tag;

	Webserver.~Webserver();
	FlashPolicy.~FlashPolicy();
	RelayServer.~RelayServer();
	EventPump.~EventPump();
	Ini.~INI();
	Components.~vector();
	Err.~Logger();
	Log.~Logger();

	return(FALSE);
}

