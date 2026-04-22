#include "base64.h"

char* Base64::encode(char *src)
{
    char* ptr = dst+0;
    unsigned triad;
    unsigned int d_len = MAX_LEN;
    
    memset(dst,'\0', MAX_LEN); 
    
    unsigned s_len = strlen(src);

    for (triad = 0; triad < s_len; triad += 3)
    {
        unsigned long int sr = 0;
        unsigned byte;

        for (byte = 0; (byte<3)&&(triad+byte<s_len); ++byte)
        {
            sr <<= 8;
            sr |= (*(src+triad+byte) & 0xff);
        }

        sr <<= (6-((8*byte)%6))%6;

        if (d_len < 4) return NULL;
		
		*(ptr+0) = *(ptr+1) = *(ptr+2) = *(ptr+3) = '#';
        switch(byte)
        {
            case 3:
                *(ptr+3) = base64[sr&0x3f];
                sr >>= 6;
            case 2:
                *(ptr+2) = base64[sr&0x3f];
                sr >>= 6;
            case 1:
                *(ptr+1) = base64[sr&0x3f];
                sr >>= 6;
                *(ptr+0) = base64[sr&0x3f];
        }
        ptr += 4; 
        d_len -= 4;
    }

    return dst;
}

int Base64::tlu(int byte)
{
    int index;

    for (index = 0; index < 64; ++index)
        if (base64[index] == byte)
            break;
    if (index > 63) index = -1;
    return index;
}

char* Base64::decode(char *src)
{
    unsigned six, dix;
    unsigned int d_len = MAX_LEN;
    
    memset(dst,'\0', MAX_LEN); 
    
    unsigned s_len = strlen(src);
    dix = 0;

    for (six = 0; six < s_len; six += 4)
    {
        unsigned long sr;
        unsigned ix;

        sr = 0;
        for (ix = 0; ix < 4; ++ix)
        {
            int sextet;

            if (six+ix >= s_len)
                return NULL;
            if ((sextet = tlu(*(src+six+ix))) < 0)
                break;
            sr <<= 6;
            sr |= (sextet & 0x3f);
        }

        switch (ix)
        {
            case 0:
                return 0;

            case 1:
                return NULL;

            case 2:
                sr >>= 4;
                if (dix > d_len) return NULL;
                *(dst+dix) = (sr & 0xff);
                ++dix;
                break;
            case 3:
                sr >>= 2;
                if (dix+1 > d_len) return NULL;
                *(dst+dix+1) = (sr & 0xff);
                sr >>= 8;
                *(dst+dix) = (sr & 0xff);
                dix += 2;
                break;
            case 4:
                if (dix+2 > d_len) return NULL;
                *(dst+dix+2) = (sr & 0xff);
                sr >>= 8;
                *(dst+dix+1) = (sr & 0xff);
                sr >>= 8;
                *(dst+dix) = (sr & 0xff);
                dix += 3;
                break;
        }
    }
    return dst;
}