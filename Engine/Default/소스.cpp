#include "..\..\Header\InputDev.h"


IMPLEMENT_SINGLETON(CInputDev)

Engine::CInputDev::CInputDev(void)
{
    ZeroMemory(m_byKeyData, sizeof(m_byKeyData));
}

Engine::CInputDev::~CInputDev(void)
{
    Free();
}


HRESULT Engine::CInputDev::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{

    // DInput 컴객체를 생성하는 함수
    FAILED_CHECK_RETURN(DirectInput8Create(hInst,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&m_pInputSDK,
        NULL), E_FAIL);

    // 키보드 객체 생성
    FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr), E_FAIL);

    // 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
    m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

    // 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
    m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

    // 장치에 대한 access 버전을 받아오는 함수
    m_pKeyBoard->Acquire();


    // 마우스 객체 생성
    FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr), E_FAIL);

    // 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
    m_pMouse->SetDataFormat(&c_dfDIMouse);

    // 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
    m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

    // 장치에 대한 access 버전을 받아오는 함수
    m_pMouse->Acquire();



    return S_OK;
}

void Engine::CInputDev::Update_InputDev(void)
{
    m_pKeyBoard->GetDeviceState(256, m_byKeyState);
    m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

bool CInputDev::Key_Pressing(_ubyte byKeyID)
{
    if (m_byKeyState[byKeyID] & 0x80)
        return true;

    return false;
}

bool CInputDev::Key_Down(_ubyte byKeyID)
{
    // 이전에는 눌린 적이 없고 현재 눌렸을 경우
    if (!m_byKeyData[byKeyID] && (m_byKeyState[byKeyID] & 0x80))
    {
        m_byKeyData[byKeyID] = !m_byKeyData[byKeyID]; //true
        return true;
    }

    return false;
}

bool CInputDev::Key_Up(_ubyte byKeyID)
{
    // 이전에는 눌린 적이 있고 현재 눌리지 않았을 경우
    if (m_byKeyData[byKeyID] && !(m_byKeyState[byKeyID] & 0x80))
    {
        m_byKeyData[byKeyID] = !m_byKeyData[byKeyID];
        return true;
    }

    return false;
}

bool CInputDev::Mouse_Pressing(MOUSEKEYSTATE eMouse)
{
    if (m_tMouseState.rgbButtons[eMouse] & 0x80)
        return true;

    return false;
}

bool CInputDev::Mouse_Down(MOUSEKEYSTATE eMouse)
{
    // 이전에는 눌린 적이 없고 현재 눌렸을 경우
    if (!m_tMouseData[eMouse] && (m_tMouseState.rgbButtons[eMouse] & 0x80))
    {
        m_tMouseData[eMouse] = !m_tMouseData[eMouse]; //true
        return true;
    }
    return false;
}

bool CInputDev::Mouse_Up(MOUSEKEYSTATE eMouse)
{
    // 이전에는 눌린 적이 있고 현재 눌리지 않았을 경우
    if (m_tMouseData[eMouse] && !(m_tMouseState.rgbButtons[eMouse] & 0x80))
    {
        m_tMouseData[eMouse] = !m_tMouseData[eMouse];
        return true;
    }

    return false;
}

void CInputDev::LateUpdate_InputDev(void)
{
    for (int i = 0; i < 256; ++i) //눌린적 있고 현재 눌리지 않은 것 false로?
    {
        if (m_byKeyData[i] && !(m_byKeyState[i] & 0x80))
            m_byKeyData[i] = !m_byKeyData[i];

        if (!m_byKeyData[i] && (m_byKeyState[i] & 0x80))
            m_byKeyData[i] = !m_byKeyData[i];
    }

    for (int i = 0; i < DIM_END; ++i) //눌린적 있고 현재 눌리지 않은 것 false로?
    {
        if (m_tMouseData[i] && !(m_tMouseState.rgbButtons[i] & 0x80))
            m_tMouseData[i] = !m_tMouseData[i];

        if (!m_tMouseData[i] && (m_tMouseState.rgbButtons[i] & 0x80))
            m_tMouseData[i] = !m_tMouseData[i];
    }
}

void Engine::CInputDev::Free(void)
{
    Safe_Release(m_pKeyBoard);
    Safe_Release(m_pMouse);
    Safe_Release(m_pInputSDK);
}
