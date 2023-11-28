#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInput_Device : public CBase
{

private:
    CInput_Device();
    virtual ~CInput_Device() = default;

public:
    _byte   Get_DIKeyState(_ubyte byKeyID)
    {
        return m_byKeyState[byKeyID];
    }

    _byte   Get_DIMouseState(MOUSEKEYSTATE eMouse)
    {
        return m_tMouseState.rgbButtons[eMouse];
    }

    _long   Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
    {
        return *(((_long*)&m_tMouseState) + eMouseState);
    }
public:
    _bool   Key_Pressing(_ubyte byKeyID);
    _bool   Key_Down(_ubyte byKeyID);
    _bool   Key_Up(_ubyte byKeyID);

    _bool   Mouse_Pressing(MOUSEKEYSTATE eMouse);
    _bool   Mouse_Down(MOUSEKEYSTATE eMouse);
    _bool   Mouse_Up(MOUSEKEYSTATE eMouse);

public:
    HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);

    HRESULT Initialize(HINSTANCE hInst, HWND hWnd);

    void   Tick();

    void   Late_Tick();

private:
    IDirectInput8* m_pInput = nullptr;

private:
    IDirectInputDevice8* m_pKeyBoard = nullptr;
    IDirectInputDevice8* m_pMouse = nullptr;

private:
    _byte               m_byKeyState[256];      // 키보드에 있는 모든 키값을 저장하기 위한 변수
    DIMOUSESTATE2         m_tMouseState;

    _bool               m_byKeyData[256];
    _bool               m_tMouseData[DIM_END];

public:
    static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
    virtual void   Free();

};
END