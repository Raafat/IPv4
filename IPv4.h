#ifndef _IPv4_H 
#include <stdbool.h>
#define TOTAL_OCTETS 4
#define BITS_PER_OCTET 8
#define SEP '.'

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

struct IPv4Mask
{
	int first_octet;
	int second_octet;
	int third_octet;
	int fourth_octet;
	char Class;
	bool IsInitialized;
};

typedef struct IPv4Mask IPv4Mask;

struct IPv4Subnet
{
	IPv4Addr id;
	IPv4Mask mask;
	unsigned long numberOfHosts;
	unsigned long numberOfSubnets;
	bool IsInitialized;
};

typedef struct IPv4Subnet IPv4Subnet;

IPv4Addr * IsIPv4Addr (char * str);

IPv4Mask * IsIPv4Mask(char * str);

bool IsIPv4PrivateAddr(IPv4Addr * addr);

IPv4Subnet * CreateSubnet(IPv4Addr * id, IPv4Mask * mask);

unsigned long GetNumberOfHosts(IPv4Subnet * subnet);
