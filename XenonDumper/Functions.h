#ifndef __Functions_H__
#define __Functions_H__
#pragma once
//using std::string;

extern HMODULE hAPP;
extern BOOL IsDevkit;

// =========================  ENABLE DEBUG PRINTS  ====================================
#define _DEBUG

#ifdef _DEBUG
VOID Byrom_Print(const CHAR* fmt, ...);
#define DbgPrint(x, ...) { do { Byrom_Print("[XenonDumper] "  x "\n", __VA_ARGS__); } while(0); }
#endif

#ifndef _DEBUG
#define DbgPrint
#endif

VOID XNotify(PWCHAR text);
PWCHAR LinkWChar(PWCHAR Text, ...);
BOOL FileExists(PCHAR path);
BOOL DirectoryExists(LPCTSTR szPath);
BOOL CWriteFile(const CHAR* FilePath, const VOID* Data, DWORD Size);

#define MOUNT_FLASH "Flash:"
#define DEVICE_FLASH "\\Device\\Flash"
HRESULT CreateMount(const char* szDrive, const char* szDevice, BOOL System);

#endif // !__Functions_H