#include "stdafx.h"

#pragma region APP
// Register custom classes.
HRESULT CMyApp::RegisterXuiClasses()
{
	CMainScene::Register();
	return S_OK;
}

// Unregister custom classes.
HRESULT CMyApp::UnregisterXuiClasses()
{
	CMainScene::Unregister();
	return S_OK;
}
#pragma endregion


#define DUMP_DIR		"GAME:\\console"
#define PATH_1BL		DUMP_DIR "\\1bl.bin"
#define PATH_FUSES		DUMP_DIR "\\fuses.txt"

#define NAND_DIR		"GAME:\\nand"
#define PATH_NAND_AUTO	NAND_DIR "\\simpleflasher.cmd"
#define PATH_NAND_XEX	NAND_DIR "\\Simple360NandFlasher.xex"
#define RES_NAND_XEX	"S360NF"

//==============================================================================================================================================================================
//
//			CLASS TO HANDLE THE MainScene
//
//==============================================================================================================================================================================
#pragma region Overlay
extern BOOL bShouldExit;

BOOL KillControls = FALSE;

// Default yellow colour used for text 0xFFEBEB0F
DWORD YellowTextCol = 0xFFEBEB0F;
// Red for error 0xFFFF0000
DWORD RedTextCol = 0xFFFF0000;
HRESULT CMainScene::SetTextColour(HXUIOBJ TextObject, DWORD dwTextColour)
{
	HRESULT ret = S_FALSE;
	DWORD dwId; XUIElementPropVal pVal;
	ret = XuiObjectGetPropertyId(TextObject, L"TextColor", &dwId);
	if (ret == S_OK) {
		pVal.SetColorVal(dwTextColour);
		ret = XuiObjectSetProperty(TextObject, dwId, 0, &pVal);
	}
	return ret;
}

BOOL CreateFusesTxt()
{
	FILE* fd;
	fopen_s(&fd, PATH_FUSES, "w");
	if (fd != NULL)
	{
		QWORD FuseArray[12];
		for (int i = 0; i < 12; i++)
		{
			FuseArray[i] = Hvx::HvGetFuseline(i);
			DbgPrint("fuseset %02d: %016llx", i, FuseArray[i]);
			fprintf(fd, "fuseset %02d: %016llx\n", i, FuseArray[i]); 
		}
		// CPUKey
		DbgPrint("Your CPU key : %016llX%016llX", FuseArray[3], FuseArray[5]); // fuse lines 3 & 5
		fprintf(fd, "\nYour CPU key : %016llX%016llX\n", FuseArray[3], FuseArray[5]);
		// DVDKey
		BYTE DVDKeyBytes[16];
		QWORD kvAddress = Hvx::HvPeekQWORD(IsDevkit ? 0x00000002000162E0 : 0x00000002000163C0);
		Hvx::HvPeekBytes(kvAddress + 0x100, DVDKeyBytes, 16);
		DbgPrint("Your DVD key : %016llX%016llX", *(QWORD*)(DVDKeyBytes), *(QWORD*)(DVDKeyBytes + 8));
		fprintf(fd, "Your DVD key : %016llX%016llX", *(QWORD*)(DVDKeyBytes), *(QWORD*)(DVDKeyBytes + 8));

		fclose(fd);
		return TRUE;
	}
	return FALSE;
}

BOOL Save1blToFile()
{
	BYTE RomBuf[MEM_1BL_SZ];
	ZeroMemory(RomBuf, MEM_1BL_SZ);
	HRESULT res = Hvx::HvDump1blRom(RomBuf);
	if (!res)
	{
		if (CWriteFile(PATH_1BL, &RomBuf, sizeof(RomBuf)) == TRUE)
		{
			return TRUE;
		}
		DbgPrint("[Save1blToFile] Failed to write file!");
	}
	else
	{
		DbgPrint("[Save1blToFile] HV exp call failed! %08X", res);
	}
	return FALSE;
}

BOOL CreateNandDumpAutofile()
{
	FILE* fd;
	fopen_s(&fd, PATH_NAND_AUTO, "w");
	if (fd != NULL)
	{
		fprintf(fd, "dump");
		fclose(fd);
		return TRUE;
	}
	return FALSE;
}

