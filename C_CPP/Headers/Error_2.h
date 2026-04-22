#ifndef _error_h
#define _error_h
//////////////////////////////////////////////////////
// Преобразует цифровой код ошибки в нормальный тескт
//////////////////////////////////////////////////////
void ShowError()
{
LPSTR lpMsgBuf;

FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    WSAGetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL
);

// Display the string.
MessageBox( NULL, lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION );

// Free the buffer.
LocalFree( lpMsgBuf );
}

#endif