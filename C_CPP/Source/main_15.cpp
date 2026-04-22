#include <stdio.h>
#include <string.h>

#define SERVERFILE				"..\\..\\server\\Release\\server.exe"
#define TEXTFILE				"..\\..\\client\\server.h"
#define	CHARSPERCOLUMN			16
#define MAXCHARSPERBLOCK		32768
#define MAXBLOCKS				8
#define HEAD		"char *serverfiledata[] = {\r\n"


int main(int argc, char *argv[])
{
	FILE				*rf, *wf;
	unsigned char		c;
	int					i = 1, lines = 1, block = 0;
	int					chars[MAXBLOCKS];
	char				temp[5] = {0};
	char				temp2[32];

	rf = fopen(SERVERFILE, "rb");
	wf = fopen(TEXTFILE, "wb");

	if (rf == NULL || wf == NULL)
	{
		printf("Failed to open file.\n");
		getchar();
		return 0;
	}

	fputs(HEAD, wf);

	printf("Head written.\n");

	chars[block] = 1;

	while (fread((void *)&c, 1, 1, rf) == 1)
	{
		printf("Writting lines: %d, block: %d, chars in block: %d", lines, block, chars[block]);
		// start of line
		if (i == 1)
		{
			if (chars[block] == MAXCHARSPERBLOCK + 1)
			{
				fputs(",\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", wf);
				block++;
				chars[block] = 1;
			}
			fputs("\"", wf);
		}

		// put in char
		sprintf(temp, "\\x%02X", c);
		fputs(temp, wf);

		// end of line
		if (i == CHARSPERCOLUMN)
		{
			fputs("\"\r\n", wf);
			i = 0;
			lines++;
		}

		i++;
		chars[block]++;
		memset(temp, 0, sizeof(temp));
		printf("\r");
	}

	fputs(",\nNULL\n};\n\n", wf);

	fputs("int charsinblock[] = { ", wf);
	for (i = 0; i <= block; i++)
	{
		sprintf(temp2, "%d, ", chars[i] - 1);
		fputs(temp2, wf);
	}
	fputs("0 };", wf);

	fclose(wf);
	fclose(rf);

	printf("\nDone.\n");
	getchar();

	return 0;
}