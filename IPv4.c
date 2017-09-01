// IPv4.c. Version 1.0
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "IPv4.h" 

static int * InitOctets(const char * str)
{
	char ch;
	int octet; 
	char temp_octet[4];
	bool last_char = false;
	int octets_i = 0;

	// malloc for four (int) octets
	int * octets = (int *) malloc(4 * sizeof(int));

	for (int str_i = 0, temp_octet_i = 0; str_i < 16; str_i++)
	{
		if (!(ch = str[str_i]) || ch == '\n' || str_i == 15) last_char = true;

		if (isdigit(ch))
		{	
			if (temp_octet_i == 3) 
			{
				break; // there are more than 3 digits
			}

			temp_octet[temp_octet_i++] = ch;
			temp_octet[temp_octet_i] = '\0'; // null character
		}

		else if ((ch == SEPARATOR || last_char) && temp_octet_i > 0)
		{
			octet = atoi(temp_octet);
			if (!(octet >= 0 && octet <= 255)) 
			{
				break;
			}

			octets[octets_i++] = octet;
			temp_octet_i = 0; // reset for the next octet
		}

		else 
		{
			break;
		}

		if (last_char) 
		{
			break; // no need for further checking
		}
	};

	// total received octets
	if (octets_i == 4)
	 {
		 return octets;
	 }
	
	// otherwise
	free(octets);
	
	return NULL;
}

//
IPv4Addr * IsIPv4Addr(const char * str)
{
	int * octets;
	char Class;
	IPv4Addr * addr; 

	if (!(octets = InitOctets(str))) // octets must be freed later
	{
		return false;
	}

	// class A
	if (octets[0] > 0 && octets[0] < 127) 
	{
		Class = 'A';
	}

	// class B
	else if (octets[0] > 127 && octets[0] < 192) 
	{
		Class = 'B';
	}

	// class C
	else if (octets[0] > 191 && octets[0] < 224) 
	{
		Class = 'C';
	}

	// class D
	else if (octets[0] > 223 && octets[0] < 240) 
	{
		Class = 'D';
	}

	// class E
	else if (octets[0] > 239 && octets[0] < 248) 
	{
		Class = 'E';
	}

	// not classified
	else 
	{
		return false;
	}

	// initialize IPv4Addr
	if(addr = (IPv4Addr *) malloc(sizeof(IPv4Addr)))
	{
		addr->first_octet = octets[0], addr->second_octet = octets[1], addr->third_octet = octets[2], addr->fourth_octet = octets[3];
		addr->Class = Class;
		addr->IsInitialized = true;
	}

	free(octets);

	return addr;
}

bool IsUnicastIPv4Addr(const IPv4Addr * addr)
{
	if (addr->IsInitialized && (addr->Class == 'A' || addr->Class == 'B' || addr->Class == 'C')) 
	{
		return true;
	}

	return false;
}

//
IPv4Mask * IsIPv4Mask(const char * str)
{
	int START = 0, end_of_array = 8;
	int * octets;
	bool all_octets_zero, all_octets_one;
	IPv4Mask * mask;
	
	if (!(octets = InitOctets(str))) //octets must be freed later
	{
		return NULL;
	}

	// ensure that not all four octets are zero or one(mask-specific condition)
	all_octets_zero = (octets[0] == 0 && octets[1] == 0 && octets[2] == 0 && octets[3] == 0);
	all_octets_one = (octets[0] == 255 && octets[1] == 255 && octets[2] == 255 && (octets[3] == 255 || octets[3] == 254));

	if (all_octets_zero || all_octets_one) 
	{
		return NULL;
	}
		
	// initialize mask_octets and valid_mask-octets arrays
	int valid_mask_octets[9] = {0, 128, 192, 224, 240, 248, 252, 254, 255};

	for (int i = 0; i < 4; i++)
	{	
		for (int j = START; j < 9; j++)
		{
			if (octets[i] == valid_mask_octets[j])
			{
				if (octets[i] != 255)
				{
					valid_mask_octets[end_of_array] = 0; // 0 is the only allowed value for next octets
					START = end_of_array; // change starting point of search instead of searching over the whole array
					break;
			 	}
			}
			else if (j == end_of_array) // and there is no matching
			{
				 return NULL; 
			}
		}
	}

	// Initialize IPv4Mask
	if (mask = (IPv4Mask *) malloc(sizeof(IPv4Mask)))
	{
		mask->first_octet = octets[0], mask->second_octet = octets[1], mask->third_octet = octets[2], mask->fourth_octet = octets[3];
		mask->IsInitialized = true;
	}

	free(octets);

	return mask;
}

