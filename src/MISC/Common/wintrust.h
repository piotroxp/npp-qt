#pragma once
#include "WindowsCompat.h"

#define WTD_UI_FLAG 1
#define WTD_REVOKE_NONE 0
#define WTD_CHOICE_FILE 2
#define WTD_STATEACTION_VERIFY 0x10
#define WTD_CACHE_ONLY_URL_RETRIEVAL 0x1000

typedef struct WINTRUST_FILE_INFO_ {
	DWORD cbStruct;
	LPCWSTR pcwszFilePath;
	HANDLE hFile;
	GUID* pgKnownSubject;
} WINTRUST_FILE_INFO;

typedef struct WINTRUST_DATA_ {
	DWORD cbStruct;
	void* pPolicyCallbackData;
	void* pSIPClientData;
	DWORD dwUIChoice;
	DWORD fdwRevocationChecks;
	DWORD dwUnionChoice;
	WINTRUST_FILE_INFO* pFile;
	DWORD dwStateAction;
	HANDLE hWVTStateData;
	const wchar_t* pcwszURLReference;
	DWORD dwProvFlags;
	DWORD dwUIContext;
} WINTRUST_DATA;

inline LONG WinVerifyTrust(HWND, GUID*, WINTRUST_DATA*) { return 0; }
