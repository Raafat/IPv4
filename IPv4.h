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
	bool IsPrivate;
	bool IsUnicast;
	bool IsInitialized;
};

typedef struct IPv4Addr IPv4Addr;

IPv4Addr * IsIPv4Addr (char * str);

bool IsIPv4PrivateAddr(IPv4Addr * addr);

char * GetIPv4AddrAsString(const IPv4Addr * addr);

// masks
struct IPv4Mask
{
	int first_octet;
	int second_octet;
	int third_octet;
	int fourth_octet;
	char Class;// Is it a good idea ?
	bool IsInitialized;
};

typedef struct IPv4Mask IPv4Mask;

IPv4Mask * IsIPv4Mask(char * str);

// subnets
struct IPv4Subnet
{
	IPv4Addr id;
	IPv4Mask mask;
	unsigned long numberOfHosts;
	bool IsInitialized;
};

typedef struct IPv4Subnet IPv4Subnet;

IPv4Subnet * CreateSubnet(IPv4Addr * id, IPv4Mask * mask);

unsigned long GetNumberOfHosts(IPv4Subnet * subnet);

char ** GetHostsAddr(IPv4Subnet * subnet);

IPv4Addr * GetFirstUsableAddr(IPv4Subnet * subnet);

IPv4Addr * GetLastUsableAddr(IPv4Subnet * subnet);

#endif
