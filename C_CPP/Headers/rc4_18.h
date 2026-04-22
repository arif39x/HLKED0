/* :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   ;:::                                                             :::
   ;:::  This function performs 'RC4' Stream Encryption             :::
   ;:::  (Based on what is widely thought to be RSA's RC4           :::
   ;:::  algorithm. It produces output streams that are identical   :::
   ;:::  to the commercial products)                                :::
   ;:::                                                             :::
   ;:::  Adapted by permission from a VB script by Mike Shaffer     :::
   ;:::  http://www.4guysfromrolla.com/webtech/010100-1.shtml       :::
   ;:::                                                             :::
   ;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */
void swapints(int *array, int ndx1, int ndx2)
{
    int temp = array[ndx1];
    array[ndx1] = array[ndx2];
    array[ndx2] = temp;
}

char *RC4(const char *pszText, int iTextLen, const char *pszKey)
/* ;:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   ;:::  This routine does all the work. Call it both to ENcrypt    :::
   ;:::  and to DEcrypt your data.                                  :::
   ;:::  You MUST free the returned pointer when no longer needed   :::
   ;::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: */
{
    char *cipher;                       /* Output buffer                */
    int a, b, i=0, j=0, k;              /* Ambiguously named counters   */
    int ilen;                           /* Length of a string           */
    int sbox[256];                      /* Encryption array             */
    int key[256];                       /* Numeric key values           */

    ilen = strlen(pszKey);

    for (a=0; a < 256; a++)
    {
        key[a] = pszKey[a % ilen];
        sbox[a] = a;
    }

    for (a=0, b=0; a < 256; a++)
    {
        b = (b + sbox[a] + key[a]) % 256;
        swapints(sbox, a, b);
    }

    cipher = (char *)malloc(iTextLen);

    for (a=0; a < iTextLen; a++)
    {
        i = (i + 1) % 256;
        j = (j + sbox[i]) % 256;
        swapints(sbox, i, j);
        k = sbox[(sbox[i] + sbox[j]) % 256];
        cipher[a] = pszText[a] ^ k;
    }
    return cipher;
}