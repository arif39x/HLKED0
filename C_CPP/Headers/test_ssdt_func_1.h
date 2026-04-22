/*
 *      name: It's just for testing SSDT Hook
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#pragma once


NTSTATUS NewNtCreateProcessEx(OUT PHANDLE  ProcessHandle,
												IN ACCESS_MASK        DesiredAccess,
											    IN DWORD ObjectAttributes OPTIONAL,
												IN HANDLE             ParentProcess,
												IN BOOLEAN            InheritObjectTable,
												IN HANDLE             SectionHandle OPTIONAL,
												IN HANDLE             DebugPort OPTIONAL,
												IN HANDLE             ExceptionPort OPTIONAL ,
												IN HANDLE			  UNKNOWN);

NTSTATUS NewZwQuerySystemInformation(
											  IN DWORD SystemInformationClass,
											  OUT PVOID               SystemInformation,
											  IN ULONG                SystemInformationLength,
											  OUT PULONG              ReturnLength OPTIONAL );

NTSTATUS NewNtQueryDirectoryFile(

						  IN HANDLE               FileHandle,
						  IN HANDLE               Event OPTIONAL,
						  IN DWORD      ApcRoutine OPTIONAL,
						  IN PVOID                ApcContext OPTIONAL,
						  OUT DWORD    IoStatusBlock,
						  OUT PVOID               FileInformation,
						  IN DWORD                strLength,
						  IN DWORD FileInformationClass,
						  IN BOOLEAN              ReturnSingleEntry,
						  IN DWORD      FileMask OPTIONAL,
						  IN BOOLEAN              RestartScan );
