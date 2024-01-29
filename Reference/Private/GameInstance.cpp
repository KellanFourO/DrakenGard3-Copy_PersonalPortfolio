#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Data_Manager.h"
#include "Font_Manager.h"
#include "Renderer.h"
#include "Collision_Manager.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "Event_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, HINSTANCE hInstance, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	//TODO 그래픽 디바이스 초기화
	m_pGraphic_Device = CGraphic_Device::Create(GraphicDesc, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	//TODO 타이머 셋팅
	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	//TODO 인풋디바이스 셋팅
	m_pInput_Device = CInput_Device::Create(hInstance, GraphicDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	//TODO 레벨 셋팅
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;
	
	//TODO 오브젝트 매니저
	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	//TODO 컴포넌트 매니저
	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pData_Manager = CData_Manager::Create();
	if(nullptr == m_pData_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	//TODO 렌더러
	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	//TODO 파이프라인
	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	//TODO 폰트매니저
	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;
		
	//TODO 콜리전매니저
	m_pCollision_Manager = CCollision_Manager::Create();
	if(nullptr == m_pCollision_Manager)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pEvent_Manager = CEvent_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if(nullptr == m_pLevel_Manager ||
	   nullptr == m_pObject_Manager ||
	   nullptr == m_pPipeLine ||
	   nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Tick();
	
	m_pObject_Manager->Priority_Tick(fTimeDelta);
	
	m_pObject_Manager->Tick(fTimeDelta);

	//TODO 파이프라인 틱 호출순서에 관하여
	//! 카메라는 오브젝트 매니저에 들어갈것이다. 카메라의 Tick함수에서 카메라의 뷰, 투영 행렬을 생성해주기 때문에
	//! 카메라의 뷰, 투영행렬을 가지고있을 파이프라인의 Tick함수는 카메라의 Tick함수 이후에 해주어야 한다.
	m_pPipeLine->Tick();
	//! 객체들의 Late_Tick에서 파이프라인에게 접근해서 작업할 수 있기에 Tick과 Late_Tick 사이에 넣은 것이다.

	m_pObject_Manager->Late_Tick(fTimeDelta);

	m_pCollision_Manager->Update_CollisionMgr(m_pLevel_Manager->Get_CurrentLevelIndex(), fTimeDelta);

	m_pLevel_Manager->Tick(fTimeDelta);
	
	m_pEvent_Manager->Tick(fTimeDelta);

}



HRESULT CGameInstance::Render_Engine()
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pRenderer)
		return E_FAIL;

	m_pRenderer->Draw_RenderGroup();

#ifdef _DEBUG
	m_pLevel_Manager->Render();
	m_pEvent_Manager->Render_Events();
#endif

	m_pInput_Device->LateTick();

	return S_OK;
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if(nullptr == m_pObject_Manager
	 || nullptr == m_pComponent_Manager)
		return;

	//! 오브젝트 매니져에 레벨별로 구분 해 놓은 객체들 중 특정된 객체들을 지운다.
	m_pObject_Manager->Clear(iLevelIndex);

	//! 컴포넌트 매니져에 레벨별로 구분 해 놓은 컴포넌트들 중 특정된 객체들을 지운다.
	m_pComponent_Manager->Clear(iLevelIndex);
}

IDXGISwapChain* CGameInstance::Get_SwapChain()
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;
	
	return m_pGraphic_Device->Get_SwapChain();
}

ID3D11RenderTargetView* CGameInstance::Get_BackBufferRTV() const
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_BackBufferRTV();
}

ID3D11DepthStencilView* CGameInstance::Get_DSV() const
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_DSV();
}

GRAPHIC_DESC CGameInstance::Get_GraphicDesc()
{
	if (nullptr == m_pGraphic_Device)
		return GRAPHIC_DESC();

	return m_pGraphic_Device->Get_GraphicDesc();
}


HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Resize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Resize(hWnd, message, wParam, lParam);
}

HRESULT CGameInstance::UseFullScreen(_bool bMode)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->UseFullScreen(bMode);
}