// according to RFC1918
bool IsIPv4PrivateAddr(const IPv4Addr * addr)
{
	// make sure octets in place
	if (!addr->IsInitialized) 
	{
		return false;
	}
	
	// 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16
	if (addr->first_octet == 10 || (addr->first_octet == 172 && addr->second_octet >= 16 && addr->second_octet <= 31) || (addr->first_octet == 192 && addr->second_octet == 168))
	{	
		return true;
	}

	return false;
}

// networks
IPv4Network * CreateIPv4Network(const IPv4Addr * id, const IPv4Mask * mask)
{
	IPv4Network * network; 

	if (!(mask->IsInitialized && IsUnicastIPv4Addr(id)))
	{
		return NULL;
	}
	
	if (network = (IPv4Network *) malloc(sizeof(IPv4Network)))
	{
		network->id.first_octet = id->first_octet & mask->first_octet;
		network->id.second_octet = id->second_octet & mask->second_octet;
		network->id.third_octet = id->third_octet & mask->third_octet;
		network->id.fourth_octet = id->fourth_octet & mask->fourth_octet;
		network->id.Class = id->Class;
		network->id.IsInitialized = true;

		network->mask = *mask;

		network->IsInitialized = true;
	}
	
	return network;
}

unsigned long GetNumberOfHosts(const IPv4Network * network)
{
	unsigned long first_hosts = 1, second_hosts = 1, third_hosts = 1, fourth_hosts = 1;

	if (network->mask.first_octet < 255)
	{
		first_hosts = 256 - network->mask.first_octet;
	}
	
	if (network->mask.second_octet < 255)
	{	
		second_hosts = 256 - network->mask.second_octet;
	}

	if (network->mask.third_octet < 255)
	{	
		third_hosts = 256 - network->mask.third_octet;
	}

	if (network->mask.fourth_octet < 255)
	{	
		fourth_hosts = 256 - network->mask.fourth_octet;
	}

	return (first_hosts * second_hosts * third_hosts * fourth_hosts - 2);

}

IPv4Addr * GetFirstUsableAddr(const IPv4Network * network)
{
	IPv4Addr * addr = (IPv4Addr *) malloc(sizeof(IPv4Addr));

	if (!addr)
	{
		return NULL;
	}

	*addr = network->id;
	addr->fourth_octet++;
	addr->IsInitialized = true;

	return addr;
}

IPv4Addr * GetLastUsableAddr(const IPv4Network * network)
{
	IPv4Addr * addr = (IPv4Addr *) malloc(sizeof(IPv4Addr));
	unsigned long total_hosts = GetNumberOfHosts(network);
	unsigned long rem;
	*addr = network->id;

	if (!addr)
	{
		return NULL;
	}

	addr->fourth_octet += total_hosts % 256;

	addr->third_octet += (rem = total_hosts / 256) <= 255 ? rem : 255;

	addr->second_octet += (rem = total_hosts / (256 * 256)) <= (256 * 256) ? rem : 255;

	addr->first_octet += (rem = total_hosts / (256 * 256 * 256)) <= (256 * 256 * 256) ? rem : 255;

	return addr;
}

