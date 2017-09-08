// IPv4.c version 1.0 beta
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
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

IPv4Addr * CreateIPv4Addr(const char * str)
{
	int * octets;
	char Class;
	IPv4Addr * addr = NULL; 

	if (!(octets = InitOctets(str))) // octets must be freed later
	{
		return NULL;
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
		goto BREAK;
	}

	// initialize IPv4Addr
	if(addr = (IPv4Addr *) malloc(sizeof(IPv4Addr)))
	{
		addr->first_octet = octets[0], addr->second_octet = octets[1], addr->third_octet = octets[2], addr->fourth_octet = octets[3];
		addr->Class = Class;
		addr->IsInitialized = true;
	}

	BREAK:free(octets);

	return addr;
}

bool IsIPv4AddrUnicast(const IPv4Addr * addr)
{
	if (addr->IsInitialized && (addr->Class == 'A' || addr->Class == 'B' || addr->Class == 'C')) 
	{
		return true;
	}

	return false;
}

// according to RFC1918
bool IsIPv4AddrPrivate(const IPv4Addr * addr)
{
	// make sure octets are in place
	if (!addr->IsInitialized) 
	{
		return false;
	}
	
	// 10.0.0.0/8
	if (addr->first_octet == 10)
	{
		return true;
	}
	
	// 172.16.0.0/12
	else if (addr->first_octet == 172 && (addr->second_octet > 15 && addr->second_octet < 32))
	{
		return true;
	}

	// 192.168.0.0/24
	else if (addr->first_octet == 192 && addr->second_octet == 168)
	{
		return true;
	}

	return false;
}

char GetClassOfIPv4Addr(IPv4Addr * addr)
{
	char Class;

	if (addr->IsInitialized)
	{
		return addr->Class;
	}

	// class A
	if (addr->first_octet > 0 && addr->first_octet < 127) 
	{
		Class = 'A';
	}

	// class B
	else if (addr->first_octet > 127 && addr->first_octet < 192) 
	{
		Class = 'B';
	}

	// class C
	else if (addr->first_octet > 191 && addr->first_octet < 224) 
	{
		Class = 'C';
	}

	// class D
	else if (addr->first_octet > 223 && addr->first_octet < 240) 
	{
		Class = 'D';
	}

	// class E
	else if (addr->first_octet > 239 && addr->first_octet < 248) 
	{
		Class = 'E';
	}

	// not classified
	else 
	{
		Class = 'N';
	}

	return Class;
}

char * GetIPv4AddrAsString(const IPv4Addr * addr)
{
	// big enough to hold an IPv4 address which is 16 characters long at maximum
	char * addrp = (char *) malloc( 16 * sizeof(char));

	if (addrp)
	{
		sprintf(addrp, "%d.%d.%d.%d", addr->first_octet, addr->second_octet, addr->third_octet, addr->fourth_octet);
		return addrp;
	}

	return NULL;
}

// IPv4 networks
IPv4Mask * CreateIPv4Mask(const char * str)
{
	int START = 0, end_of_array = 8;
	int * octets;
	bool all_octets_zero, all_octets_one;
	IPv4Mask * mask = NULL; 

	if (!(octets = InitOctets(str))) //octets must be freed later
	{
		return NULL;
	}

	// ensure that not all four octets are zero or one(mask-specific condition)
	all_octets_zero = (octets[0] == 0 && octets[1] == 0 && octets[2] == 0 && octets[3] == 0);
	all_octets_one = (octets[0] == 255 && octets[1] == 255 && octets[2] == 255 && (octets[3] == 255 || octets[3] == 254));

	if (all_octets_zero || all_octets_one) 
	{
		 goto BREAK;
	}
		
	// valid mask octet values
	int valid_mask_octets[9] = {0, 128, 192, 224, 240, 248, 252, 254, 255};

	// compare each octet of octets to each value of valid mask octets array to find a match
	for (int i = 0; i < 4; i++)
	{	
		// over valid mask octets array
		for (int j = START; j < 9; j++)
		{
			if (octets[i] == valid_mask_octets[j])
			{
				if (octets[i] != 255)
				{
					// all next octes must be zero as their value(mask-specific condition)
					valid_mask_octets[end_of_array] = 0; // thus 0 is the only allowed value for next octets
					START = end_of_array; // change the starting point of search
					break;
			 	}
			}
			else if (j == end_of_array) // and there is no matching
			{
				  goto BREAK; 
			}
		}
	}

	// Initialize IPv4Mask
	if (mask = (IPv4Mask *) malloc(sizeof(IPv4Mask)))
	{
		mask->first_octet = octets[0], mask->second_octet = octets[1], mask->third_octet = octets[2], mask->fourth_octet = octets[3];
		mask->IsInitialized = true;
	}

	BREAK:free(octets);

	return mask;
}

