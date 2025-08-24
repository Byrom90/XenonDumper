#ifndef __XuiClasses_H__
#define __XuiClasses_H__
#pragma once

class CMyApp : public CXuiModule
{
protected:
	// Override so that CMyApp can register classes.
	virtual HRESULT RegisterXuiClasses();

	// Override so that CMyApp can unregister classes. 
	virtual HRESULT UnregisterXuiClasses();
};


//==============================================================================================================================================================================
//
//			CLASS TO HANDLE THE Main Scene
//
//==============================================================================================================================================================================
#pragma region MainScene
class CMainScene : public CXuiSceneImpl
{
	//private:
public:
	XUI_IMPLEMENT_CLASS(CMainScene, L"MainScene", XUI_CLASS_SCENE)
protected:
	CXuiTextElement textPrompt;
	CXuiControl btnExitApp, btnAccept;
	CXuiImageElement ImgBackground;
	
	// Handler for the XM_INIT message.
	HRESULT OnInit(XUIMessageInit* pInitData, BOOL& bHandled);
	HRESULT OnNotifyPress(HXUIOBJ hObjPressed, BOOL& bHandled);
	//HRESULT OnTimer(XUIMessageTimer* pTimer, BOOL& bHandled);
	HRESULT SetTextColour(HXUIOBJ TextObject, DWORD dwTextColour);

	VOID SetFailedText(wstring ErrorText);

	// Message map.
	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT(OnInit)
		XUI_ON_XM_NOTIFY_PRESS(OnNotifyPress)
		//XUI_ON_XM_TIMER(OnTimer)
		XUI_END_MSG_MAP()

};
#pragma endregion

#endif