char * GetIPv4AddrAsString(const IPv4Addr * addr)
{
	// big enough to hold an IPv4 address which is 16 long characters at maximum
	char * addrp = (char *) malloc( 16 * sizeof(char));

	if (addrp)
	{
		sprintf(addrp, "%d.%d.%d.%d", addr->first_octet, addr->second_octet, addr->third_octet, addr->fourth_octet);
		return addrp;
	}

	return NULL;
}

char ** GetHostAddrs(const IPv4Network * network, unsigned long limit)
{
	int first_octet = network->id.first_octet, second_octet = network->id.second_octet;
	int third_octet = network->id.third_octet, fourth_octet = network->id.fourth_octet;

	unsigned long total_hosts = GetNumberOfHosts(network);

	// use limit as a maximum limit and ensure limit is not beyond value of total_hosts
	total_hosts = total_hosts < limit ? total_hosts : limit;
	
	char * host_addrs = (char *) malloc(total_hosts * 16 * sizeof(char));
	char ** host_addrs_heads = (char **) malloc(total_hosts * sizeof(char *));

	if (!(host_addrs && host_addrs_heads))
	{
		puts("Error(GetHostAddrs function): Couldn't receive needed memory.");
		return NULL;
	}

	for (unsigned long host = 1, i = 0, j = 0; host <= total_hosts; host++)
	 {
		 if (second_octet == 255 && third_octet == 255 && fourth_octet == 255)
		 {
			 first_octet++;
			 second_octet = 0;
			 third_octet = 0;
			 fourth_octet = 0;
		 }

		else if (third_octet == 255 && fourth_octet == 255)
		 {
			 second_octet++;
			 third_octet = 0;
			 fourth_octet = 0;
		 }

		else if (fourth_octet == 255)
		 {
			 third_octet++;
			 fourth_octet = 0;
		 }

		 else
		 {
			 fourth_octet++;
		 }

		 sprintf(host_addrs + j, "%d.%d.%d.%d", first_octet, second_octet, third_octet, fourth_octet);
		 host_addrs_heads[i++] = host_addrs + j;
		 j += 16;
	 }

	 return host_addrs_heads;
}

static unsigned int GetNumberOfBits(int value)
{
	enum value_bits {one_bit = 128, two_bits = 192, three_bits = 224, four_bits = 240, five_bits = 248, six_bits = 252, seven_bits = 254,\
					eight_bits = 255};

	switch(value)
	{
		case one_bit: return 1;
		case two_bits: return 2;
		case three_bits: return 3;
		case four_bits: return 4;
		case five_bits: return 5;
		case six_bits: return 6;
		case seven_bits: return 7;
		case eight_bits: return 8;
	}
}

unsigned long GetNumberOfIPv4Subnets(IPv4Network * network)
{
	unsigned int subnets;
	
	if (!network->IsInitialized)
	{
		return 0;
	}

	if (network->id.Class == 'A' && network->mask.second_octet > 0)
	{
		subnets = GetNumberOfBits(network->mask.second_octet);
		
		if (network->mask.third_octet > 0)
		{
			subnets += GetNumberOfBits(network->mask.third_octet);
		}

		if (network->mask.fourth_octet > 0)
		{
			subnets += GetNumberOfBits(network->mask.fourth_octet);
		}
		
		return 1 << subnets;
	}

	else if (network->id.Class == 'B' && network->mask.third_octet > 0)
	{	
		subnets = GetNumberOfBits(network->mask.third_octet);

		if (network->mask.fourth_octet > 0)
		{
			subnets += GetNumberOfBits(network->mask.fourth_octet);
		}

		return (unsigned long) 1 << subnets;
	}

	else if (network->id.Class == 'C' && network->mask.fourth_octet > 0)
	{
		subnets = GetNumberOfBits(network->mask.fourth_octet);

		return 1 << subnets;
	}

	return 0;
}