// networks
IPv4Network * CreateIPv4Network(const IPv4Addr * id, const IPv4Mask * mask)
{
	IPv4Network * network; 

	if (!(mask->IsInitialized && IsIPv4AddrUnicast(id)))
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

unsigned long GetNumberOfIPv4Hosts(const IPv4Network * network)
{
	unsigned int first_hosts, second_hosts, third_hosts, fourth_hosts;

	first_hosts = 256 - network->mask.first_octet;
	second_hosts = 256 - network->mask.second_octet;
	third_hosts = 256 - network->mask.third_octet;
	fourth_hosts = 256 - network->mask.fourth_octet;

	// total hosts
	return (unsigned long) (first_hosts * second_hosts * third_hosts * fourth_hosts - 2);
}

IPv4Addr * GetFirstUsableIPv4Addr(const IPv4Network * network)
{
	IPv4Addr * addr = (IPv4Addr *) malloc(sizeof(IPv4Addr));

	if (!addr)
	{
		return NULL;
	}

	*addr = network->id;
	addr->fourth_octet++;

	return addr;
}

IPv4Addr * GetLastUsableIPv4Addr(const IPv4Network * network)
{
	IPv4Addr * addr = (IPv4Addr *) malloc(sizeof(IPv4Addr));
	unsigned long squared_256 = 256 * 256;
	unsigned long cubed_256 = 256 * 256 * 256;
	unsigned long total_hosts = GetNumberOfIPv4Hosts(network);
	*addr = network->id;

	if (!addr)
	{
		return NULL;
	}
	
	if (total_hosts > cubed_256)
	{
		addr->first_octet += total_hosts / cubed_256;
		total_hosts = total_hosts % cubed_256;
	}

	if (total_hosts > squared_256)
	{
		addr->second_octet += total_hosts / squared_256;
		total_hosts = total_hosts % squared_256;
	}

	if (total_hosts > 256)
	{
		addr->third_octet += total_hosts / 256;
	}
	
	addr->fourth_octet += total_hosts % 256;

	return addr;
}

IPv4Addr * GetBroadcastIPv4Addr(const IPv4Network * network)
{
	IPv4Addr * bcast_addr = (IPv4Addr *) malloc(sizeof(IPv4Addr));

	if (!bcast_addr)
	{
		return NULL;
	}

	bcast_addr = GetLastUsableIPv4Addr(network);
	bcast_addr->fourth_octet++;
	bcast_addr->IsInitialized = true;

	return bcast_addr;
}

bool DoesBelongToIPv4Network(const IPv4Network * network, const IPv4Addr * addr, const IPv4Mask * mask)
{
	if (!(network->IsInitialized && addr->IsInitialized && mask->IsInitialized))
	{
		return false;
	}

	if (network->id.first_octet != (addr->first_octet & mask->first_octet))
	{
		return false;
	}

	if (network->id.second_octet != (addr->second_octet & mask->second_octet))
	{
		return false;
	}

	if (network->id.third_octet != (addr->third_octet & mask->third_octet))
	{
		return false;
	}

	if (network->id.fourth_octet != (addr->fourth_octet & mask->fourth_octet))
	{
		return false;
	}

	return true;
}

bool IsIPv4NetworkClassful(const IPv4Network * network)
{
	IPv4Mask mask = network->mask;
	char Class = network->id.Class;

	if (!network->IsInitialized)
	{
		return false;
	}

	// class A
	if (Class == 'A' && mask.first_octet == 255 && mask.second_octet == 0 && mask.third_octet == 0 && mask.fourth_octet == 0)
	{
		return true;
	}

	// class B
	else if (Class == 'B' && mask.first_octet == 255 && mask.second_octet == 255 && mask.third_octet == 0 && mask.fourth_octet == 0)
	{
		return true;
	}

	// class C
	else if (Class == 'C' && mask.first_octet == 255 && mask.second_octet == 255 && mask.third_octet == 255 && mask.fourth_octet == 0)
	{
		return true;
	}

	return false;
}

inline char ** GetIPv4HostAddrs(const IPv4Network * network, unsigned long limit)
{
	int first_octet = network->id.first_octet, second_octet = network->id.second_octet;
	int third_octet = network->id.third_octet, fourth_octet = network->id.fourth_octet;

	unsigned long total_hosts = GetNumberOfIPv4Hosts(network);

	// use limit as a maximum limit and ensure limit is not beyond value of total_hosts
	total_hosts = total_hosts < limit ? total_hosts : limit;
	
	char * host_addrs = (char *) malloc(total_hosts * 16 * sizeof(char));
	char ** host_addrs_heads = (char **) malloc(total_hosts * sizeof(char *));

	if (!(host_addrs && host_addrs_heads))
	{
		puts("Error(GetHostAddrs function): Couldn't receive requested memory.");

		if(host_addrs)
		{
			free(host_addrs);
		}

		else
		{
			free(host_addrs_heads);
		}

		return NULL;
	}

	// go over host addresses
	for (unsigned long host = total_hosts, i = 0, j = 0; host != 0; host--)
	 {
		// octets' algorithm
	 	if (++fourth_octet == 256)
		{
			fourth_octet = 0;

			if (++third_octet == 256)
			{
				third_octet = 0;

				if (++second_octet == 256)
				{
					second_octet = 0;
					first_octet++;
				}
			}
		}

		sprintf(host_addrs + j, "%d.%d.%d.%d", first_octet, second_octet, third_octet, fourth_octet);
		host_addrs_heads[i++] = host_addrs + j;
		j += 16;
	 }

	 return host_addrs_heads;
}

bool ReadIPv4HostAddrsIntoAFile(IPv4Network * network, unsigned long limit, FILE * IPv4File)
{
	int first_octet = network->id.first_octet, second_octet = network->id.second_octet;
	int third_octet = network->id.third_octet, fourth_octet = network->id.fourth_octet;
	
	unsigned long total_hosts = GetNumberOfIPv4Hosts(network);

	// use limit as a maximum limit and ensure limit is not beyond value of total_hosts
	total_hosts = total_hosts < limit ? total_hosts : limit;
	
	// ensure that file is readable
	if (!IPv4File) 
	{
		return false;
	}

	// go over host addresses
	for (unsigned long host = total_hosts; host != 0; host--)
	 {
		 // octets' algorithm
		 if (++fourth_octet == 256)
		 {
			 fourth_octet = 0;

			 if (++third_octet == 256)
			 {
				 third_octet = 0;

				 if (++second_octet == 256)
				 {
					 second_octet = 0;
					 first_octet++;
				 }
			 }
		 }

		 fprintf(IPv4File, "%d.%d.%d.%d,", first_octet, second_octet, third_octet, fourth_octet);
	 }

	return true;
}

static unsigned int GetNumberOfBits(int value)
{
	enum value_bits {one_bit = 128, two_bits = 192, three_bits = 224, four_bits = 240, five_bits = 248, six_bits = 252, seven_bits = 254, eight_bits = 255};
	
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
	unsigned  long subnets;
	
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

		return 1 << subnets;
	}

	else if (network->id.Class == 'C' && network->mask.fourth_octet > 0)
	{
		subnets = GetNumberOfBits(network->mask.fourth_octet);

		return 1 << subnets;
	}

	return 0;
}