HRESULT CGameInstance::Add_Timer(const wstring& strTimeTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimeTag);
}

_float CGameInstance::Compute_TimeDelta(const wstring& strTimeTag)
{
	if(nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimeTag);
}

HRESULT CGameInstance::Open_Level(_uint iCurrentLevelIndex, CLevel* pNewLevel)
{
	if(nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iCurrentLevelIndex, pNewLevel);
}

_uint CGameInstance::Get_CurrentLevelIndex()
{
	return m_pLevel_Manager->Get_CurrentLevelIndex();
}

HRESULT CGameInstance::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype, _bool bAddData, _bool bModelType)
{
	if(nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag,pPrototype, bAddData, bModelType);
}

HRESULT CGameInstance::Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg, CGameObject** ppOut)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_CloneObject(iLevelIndex,strLayerTag,strPrototypeTag,pArg, ppOut);
}

CGameObject* CGameInstance::Get_CloneObject(const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_CloneObject(strPrototypeTag, pArg);
}

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

CComponent* CGameInstance::Get_PartComponent(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, const wstring& strPartTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_PartComponent(iLevelIndex, strLayerTag, strComponentTag, iIndex, strPartTag);
}

CGameObject* CGameInstance::Get_Player(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Player(iLevelIndex);
}

HRESULT CGameInstance::Erase_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pEraseObject)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Erase_CloneObject(iLevelIndex, strLayerTag, pEraseObject);
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}



HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype, _bool bModelCom)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex,strPrototypeTag,pPrototype, bModelCom);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag,pArg);
}

HRESULT CGameInstance::Add_PrototypeTag(const wstring& strProtoTypeTag, _bool bModelType)
{
	if (nullptr == m_pData_Manager)
		return E_FAIL;
	
	return m_pData_Manager->Add_PrototypeTag(strProtoTypeTag, bModelType);
}

map<const wstring, _bool>& CGameInstance::Get_ObjectTags()
{
	if (nullptr == m_pData_Manager)
		return map<const wstring, _bool>();

	return m_pData_Manager->Get_ObjectTags();
}

vector<wstring>& CGameInstance::Get_VecTags()
{
	if (nullptr == m_pData_Manager)
		return vector<wstring>();

	return m_pData_Manager->Get_VecTags();
}

HRESULT CGameInstance::Add_LayerTag(const wstring& strLayerTag)
{
	if (nullptr == m_pData_Manager)
		return E_FAIL;

	return m_pData_Manager->Add_LayerTag(strLayerTag);
}

vector<wstring>& CGameInstance::Get_LayerTags()
{
	if (nullptr == m_pData_Manager)
		return vector<wstring>();

	return m_pData_Manager->Get_LayerTags();
}

HRESULT CGameInstance::Add_ModelTag(const wstring& strModelTag)
{
	if (nullptr == m_pData_Manager)
		return E_FAIL;

	return m_pData_Manager->Add_ModelTag(strModelTag);
}

vector<wstring>& CGameInstance::Get_ModelTags()
{
	if (nullptr == m_pData_Manager)
		return vector<wstring>();

	return m_pData_Manager->Get_ModelTags();
}

HRESULT CGameInstance::Add_EffectTexutreTag(const wstring& strTextureTag)
{
	if (nullptr == m_pData_Manager)
		return E_FAIL;

	return m_pData_Manager->Add_EffectTexutreTag(strTextureTag);
}

HRESULT CGameInstance::Add_EffectMeshTag(const wstring& strMeshModelTag)
{
	if (nullptr == m_pData_Manager)
		return E_FAIL;

	return m_pData_Manager->Add_EffectMeshTag(strMeshModelTag);
}

vector<wstring>& CGameInstance::Get_EffectTextureTags()
{
	if (nullptr == m_pData_Manager)
		return vector<wstring>();

	return m_pData_Manager->Get_EffectTextureTags();
}

