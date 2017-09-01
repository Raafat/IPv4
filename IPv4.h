#ifndef _IPv4_H 
#include <stdbool.h>
#define TOTAL_OCTETS 4
#define BITS_PER_OCTET 8
#define SEPARATOR '.'

// IPv4 addresses
struct IPv4Addr
{
	int first_octet;
	int second_octet;
	int third_octet;
	int fourth_octet;
	char Class;
	bool IsInitialized;
};

typedef struct IPv4Addr IPv4Addr;

IPv4Addr * IsIPv4Addr (const char * str);

bool IsUnicastIPv4Addr(const IPv4Addr * addr);

bool IsIPv4PrivateAddr(const IPv4Addr * addr);

char * GetIPv4AddrAsString(const IPv4Addr * addr);

// masks
struct IPv4Mask
{
	int first_octet;
	int second_octet;
	int third_octet;
	int fourth_octet;
	bool IsInitialized;
};

typedef struct IPv4Mask IPv4Mask;

IPv4Mask * IsIPv4Mask(const char * str);

//networks 
struct IPv4Network
{
	IPv4Addr id;
	IPv4Mask mask;
	bool IsInitialized;
};

typedef struct IPv4Network IPv4Network;

IPv4Network * CreateIPv4Network(const IPv4Addr * id, const IPv4Mask * mask);

unsigned long GetNumberOfHosts(const IPv4Network * network);

IPv4Addr * GetFirstUsableAddr(const IPv4Network * network);

IPv4Addr * GetLastUsableAddr(const IPv4Network * network);

char ** GetHostAddrs(const IPv4Network * network, unsigned long limit);

unsigned long GetNumberOfIPv4Subnets(IPv4Network * network);

IPv4Network ** AvailableIPv4Subnets(IPv4Network * network);

#endif
