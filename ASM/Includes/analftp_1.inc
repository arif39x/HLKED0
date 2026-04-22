.data

FTPSession STRUCT
	ServSock	DWORD ?
	LoggedIn	DWORD ?
	User_Accepted	DWORD ?
	Renaming	DWORD ?
	ResumeOffset	DWORD	?
	CurrentType	DWORD	?
	lpCurrentDir	DWORD ?
	DTPMode		DWORD ?
	PortIP		DWORD ?
	PortPortNumb	DWORD ?
	PortSock	DWORD ?
	PasvSock	DWORD ?
	PasvPortNumb	DWORD ?
	DTPCommand	DWORD ?
	DTPPathBuff	DWORD ?
FTPSession ENDS


DTP_No_Mode		equ 0
DTP_Pasv_Mode		equ 1
DTP_Port_Mode		equ 2
DTP_List_Cmd		equ 1
DTP_Name_List_Cmd	equ 2
DTP_Retrieve_Cmd	equ 3
DTP_Store_Cmd		equ 4
DTP_Store_Unique_Cmd	equ 5
DTP_Append_Cmd		equ 6

;Command numbers
USER_Cmd	equ 1
PASS_Cmd	equ 2
CWD_Cmd		equ 3
CDUP_Cmd	equ 4
QUIT_Cmd	equ 5
PORT_Cmd	equ 6
PASV_Cmd	equ 7
TYPE_Cmd	equ 8
MODE_Cmd	equ 9
RETR_Cmd	equ 10
STOR_Cmd	equ 11
APPE_Cmd	equ 12
REST_Cmd	equ 13
RNFR_Cmd	equ 14
RNTO_Cmd	equ 15
ABOR_Cmd	equ 16
DELE_Cmd	equ 17
RMD_Cmd		equ 18
MKD_Cmd		equ 19
LIST_Cmd	equ 20
NLST_Cmd	equ 21
SYST_Cmd	equ 22
STAT_Cmd	equ 23
HELP_Cmd	equ 24
NOOP_Cmd	equ 25
SIZE_Cmd	equ 26
EXEC_Cmd	equ 27
PWD_Cmd		equ 28
szUser				db "USER"
szPass				db "PASS"
szChangeWorkingDir		db "CWD "
szChangeToParentDir		db "CDUP"
szQuit				db "QUIT"
szDataPort			db "PORT"
szPassive			db "PASV"
szRepresentationType		db "TYPE"
szTransferMode			db "MODE"
szRetrieve			db "RETR"
szStore				db "STOR"
szAppend			db "APPE"
szRestart			db "REST"
szRenameFrom			db "RNFR"
szRenameTo			db "RNTO"
szAbort				db "ABOR"
szDelete			db "DELE"
szRemoveDir			db "RMD "
szMakeDir			db "MKD "
szList				db "LIST"
szNameList			db "NLST"
szSystem			db "SYST"
szStatus			db "STAT"
szHelp				db "HELP"
szNoop				db "NOOP"
szSize				db "SIZE"
szExecute			db "EXEC"
szPrintWorkingDir		db "PWD",0


;Replys
szRep5	db "200 NOOP ok.",0
szRep7	db "211 Status: lol =)",0
szRep9	db "213 %lu",0
szRep10	db "214 Fack you!",0
szRep13	db "221 Bye!",0
szRep16	db "227 Entering Passive Mode (%i,%i,%i,%i,%i,%i).",0
szRep17	db "230 User logged in, proceed.",0
szRep19	db "257 directory created.",0
szRep20	db "331 Password required for %s.",0
szRep22	db "350 REST supported. Ready to resume at byte offset %lu.",0
szRep24	db "425 Can't open data connection.",0
szRep27	db "451 Requested action aborted: local error in processing.",0
szRep29	db "500 Syntax error, command unrecognized.",0
szRep30	db "501 Syntax error in parameters or arguments.",0
szRep32	db "503 Bad sequence of commands.",0
szRep34	db "530 Not logged in.",0
szRep40	db "530 Login or Password incorrect.",0
szRep42	db "200 Type set to %c.",0
szRep43 db "257 ""%s"" is current directory.",0
szRep44	db "150 Data connection accepted.",0
szRep45	db "226 Transfer ok",0
szRep46 db "215 UNIX Type: L8",0
szRep47 db "200 Port command successful.",0
szRep48 db "550 No port specified.",0
szRep49 db "150 Opening data connection.",0
szRep50 db "451 Failed: Cannot build data connection.",0
szRep51 db "250 CWD command successful.",0
szRep52 db "550 No such file or directory.",0
szRep53 db "426 Cannot retrieve. Failed. Aborting.",0
szRep54 db "266 ABOR command successful.",0
szRep55 db "250 File deleted successfully.",0
szRep56 db "250 Directory removed.",0
szRep57	db "350 File exists.  Ready for destination name.",0
szRep58 db "250 File renamed successfully.",0
szRep59 db "250 File executed successfully.",0


szDriveString db "drwxrwxrwx   1 ftp      ftp            0 Jan 01  1980 C:",13,10,0
szFileString db  "-rw-rw-rw-   1 ftp      ftp  %11lu %s %2.2i %s %s",13,10,0

szYearFormat db "%5i",0
szTimeFormat db "%2.2i:%2.2i",0 

;Dates for file creation date
szDateList	db "Jan",0
		db "Feb",0
		db "Mar",0
		db "Apr",0
		db "May",0
		db "Jun",0
		db "Jul",0
		db "Aug",0
		db "Sep",0
		db "Oct",0
		db "Nov",0
		db "Dec",0
szExeExtentions db ".exe"
NumbExeExtentions equ 6			
		db ".bat"
		db ".com"
		db ".scr"
		db ".cmd"
		db ".pif"
