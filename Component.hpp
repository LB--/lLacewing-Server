#ifndef Component_HeaderPlusPlus
#define Component_HeaderPlusPlus
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <cstring>
#include <Windows.h>

#include "Lacewing.h"
#include "StringConvert.hpp"
#include "ConfigPanel.hpp"
#include "Logger.hpp"

extern Logger Log;
extern Logger Err;

class Component
{
	HMODULE DLL;
	Component();

	typedef std::string (__cdecl*InitFunc)(Logger &Log, Logger &Err);
	typedef std::vector<ConfigPanel::Element*> (*WCP)(Lacewing::RelayServer &Server);
	typedef void (*CPD)(Lacewing::RelayServer &Server, std::map<std::string, std::string> &PostData);
public:
	class WindowsException: public std::exception
	{
		char *msg;
	public:
		WindowsException() throw()
		{
			msg = new char[1];
			msg[0] = '\0';
		}
		WindowsException(const WindowsException &from) throw()
		{
			msg = new char[strlen(from.msg)+1];
			strcpy(msg, from.msg);
		}
		WindowsException(const char *from) throw()
		{
			msg = new char[strlen(from)+1];
			strcpy(msg, from);
		}
		virtual WindowsException &operator=(const WindowsException &from) throw()
		{
			delete[] msg;
			msg = new char[strlen(from.msg)+1];
			strcpy(msg, from.msg);
			return(*this);
		}
		virtual ~WindowsException() throw()
		{
			delete[] msg;
		}
		virtual const char *what() const throw()
		{
			return(msg);
		}
	};

	std::string Name;

private:
	InitFunc Init;
public:
	unsigned long (*Version)();
	Lacewing::RelayServer::HandlerConnect OnConnect;
	Lacewing::RelayServer::HandlerDisconnect OnDisconnect;
	Lacewing::RelayServer::HandlerServerMessage OnServerMessage;
	Lacewing::RelayServer::HandlerChannelMessage OnChannelMessage;
	Lacewing::RelayServer::HandlerPeerMessage OnPeerMessage;
	Lacewing::RelayServer::HandlerJoinChannel OnJoinChannel;
	Lacewing::RelayServer::HandlerLeaveChannel OnLeaveChannel;
	Lacewing::RelayServer::HandlerSetName OnSetName;

	WCP WebserverConfigPanel;
	CPD ConfigPanelData;
private:
	void (*Release)();
public:
	Component(const Component &from):
		DLL(from.DLL),
		Name(from.Name),
		Init(from.Init),
		Version(from.Version),
		OnConnect(from.OnConnect),
		OnDisconnect(from.OnDisconnect),
		OnServerMessage(from.OnServerMessage),
		OnChannelMessage(from.OnChannelMessage),
		OnPeerMessage(from.OnPeerMessage),
		OnJoinChannel(from.OnJoinChannel),
		OnLeaveChannel(from.OnLeaveChannel),
		OnSetName(from.OnSetName),
		WebserverConfigPanel(from.WebserverConfigPanel),
		Release(from.Release)
		{}
	Component &operator=(const Component &from)
	{
		DLL = from.DLL;

		Name = from.Name;

		Init = from.Init;
		Version = from.Version;
		OnConnect = from.OnConnect;
		OnDisconnect = from.OnDisconnect;
		OnServerMessage = from.OnServerMessage;
		OnChannelMessage = from.OnChannelMessage;
		OnPeerMessage = from.OnPeerMessage;
		OnJoinChannel = from.OnJoinChannel;
		OnLeaveChannel = from.OnLeaveChannel;
		OnSetName = from.OnSetName;
		WebserverConfigPanel = from.WebserverConfigPanel;
		Release = from.Release;

		return(*this);
	}
	Component(HMODULE Dll) : DLL(Dll)
	{
		unsigned long Error[13];
		Init = (InitFunc)GetProcAddress(DLL, "Init");																Error[0] = (Init ? 0 : GetLastError()); //GetProcAddress doesn't clear the last error
		Version = (unsigned long (*)())GetProcAddress(DLL, "version");												Error[1] = (Version ? 0 : GetLastError());
		OnConnect = (Lacewing::RelayServer::HandlerConnect)GetProcAddress(DLL, "OnConnect");						Error[2] = (OnConnect ? 0 : GetLastError());
		OnDisconnect = (Lacewing::RelayServer::HandlerDisconnect)GetProcAddress(DLL, "OnDisconnect");				Error[3] = (OnDisconnect ? 0 : GetLastError());
		OnServerMessage = (Lacewing::RelayServer::HandlerServerMessage)GetProcAddress(DLL, "OnServerMessage");		Error[4] = (OnServerMessage ? 0 : GetLastError());
		OnChannelMessage = (Lacewing::RelayServer::HandlerChannelMessage)GetProcAddress(DLL, "OnChannelMessage");	Error[5] = (OnChannelMessage ? 0 : GetLastError());
		OnPeerMessage = (Lacewing::RelayServer::HandlerPeerMessage)GetProcAddress(DLL, "OnPeerMessage");			Error[6] = (OnPeerMessage ? 0 : GetLastError());
		OnJoinChannel = (Lacewing::RelayServer::HandlerJoinChannel)GetProcAddress(DLL, "OnJoinChannel");			Error[7] = (OnJoinChannel ? 0 : GetLastError());
		OnLeaveChannel = (Lacewing::RelayServer::HandlerLeaveChannel)GetProcAddress(DLL, "OnLeaveChannel");			Error[8] = (OnLeaveChannel ? 0 : GetLastError());
		OnSetName = (Lacewing::RelayServer::HandlerSetName)GetProcAddress(DLL, "OnSetName");						Error[9] = (OnSetName ? 0 : GetLastError());
		WebserverConfigPanel = (WCP)GetProcAddress(DLL, "WebserverConfigPanel");									Error[10] = (WebserverConfigPanel ? 0 : GetLastError());
		ConfigPanelData = (CPD)GetProcAddress(DLL, "ConfigPanelData");												Error[11] = (WebserverConfigPanel ? 0 : GetLastError());
		Release = (void (*)())GetProcAddress(DLL, "ReleaseData");													Error[12] = (Release ? 0 : GetLastError());

		bool Throw = false;
		std::string exc;
		for(unsigned char i = 0; i < 13; ++i)
		{
			if(i)
			{
				exc += ',';
			}
			exc += ConvTo<std::string>::f(Error[i]);
			if(Error[i])
			{
				Throw = true;
			}
		}
		if(Throw)
		{
			throw(WindowsException(exc.c_str()));
		}
		Name = Init(Log, Err);
	}

	~Component()
	{
		Release();
	}
};

#endif