char ** ListOfAvailableIPv4Subnets(IPv4Network * network, unsigned long limit)
{
	int first_octet, second_octet, third_octet, fourth_octet;

	IPv4Addr subnet_id;
	IPv4Addr * last_addr;

	unsigned long total_subnets = GetNumberOfIPv4Subnets(network);
	
	char * subnets = (char *) malloc(total_subnets * 16 * sizeof(char));
	char ** subnet_heads = (char **) malloc(total_subnets * sizeof(char *));
	
	if (!(subnets && subnet_heads))
	{
		puts("Couldn't receiving requested memory.");
		
		if (subnets)
		{
			free(subnets);
		}

		else
		{
			free(subnet_heads);
		}

		return NULL;
	}

	// ensure network is initialized and there is at least one IPv4 subnet
	if (!(network->IsInitialized && total_subnets))
	{	
		return NULL;
	}

	// determine the classfull network's id
	if (network->id.Class == 'A')
	{
		first_octet = network->id.first_octet;
		second_octet = third_octet = fourth_octet = 0;
	}

	else if (network->id.Class == 'B')
	{
		first_octet = network->id.first_octet;
		second_octet = network->id.second_octet;
		third_octet = fourth_octet = 0;
	}

	else if (network->id.Class == 'C')
	{
		first_octet = network->id.first_octet;
		second_octet = network->id.second_octet;
		third_octet = network->id.third_octet;
		fourth_octet = 0;
	}

	// use limit as a maximum limit and ensure limit is not beyond value of total_subnets
	total_subnets = total_subnets < limit ? total_subnets : limit;
	
	// generating IPv4 subnets in case they are available
	for (unsigned long subnet = total_subnets, j = 0, i = 0; subnet != 0; subnet--)
	{
		sprintf(subnets + j, "%d.%d.%d.%d",first_octet, second_octet, third_octet, fourth_octet);
		subnet_heads[i++] = subnets + j;
		j += 16;
		
		// subnet id of next subnet
		subnet_id = (IPv4Addr) {first_octet, second_octet, third_octet, fourth_octet, 'A', true}; // notice that value of class is a dummy one
		network = CreateIPv4Network(&subnet_id, &network->mask);
		last_addr = GetLastUsableIPv4Addr(network);
		first_octet = last_addr->first_octet;
		second_octet = last_addr->second_octet;
		third_octet = last_addr->third_octet;
		fourth_octet = last_addr->fourth_octet + 2;

		free(network);
		free(last_addr);
		
		// octets' algorithm
		if (fourth_octet == 256)
		{
			fourth_octet = 0;

			if (++third_octet == 256)
			{
				third_octet = 0;

				if (++second_octet == 256)
				{
					second_octet = 0;
					first_octet++;
				}
			}
		}
	}
	
	return subnet_heads;
}
