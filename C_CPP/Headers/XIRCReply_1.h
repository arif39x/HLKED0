bool XIRCRawSend(SOCKET sock, int Delay, char *Format, ...);
bool XIRCMessage(SOCKET sock, bool Delay, bool Notice, bool Silent, char *Destination, char *Format, ...);
bool XIRCAction(SOCKET sock, bool Delay, char *Action, char *Format, ...);