#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Lacewing.h"
#include "INI.hpp"
#include "Logger.hpp"
#include "TagStructs.hpp"
#include "Component.hpp"
#include "ConfigPanel.hpp"
#include "StringConvert.hpp"

//defined in Main.cpp:
extern Logger Log;
extern Logger Err;
extern INI Ini;
extern std::vector<Component> Components;
extern Lacewing::EventPump EventPump;
extern Lacewing::RelayServer RelayServer;
extern Lacewing::FlashPolicy FlashPolicy;
extern Lacewing::Webserver Webserver;
extern bool Run;

bool OnConnect(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client)
{
	bool allow = true;
	for(unsigned long i = 0; i < Components.size(); ++i)
	{
		allow &= Components[i].OnConnect(Server, Client);
	}
	return(allow);
}
void OnDisconnect(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client)
{
	for(unsigned long i = 0; i < Components.size(); ++i)
	{
		Components[i].OnDisconnect(Server, Client);
	}
}

void OnError(Lacewing::RelayServer &Server, Lacewing::Error &Error)
{
	Err << "Relay Server Error: " << Error.ToString() << std::endl;
}
void OnError(Lacewing::FlashPolicy &FlashPolicy, Lacewing::Error &Error)
{
	Err << "Flash Policy Error: " << Error.ToString() << std::endl;
}

void OnServerMessage(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, bool Blasted, int Subchannel, char * Data, int Size, int Variant)
{
	for(unsigned long i = 0; i < Components.size(); ++i)
	{
		Components[i].OnServerMessage(Server, Client, Blasted, Subchannel, Data, Size, Variant);
	}
}
bool OnChannelMessage(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, Lacewing::RelayServer::Channel &Channel, bool Blasted, int Subchannel, char * Data, int Size, int Variant)
{
	bool allow = true;
	for(unsigned long i = 0; i < Components.size(); ++i)
	{
		allow &= Components[i].OnChannelMessage(Server, Client, Channel, Blasted, Subchannel, Data, Size, Variant);
	}
	return(allow);
}
bool OnPeerMessage(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, Lacewing::RelayServer::Channel &Channel, Lacewing::RelayServer::Client &TargetClient, bool Blasted, int Subchannel, char * Packet, int Size, int Variant)
{
	bool allow = true;
	for(unsigned long i = 0; i < Components.size(); ++i)
	{
		allow &= Components[i].OnPeerMessage(Server, Client, Channel, TargetClient, Blasted, Subchannel, Packet, Size, Variant);
	}
	return(allow);
}

bool OnJoinChannel(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, Lacewing::RelayServer::Channel &Channel)
{
	bool allow = true;
	for(unsigned long i = 0; i < Components.size(); ++i)
	{
		allow &= Components[i].OnJoinChannel(Server, Client, Channel);
	}
	return(allow);
}
bool OnLeaveChannel(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, Lacewing::RelayServer::Channel &Channel)
{
	bool allow = true;
	for(unsigned long i = 0; i < Components.size(); ++i)
	{
		allow &= Components[i].OnLeaveChannel(Server, Client, Channel);
	}
	return(allow);
}

bool OnSetName(Lacewing::RelayServer &Server, Lacewing::RelayServer::Client &Client, const char * Name)
{
	bool allow = true;
	for(unsigned long i = 0; i < Components.size(); ++i)
	{
		allow &= Components[i].OnSetName(Server, Client, Name);
	}
	return(allow);
}

/* ******* Webserver ******* */
#define PageStart() "<html><head><title>lLacewing Server Control Panel</title></head><body>"

