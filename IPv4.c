// IPv4.c. Version 1.0
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "IPv4.h" 

static int * InitOctets(char * str)
{
	char ch;
	int octet; 
	char temp_octet[4] = {'\0'};
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

		else if ((ch == '.' || last_char) && temp_octet_i > 0)
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

	if (octets_i == 4)
	 {
		 return octets;
	 }

	else 
	{
		free(octets);
	}
	
	return NULL;
}

//
IPv4Addr * IsIPv4Addr(char * str)
{
	int * octets;
	char Class;
	IPv4Addr * addr = NULL; 

	if (!(octets = InitOctets(str))) 
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

bool IsUnicastIPv4Addr(IPv4Addr * addr)
{
	if (addr->IsInitialized && (addr->Class == 'A' || addr->Class == 'B' || addr->Class == 'C')) 
	{
		return true;
	}

	return false;
}

//
IPv4Mask * IsIPv4Mask(char * str)
{
	int START = 0, end_of_array = 8;
	int * octets;
	bool all_octets_zero, all_octets_one;
	IPv4Mask * pt;
	
	if (!(octets = InitOctets(str))) 
	{
		return NULL;
	}

	// ensure that not all four octets are zero or one(mask-specific condition)
	all_octets_zero = (octets[0] == 0 && octets[1] == 0 && octets[2] == 0 && octets[3] == 0);
	all_octets_one = (octets[0] == 255 && octets[1] == 255 && octets[2] == 255 && octets[3] == 255);

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
	if (pt = (IPv4Mask *) malloc(sizeof(IPv4Mask)))
	{
		pt->first_octet = octets[0], pt->second_octet = octets[1], pt->third_octet = octets[2], pt->fourth_octet = octets[3];
		pt->IsInitialized = true;
	}

	free(octets);

	return pt;
}

/*
bool GetClass(IPv4Mask * mask)
{
	// class A
	if (mask->first_octet == 255 && (mask->second_octet > 0 | mask->third_octet > 0 || mask->fourth_octet > 0))
		mask->Class = 'A';
	
	// class B
	else if (mask->first_octet == 255 && mask->second_octet >= 240 && (mask->third_octet > 0 || mask->fourth_octet > 0))
		mask->Class = 'B';

	// class C
	else if (mask->first_octet == 255 && mask->second_octet == 255 && mask->third_octet == 255 && mask-fourth_octe > 0)
		mask->Class = 'C';

	// no class
	else
		return false;

	return true;
}
*/

// according to RFC1918
bool IsIPv4PrivateAddr(IPv4Addr * addr)
{
	// make sure octets in place
	if (!addr->IsInitialized) 
	{
		return false;
	}
	
	// initialize the first and second octets
	int octets[2] = {addr->first_octet, addr->second_octet};
	
	// 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16
	if (octets[0] == 10 || (octets[0] == 172 && octets[1] >= 16 && octets[1] <= 31) || (octets[0] == 192 && octets[1] == 168))
	{	
		return true;
	}

	return false;
}

/*
//preconditions: ipv4 must be intialized by the ItnitializeIPv4Address function
bool GetIPv4AddrAsBinary(const IPv4Addr * ipv4)
{
	// convert each octet to an integer
	int octets[4] = {atoi(ipv4->first_octet), atoi(ipv4->second_octet), atoi(ipv4->third_octet), atoi(ipv4->fourth_octet)};
	
	// Start to get the binary representation of each octet
	for (int octet_index = 0; octet_index < 4; octet_index++)
	{
		for (int shifts = BITS_PER_OCTET - 1; shifts >= 0; shifts--)
			printf("%d", octets[octet_index] >> shifts & 0x1);
		
		printf("%c", '.');
	};

	return true;
}

*/

// subnets
IPv4Subnet * CreateSubnet(IPv4Addr * id, IPv4Mask * mask)
{
	IPv4Subnet * subnet; 
	
	if (!(id->IsInitialized && mask->IsInitialized) || !IsUnicastIPv4Addr(id))
	{	
		return NULL;
	}

	// ensure mask is a valid mask regading subnet's id
	if (id->Class = 'A' && (!(mask->first_octet == 255 && ( mask->second_octet > 0 || mask->third_octet > 0 || mask->fourth_octet > 0))))
	{	
		return NULL;
	}
	
	if (subnet = (IPv4Subnet *) malloc(sizeof(IPv4Subnet)))
	{
		subnet->id.first_octet = id->first_octet & mask->first_octet;
		subnet->id.second_octet = id->second_octet & mask->second_octet;
		subnet->id.third_octet = id->third_octet & mask->third_octet;
		subnet->id.fourth_octet = id->fourth_octet & mask->fourth_octet;
		subnet->id.IsInitialized = true;

		subnet->mask = *mask;

		subnet->IsInitialized = true;
	}
	
	return subnet;
}

unsigned long GetNumberOfHosts(IPv4Subnet * subnet)
{
	unsigned long first_hosts = 1, second_hosts = 1, third_hosts = 1, fourth_hosts = 1;

	if (subnet->mask.first_octet < 255)
	{
		first_hosts = 256 - subnet->mask.first_octet;
	}
	
	if (subnet->mask.second_octet < 255)
	{	
		second_hosts = 256 - subnet->mask.second_octet;
	}

	if (subnet->mask.third_octet < 255)
	{	
		third_hosts = 256 - subnet->mask.third_octet;
	}

	if (subnet->mask.fourth_octet < 255)
	{	
		fourth_hosts = 256 - subnet->mask.fourth_octet;
	}

	return (first_hosts * second_hosts * third_hosts * fourth_hosts - 2);

}