BOOL CreateNandDumpXex()
{
	VOID* pSectionData;
	DWORD dwSectionSize;

	HMODULE hModule = GetModuleHandle(NULL);

	if (XGetModuleSection(hModule, RES_NAND_XEX, &pSectionData, &dwSectionSize))
	{
		if (CWriteFile(PATH_NAND_XEX, pSectionData, dwSectionSize) == TRUE)
		{
			return TRUE;
		}
	}

	return FALSE;
}

VOID CMainScene::SetFailedText(wstring ErrorText) // Sets failed text as well as disable the A button to prevent further input. User must exit using the back button
{
	btnAccept.SetEnable(FALSE);
	btnAccept.SetShow(FALSE);
	SetTextColour(textPrompt, RedTextCol);
	textPrompt.SetText(ErrorText.c_str());
	
}

HRESULT CMainScene::OnInit(XUIMessageInit* pInitData,
	BOOL& bHandled)
{
	GetChildById(L"textPrompt", &textPrompt);
	GetChildById(L"btnAccept", &btnAccept);
	GetChildById(L"btnExitApp", &btnExitApp);
	GetChildById(L"ImgBackground", &ImgBackground);
	
	if ((IsDevkit && !(XboxKrnlVersion->Build >= 17489)) || (!IsDevkit && XboxKrnlVersion->Build != 17559))
	{
		SetFailedText(L"Unsupported kernel version!\nThis app only supports versions 17559 Retail & 17489 (or 17559 spoofed) Developer.\n- Press the Back button to exit -");
	}
	else
	{
		if (!Hvx::CheckPPExpHVAccess()) // Check if we have pp access. We can't do anything without it
		{
			if (!Hvx::SetupPPExpHVAccess()) // If not we try to setup the expansion. If we fail here we're done
			{
				SetFailedText(L"Unable to obtain HV access via expansion!\nHV access is required to proceed.\n- Press the Back button to exit -");
			}
		}
	}

	return S_OK;
}



BOOL bStage1Complete = FALSE;
HRESULT CMainScene::OnNotifyPress(HXUIOBJ hObjPressed, BOOL& bHandled)
{
	
	if (KillControls) {
		bHandled = TRUE;
		return S_OK;
	}

	if (hObjPressed == btnExitApp)
	{
		bShouldExit = TRUE;
	}
	else if (hObjPressed == btnAccept)
	{
		KillControls = TRUE;
		

		if (!bStage1Complete) // Dump the fuses and 1bl
		{
			if (!DirectoryExists(DUMP_DIR)) CreateDirectory(DUMP_DIR, NULL);

			if (!CreateFusesTxt()) SetFailedText(L"Failed to dump fuses!\n- Press the Back button to exit -");
			else
			{
				if (!Save1blToFile()) SetFailedText(L"Failed to dump 1bl.bin!\n- Press the Back button to exit -");
				else
				{
					bStage1Complete = TRUE;
					textPrompt.SetText(L"fuses.txt & 1bl.bin have now been dumped!\nNand dumping will be handled by Simple360NandFlasher.\nThis is automated and should not require any buttons to be pressed.\n- Press the A button to continue -");
					btnAccept.SetText(L"Continue");
				}
			}
		}
		else // Setup and launch simple 360 in automode for dumping
		{
			if (!DirectoryExists(NAND_DIR)) CreateDirectory(NAND_DIR, NULL);

			if (CreateNandDumpAutofile() && CreateNandDumpXex()) XLaunchNewImage(PATH_NAND_XEX, 0);
			else SetFailedText(L"Failed to automate the launch of Simple360NandFlasher!\nNOTE: You can still dump your nand manually.\n- Press the Back button to exit -");
		}
		KillControls = FALSE;
	}

	bHandled = TRUE;
	return S_OK;
}


/*
HRESULT CMainScene::OnTimer(XUIMessageTimer* pTimer, BOOL& bHandled)
{
	// which timer is it?
	switch (pTimer->nId)
	{
	
	default:
		break;
	}

	bHandled = TRUE;

	return(S_OK);
}
*/