void OnGet(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request)
{
	if(std::string(Request.Session("new")) != "old")
	{
		Log << Request.GetAddress().ToString() << " connected to webserver interface" << std::endl;
		Request.Session("new", "old");
	}
	Request << PageStart();
	if(Ini("Webserver", "Password") == "Restart the server after changing this!") //If default password hasn't been changed
	{
		Request << "You must change the default password to use the webserver.";
		return;
	}
	if(Ini("Webserver", "Password") == "")
	{
		Request << "The webserver password cannot be blank.";
		return;
	}
	if(std::string(Request.Session("login")) != Ini("Webserver", "Password"))
	{
		Request <<
			"Please enter the password:"
			"<br />"
			"<form name=\"login\" action=\"/\" method=\"post\">"
			"<input type=\"password\" name=\"Password\" />"
			"<input type=\"submit\" value=\"Log in\" />"
			"<input type=\"hidden\" name=\"FormID\" value=\"Login Form\" />"
			"</form>";
		return;
	}
	Request <<
		"<form name=\"logout\" action=\"/\" method=\"post\">"
		"You are logged into the server control panel. "
		"<input type=\"submit\" value=\"Log out\" />"
		"<input type=\"hidden\" name=\"FormID\" value=\"Logout Form\" />"
		"</form>"
		"<br /><br />";
	Request <<
		"<form name=\"motd\" action=\"/\" method=\"post\">"
		"Welcome Message:<br />"
		"<textarea rows=\"3\" cols=\"80\" name=\"MOTD\">" << ((RelayServerTag*)RelayServer.Tag)->MOTD.c_str() << "</textarea><br />"
		"<input type=\"submit\" value=\"Update\" />"
		"<input type=\"hidden\" name=\"FormID\" value=\"MOTD\" />"
		"</form>";
	Request <<
		"<form name=\"channellisting\" action=\"/\" method=\"post\">"
		"Channel Listing " << (((RelayServerTag*)RelayServer.Tag)->ChannelListing ? "enabled" : "disabled") << "... "
		"<input type=\"submit\" value=\"Toggle\" />"
		"<input type=\"hidden\" name=\"FormID\" value=\"Channel Listing\" />"
		"</form>";
	//
	if(RelayServer.Hosting())
	{
		Request <<
			"<form name=\"relayunhost\" action=\"/\" method=\"post\">"
			"Relay Server: Hosting on port " << RelayServer.Port() << "... "
			"<input type=\"submit\" value=\"Stop Hosting\" />"
			"<input type=\"hidden\" name=\"FormID\" value=\"Relay Unhost\" />"
			"</form>";
	}
	else
	{
		Request <<
			"<form name=\"relayhost\" action=\"/\" method=\"post\">"
			"Relay Server: Not hosting... "
			"<input type=\"submit\" value=\"Try Hosting\" />"
			" Port: "
			"<input type=\"text\" name=\"Port\" value=\"" << Ini("Relay Server", "Port").c_str() << "\" />"
			"<input type=\"hidden\" name=\"FormID\" value=\"Relay Host\" />"
			"</form>";
	}
	if(FlashPolicy.Hosting())
	{
		Request <<
			"<form name=\"flashunhost\" action=\"/\" method=\"post\">"
			"Flash Policy Server: Hosting " << ((FlashPolicyTag*)FlashPolicy.Tag)->XML.c_str() << " on port " << ((FlashPolicyTag*)FlashPolicy.Tag)->HostingPort << "... "
			"<input type=\"submit\" value=\"Stop Hosting\" />"
			"<input type=\"hidden\" name=\"FormID\" value=\"Flash Unhost\" />"
			"</form>";
	}
	else
	{
		Request <<
			"<form name=\"flashhost\" action=\"/\" method=\"post\">"
			"Flash Policy Server: Not hosting... "
			"<input type=\"submit\" value=\"Try Hosting\" />"
			" Crossdomain XML File: "
			"<input type=\"text\" name=\"Crossdomain\" value=\"" << Ini("Flash Policy Server", "Crossdomain XML File").c_str() << "\" />"
			" Port: "
			"<input type=\"text\" name=\"Port\" value=\"" << Ini("Flash Policy Server", "Port").c_str() << "\" />"
			"<input type=\"hidden\" name=\"FormID\" value=\"Flash Host\" />"
			"</form>";
	}
	Request << "<br /><br />"
		"<form name=\"disablewebserver\" action=\"/\" method=\"post\">"
		"<input type=\"submit\" value=\"Disable this Webserver Interface\" />"
		"<input type=\"hidden\" name=\"FormID\" value=\"Disable\" />"
		"</form>"
		"<form name=\"shutdown\" action=\"/\" method=\"post\">"
		"<input type=\"submit\" value=\"Shutdown Entire Server\" />"
		"<input type=\"hidden\" name=\"FormID\" value=\"Shutdown\" />"
		"</form>";
	if(!Components.empty())
	{
		Request << "<br /><br />"
			"<h3>Components</h3>";
	}
	for(unsigned long i = 0; i < Components.size(); ++i)
	{
		Request <<
			"<form name=\"" << i << "\" action=\"/\" method=\"post\">"
			"<fieldset>"
			"<legend>" << Components[i].Name.c_str() << "</legend>";
		std::vector<ConfigPanel::Element*> Panel = Components[i].WebserverConfigPanel(RelayServer);
		for(unsigned long j = 0; j < Panel.size(); ++j)
		{
			Request << Panel[j]->HTML().c_str();
		}
		Request <<
			"<input type=\"hidden\" name=\"FormID\" value=\"" << Components[i].Name.c_str() << "\" />"
			"</fieldset>"
			"</form>";
	}
	Request << "</body></html>";
}
void OnPost(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request)
{
	if(std::string(Request.Session("login")) == "" && std::string(Request.POST("FormID")) != "Login Form")
	{
		return;
	}
	Request << PageStart();
	if(std::string(Request.POST("FormID")) == "Login Form")
	{
		if(Request.POST("Password") != Ini("Webserver", "Password"))
		{
			Request << "Incorrect password.</body></html>";
			Err << Request.GetAddress().ToString() << " used wrong password" << std::endl;
			return;
		}
		Request.Session("login", Ini("Webserver", "Password").c_str());
		Request << "Logging in...";
		Log << Request.GetAddress().ToString() << " logged in" << std::endl;
	}
	else if(std::string(Request.POST("FormID")) == "Logout Form")
	{
		Request.Session("login", "");
		Request << "Logging out...";
		Log << Request.GetAddress().ToString() << " logged out" << std::endl;
	}
	else if(std::string(Request.POST("FormID")) == "MOTD")
	{
		((RelayServerTag*)RelayServer.Tag)->MOTD = Request.POST("MOTD");
		RelayServer.SetWelcomeMessage(((RelayServerTag*)RelayServer.Tag)->MOTD.c_str());
		Request << "Updating Welcome Message...";
		Log << Request.GetAddress().ToString() << " upated Welcome Message" << std::endl;
	}
	else if(std::string(Request.POST("FormID")) == "Channel Listing")
	{
		RelayServer.SetChannelListing(((RelayServerTag*)RelayServer.Tag)->ChannelListing = !((RelayServerTag*)RelayServer.Tag)->ChannelListing);
		Request << "Toggling Channel Listing...";
		Log << Request.GetAddress().ToString() << " toggled channel listing to " << (((RelayServerTag*)RelayServer.Tag)->ChannelListing ? "enabled" : "disabled") << std::endl;
	}
	else if(std::string(Request.POST("FormID")) == "Relay Unhost")
	{
		RelayServer.Unhost();
		Request << "Stopping Relay Server...";
		Log << Request.GetAddress().ToString() << " stopped Relay Server" << std::endl;
	}
	else if(std::string(Request.POST("FormID")) == "Relay Host")
	{
		Request << "Trying to Host Relay Server...";
		Log << Request.GetAddress().ToString() << " requested to host Relay Server on port " << Request.POST("Port") << std::endl;
		RelayServer.Host(ConvTo<int>::f(Request.POST("Port")));
	}
	else if(std::string(Request.POST("FormID")) == "Flash Unhost")
	{
		FlashPolicy.Unhost();
		Request << "Stopping Flash Policy Server...";
		Log << Request.GetAddress().ToString() << " stopped Flash Policy Server" << std::endl;
	}
	else if(std::string(Request.POST("FormID")) == "Flash Host")
	{
		Request << "Trying to Host Flash Policy Server...";
		Log << Request.GetAddress().ToString() << " requested to host Flash Policy Server with " << Request.POST("Crossdomain") << " on port " << Request.POST("Port") << std::endl;
		((FlashPolicyTag*)FlashPolicy.Tag)->HostingPort = ConvTo<int>::f(Request.POST("Port"));
		((FlashPolicyTag*)FlashPolicy.Tag)->XML = Request.POST("Crossdomain");
		FlashPolicy.Host(((FlashPolicyTag*)FlashPolicy.Tag)->XML.c_str(), ((FlashPolicyTag*)FlashPolicy.Tag)->HostingPort);
	}
	else if(std::string(Request.POST("FormID")) == "Disable")
	{
		Request << "Goodbye!</body></hmtl>";
		Log << Request.GetAddress().ToString() << " disabled webserver interface" << std::endl;
		Webserver.EnableManualRequestFinish();
		Request.Finish();
		Webserver.Unhost();
		return;
	}
	else if(std::string(Request.POST("FormID")) == "Shutdown")
	{
		Request << "Goodbye!</body></hmtl>";
		Log << Request.GetAddress().ToString() << " shut down entire server" << std::endl;
		Webserver.EnableManualRequestFinish();
		Request.Finish();
		Webserver.Unhost();
		Run = false;
		return;
	}
	else
	{
		std::string search (Request.POST("FormID"));
		for(unsigned long i = 0; i < Components.size(); ++i)
		{
			if(search == Components[i].Name)
			{
				std::map<std::string, std::string> PostData;
				std::vector<ConfigPanel::Element*> Panel = Components[i].WebserverConfigPanel(RelayServer);
				for(unsigned long j = 0; j < Panel.size(); ++j)
				{
					PostData[Panel[j]->Name] = Request.POST(Panel[j]->Name.c_str());
				}
				Components[i].ConfigPanelData(RelayServer, PostData);
				break;
			}
		}
	}
	Request << "<meta http-equiv=\"refresh\" content=\"0\"></body></html>";
}
void OnHead(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request)
{
	//
}

void OnDisconnect(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request)
{
	Log << Request.GetAddress().ToString() << " disconnected from webserver" << std::endl;
}

void OnError(Lacewing::Webserver &Webserver, Lacewing::Error &Error)
{
	Err << "Webserver Error: " << Error.ToString() << std::endl;
}

void OnUploadStart(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request, Lacewing::Webserver::Upload &Upload)
{
	//
}
void OnUploadChunk(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request, Lacewing::Webserver::Upload &Upload, const char * Data, int Size)
{
	//
}
void OnUploadDone(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request, Lacewing::Webserver::Upload &Upload)
{
	//
}
void OnUploadPost(Lacewing::Webserver &Webserver, Lacewing::Webserver::Request &Request, Lacewing::Webserver::Upload * Uploads[], int UploadCount)
{
	//
}

