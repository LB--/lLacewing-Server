#ifndef LacewingTagStructures_HeaderPlusPlus
#define LacewingTagStructures_HeaderPlusPlus

#include <string>

struct RelayServerTag
{
	std::string MOTD;
	bool ChannelListing;
};

struct FlashPolicyTag
{
	std::string XML;
	int HostingPort;
};

#endif
