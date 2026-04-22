#include "stuff.h"
#include "externals.h"
#include "strings.h"

void CodeString(unsigned int l)
{
	unsigned int	i, j;

	for (i = 0; i < strlen(strings[l].encoded); i++)
	{
		for (j = 0; j < strlen(decode_key); j++)
			strings[l].encoded[i] ^= decode_key[j];

		strings[l].encoded[i] = ~ strings[l].encoded[i];
	}
}

unsigned int Decode(stringtype stype)
{
	unsigned int	i = 0;

	for (i = 0; strings[i].stype != nothing; i++)
	{
		if (strings[i].stype == stype)
		{
			if (strings[i].isdecoded == 1)
				break;
			else
			{
				CodeString(i);
				strings[i].isdecoded = 1;
				break;
			}
		}
	}

	return i;
}

void ClearDecoded()
{
	unsigned int	i;

	for (i = 0; strings[i].stype != nothing; i++)
	{
		if (strings[i].isdecoded == 1)
		{
			CodeString(i);
			strings[i].isdecoded = 0;
		}
	}
}

void DecodeSemiString(char *str)
{
	unsigned int	i, j;

	for (i = 0; i < strlen(str); i++)
	{
		for (j = 0; j < strlen(decode_key); j++)
			str[i] ^= decode_key[j];

		str[i] = ~ str[i];
	}

	return;
}