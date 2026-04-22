#ifndef BASE64_H
#define BASE64_H

#include <string.h>

#define MAX_LEN 1024
const char base64[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                  "abcdefghijklmnopqrstuvwxyz0123456789+/";

class Base64 {
    
    private:
       char dst[MAX_LEN];
                       
    protected:
      int tlu(int byte);                
        
    public:
      Base64(){
         memset(dst,'\0', MAX_LEN);      
      }

      char* encode(char *src);
      
      char* decode(char *src);
};

#endif