vector<wstring>& CGameInstance::Get_EffectMeshTags()
{
	if (nullptr == m_pData_Manager)
		return vector<wstring>();

	return m_pData_Manager->Get_EffectMeshTags();
}

HRESULT CGameInstance::Add_ModelData(const wstring& strModelDataTag, MODELDATA* ModelData)
{
	if (nullptr == m_pData_Manager)
		return E_FAIL;

	return m_pData_Manager->Add_ModelData(strModelDataTag, ModelData);
}

MODELDATA* CGameInstance::Get_ModelData_InKey(const wstring& strModelDataTag)
{

	return m_pData_Manager->Get_ModelData_InKey(strModelDataTag);
}

HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eGroupID, CGameObject* pGameObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderGroup(eGroupID, pGameObject);
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState,TransformMatrix);
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _float4x4 TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::D3DTRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::D3DTRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eState);
}

_matrix CGameInstance::Get_TransformMatrixInverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrixInverse(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4Inverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4Inverse(eState);
}

RAY CGameInstance::Get_Ray(_uint& In_ViewPortWidth, const _uint& In_ViewPortHeight)
{
	if (nullptr == m_pPipeLine)
		return RAY();

	return m_pPipeLine->Get_Ray(In_ViewPortWidth, In_ViewPortHeight);
	
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();
}


_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return _byte();

	return m_pInput_Device->Get_DIKeyState(byKeyID);


}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device)
		return _byte();

	return m_pInput_Device->Get_DIMouseState(eMouse);

}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	if (nullptr == m_pInput_Device)
		return _long();

	return m_pInput_Device->Get_DIMouseMove(eMouseState);

}

_bool CGameInstance::Key_Pressing(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Pressing(byKeyID);
}

_bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Down(byKeyID);
}

_bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Up(byKeyID);
}

_bool CGameInstance::Mouse_Pressing(MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Pressing(eMouse);
}

_bool CGameInstance::Mouse_Down(MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Down(eMouse);
}

_bool CGameInstance::Mouse_Up(MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Up(eMouse);
}

void CGameInstance::Mouse_Fix()
{
	if (nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Mouse_Fix();
}

HRESULT CGameInstance::Add_Font(const wstring& strFontTag, const wstring& strFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fScale, _float2 vOrigin, _float fRotation)
{
	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, fScale, vOrigin, fRotation);
}

HRESULT CGameInstance::Add_Check_CollisionGroup(const _tchar* LeftLayerTag, const _tchar* RightLayerTag)
{
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	return m_pCollision_Manager->Add_Check_CollisionGroup(LeftLayerTag, RightLayerTag);
}

void CGameInstance::Update_CollisionMgr(_uint iLevelIndex, _float fTimeDelta)
{
	if (nullptr == m_pCollision_Manager)
		return;

	return m_pCollision_Manager->Update_CollisionMgr(iLevelIndex, fTimeDelta);
}

void CGameInstance::Reset_CollisionGroup()
{
	if (nullptr == m_pCollision_Manager)
		return;

	m_pCollision_Manager->Reset_CollisionGroup();
}

HRESULT CGameInstance::Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const wstring& strMRTTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Begin_MRT(strMRTTag);
}

HRESULT CGameInstance::End_MRT()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RenderTarget_ShaderResource(const wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Bind_ShaderResource(strTargetTag, pShader, pConstantName);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RenderTarget_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_Debug_RTVs(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}
#endif 

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(LightDesc);
}
HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

HRESULT CGameInstance::Add_Event(const wstring& strAddEventTag, class CMyEvent* pEvent,  void* pDesc)
{
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;

	return m_pEvent_Manager->Add_Event(strAddEventTag, pEvent, pDesc);
}

HRESULT CGameInstance::Erase_Event(const wstring& strEraseEventTag)
{
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;

	return m_pEvent_Manager->Erase_Event(strEraseEventTag);
}


void CGameInstance::Release_Manager()
{
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pData_Manager);
	Safe_Release(m_pRenderer);
	

	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->Release_Manager();
	CGameInstance::GetInstance()->DestroyInstance();
}

