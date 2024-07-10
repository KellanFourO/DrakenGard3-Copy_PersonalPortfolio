#include "stdafx.h"
#include "Dynamic_Terrain.h"
#include "GameInstance.h"



CDynamic_Terrain::CDynamic_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{

}

CDynamic_Terrain::CDynamic_Terrain(const CDynamic_Terrain& rhs)
	: CGameObject(rhs)
	, m_eCurrentLevelID(rhs.m_eCurrentLevelID)
{
}

HRESULT CDynamic_Terrain::Initialize_Prototype(LEVEL eLevel)
{
	//TODO ������ü�� �ʱ�ȭ������ �����Ѵ�.
	/* 1.�����κ��� ���� �޾ƿͼ� �ʱ�ȭ�Ѵ� .*/
	/* 2.����������� ���� ���� �޾ƿ´�.*/

	m_eCurrentLevelID = eLevel;

	return S_OK;
}

HRESULT CDynamic_Terrain::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	DINFO* pInfo = (DINFO*)pArg;

	// ĳ���õ� ����ü�� �� ���
	DINFO Info = { pInfo->fX, pInfo->fY, pInfo->fZ };

	Info.vecVertexInfo = pInfo->vecVertexInfo;
	
	// �ּҰ� �ƴ�, ���� ��� ����ü �Ѱ��ֱ�
	ReceiveInfo(Info);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CDynamic_Terrain::Priority_Tick(_float fTimeDelta)
{
}

void CDynamic_Terrain::Tick(_float fTimeDelta)
{
	//m_pShaderCom->Bind_RawValue("g_vBrushPos", &m_fPickingPos, sizeof(_float4));
}

void CDynamic_Terrain::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CDynamic_Terrain::Render()
{
	//TODO ���̴����� ����� �����ִ� ������ �ݵ�� ���̴��� ����Լ��� ȣ���ϱ� ������ �ؾ��Ѵ�.
	//! �� ������, ���̴��� ����Լ� ������ pPass->Apply(0, m_pContext); �ڵ带 �����Ѵ�.
	//! Apply ȣ�� �Ŀ� ����� �����൵ ������ ���� ������, �������� ��������.
	//! Apply ȣ�� �Ŀ� ����� �����ָ�, � ������ ���������� ����ǰ�, � ������ ���� ����� �� �Ѿ�� ��찡 �ִ�.
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//! �� ���̴��� 0��° �н��� �׸��ž�.
	m_pShaderCom->Begin(0);

	if (m_pVIBufferCom != nullptr) // ���� �����Ҷ� �����ϰ� ��������� ������ Ÿ�� �����⶧����, ����ó�� �ص���.
	{
		//! ���� �׸����� �ϴ� ����, �ε��� ���۸� ��ġ�� ���ε���
		m_pVIBufferCom->Bind_VIBuffers();

		//! ���ε��� ����, �ε����� �׷�
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

void CDynamic_Terrain::Write_Json(json& Out_Json, const wstring& strFileName)
{
	__super::Write_Json(Out_Json);

	CreateHeightMap();

	wstring strFilePath = TEXT("../Bin/DataFiles/Map/");
	wstring strEXT = TEXT(".dat");
	
	wstring strFullPath = strFilePath + strFileName + strEXT;


	HANDLE	hFile = CreateFile(strFullPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if(0 == hFile)
		return;
	_ulong	dwByte;	

	vector<VTXDYNAMIC>* vecInfo = m_pVIBufferCom->Get_Infos();

	for (auto& tInfo : *vecInfo)
	{
		VTXDYNAMIC tOut;
		memcpy(&tOut, &tInfo, sizeof(VTXDYNAMIC));
		WriteFile(hFile, &tOut, sizeof(VTXDYNAMIC), &dwByte, nullptr);
	}


	CloseHandle(hFile);

	//! ���� ������ �����Ҳ���
	//! ���� ���I���� ����������Ʈ�� �������־�.
	//! ���̳ʸ�ȭ�� ��ų �ž�
	//! JSON���� �Ϲ������������ ���ؼ� �Ұž�.
	
	Out_Json.emplace("SizeX", m_tDynamicInfo.fX);
	Out_Json.emplace("SizeY", m_tDynamicInfo.fY);
	Out_Json.emplace("SizeZ", m_tDynamicInfo.fZ);
}

void CDynamic_Terrain::Load_FromJson(const json& In_Json, const wstring& strFileName)
{
	__super::Load_FromJson(In_Json);

	wstring strFilePath = TEXT("../Bin/DataFiles/Map/");
	wstring strEXT = TEXT(".dat");

	wstring strFullPath = strFilePath + strFileName + strEXT;

	m_tDynamicInfo.fX = In_Json["SizeX"];
	m_tDynamicInfo.fY = In_Json["SizeY"];
	m_tDynamicInfo.fZ = In_Json["SizeZ"];
	
	HANDLE hFile = CreateFile(strFullPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
		return;

	_ulong dwByte = { 0 };

	while (true)
	{
		VTXDYNAMIC tInfo;

		ReadFile(hFile, &tInfo, sizeof(VTXDYNAMIC), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		m_tDynamicInfo.vecVertexInfo.push_back(tInfo);
	}

	CloseHandle(hFile);

	Delete_Component(TEXT("Com_VIBuffer"));

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Dynamic_Terrain"), &m_tDynamicInfo)))
		return;

	//__super::Add_Component(LEVEL_GAMEPLAY,In_Json["Texture"], TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr);
	
	_int i = 0;

}


void CDynamic_Terrain::CreateHeightMap()
{
	
}

void CDynamic_Terrain::Picking_Terrain(EDIT_MODE eMode)
{
	if (nullptr == m_pVIBufferCom)
	{
		MSG_BOX("Picking_Terrain Buffer is nullptr");
		return;
	}
	
	_uint WinCX, WinCY;
	WinCX = g_iWinSizeX; WinCY = g_iWinSizeY;

	RAY WorldRay = m_pGameInstance->Get_Ray(WinCX, WinCY);


	if (m_pVIBufferCom->Compute_MousePos(WorldRay,m_pTransformCom->Get_WorldMatrix(), &m_fPickingPos))
	{
		m_pVIBufferCom->Update(XMLoadFloat3(&m_fPickingPos), m_fDrawRadious, m_fPower, (_uint)eMode);
	}
	
}

_float3 CDynamic_Terrain::GetMousePos()
{
	if (nullptr == m_pVIBufferCom)
	{
		MSG_BOX("Picking_Terrain Buffer is nullptr");
		return _float3();
	}
	_uint WinCX, WinCY;
	WinCX = g_iWinSizeX; WinCY = g_iWinSizeY;

	RAY WorldRay = m_pGameInstance->Get_Ray(WinCX, WinCY);


	if (m_pVIBufferCom->Compute_MousePos(WorldRay, m_pTransformCom->Get_WorldMatrix(), &m_fPickingPos))
	{
		return m_fPickingPos;
	}

	return _float3();
}

_bool CDynamic_Terrain::MouseOnTerrain()
{
	if (nullptr == m_pVIBufferCom)
	{
		MSG_BOX("Picking_Terrain Buffer is nullptr");
		return false;
	}

	_uint WinCX, WinCY;
	WinCX = g_iWinSizeX; WinCY = g_iWinSizeY;

	RAY WorldRay = m_pGameInstance->Get_Ray(WinCX, WinCY);

	if (m_pVIBufferCom->Compute_MousePos(WorldRay, m_pTransformCom->Get_WorldMatrix(), &m_fPickingPos))
	{
		return true;
	}

	return false;

	
}

HRESULT CDynamic_Terrain::Ready_Components()
{
	//! For.Com_Shader
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//! For.Com_VIBuffer_Dynamic
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_VIBuffer_Dynamic_Terrain"),	// �������� ����
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &m_tDynamicInfo)))
		return E_FAIL;

	//! For.Com_Texture
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Texture_Terrain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Mask */ // ������ �������� Mask // ���� : �� ������ ID�� �����ʾƼ� �ͷ��� ������ �����߾���, ����� ������ �ȵǴ�, Imgui shutdown�� �� ������
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_MASK]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Texture_Terrain_Brush"),
		TEXT("Com_Brush"), reinterpret_cast<CComponent**>(&m_pTextureCom[TYPE_BRUSH]))))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CDynamic_Terrain::Bind_ShaderResources()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	// ��ǻ��
	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResourceArray(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	// ����ũ
	if (FAILED(m_pTextureCom[TYPE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	//// �귯��
	if (FAILED(m_pTextureCom[TYPE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0))) // error : 
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBrushPos", &m_fPickingPos, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrushRange", &m_fDrawRadious, sizeof(float))))
		return E_FAIL;
	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))	// ����
	//	return E_FAIL;

	return S_OK;
}

void CDynamic_Terrain::ReceiveInfo(DINFO pInfo)
{
	m_tDynamicInfo = { pInfo.fX, pInfo.fY, pInfo.fZ };
	m_tDynamicInfo.vecVertexInfo = pInfo.vecVertexInfo;
}

void CDynamic_Terrain::Delete_Component(const wstring& strComTag)
{
	__super::Delete_Component(strComTag);

	// ! ������ �ȵɶ� !
	// Delete_Component������ ����� �༮�� Safe_Release�� ���۷��� ī��Ʈ�� ���̱⸸ �Ѱ��̶�,
	// ����� Ÿ���� Ȯ���ϰ� ���⼭ ��������� ���� ��������Ѵ�.

	if(m_pVIBufferCom)
	Safe_Release(m_pVIBufferCom);
	

}

CDynamic_Terrain* CDynamic_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CDynamic_Terrain* pInstance = new CDynamic_Terrain(pDevice, pContext);

	/* ���� ��ü�� �ʱ�ȭ�Ѵ�. */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CDynamic_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDynamic_Terrain::Clone(void* pArg) // ���⼭ �纻�� ���鶧 ������ ������ ���� �޾��ش�.
{
	CDynamic_Terrain* pInstance = new CDynamic_Terrain(*this);

	/* �纻 ��ü�� �ʱ�ȭ�Ѵ�. */
	if (FAILED(pInstance->Initialize(pArg))) // �Ѱ���
	{
		MSG_BOX("Failed to Cloned : CDynamic_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDynamic_Terrain::Free()
{
	__super::Free();

	
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pShaderCom);

	//Safe_Release(m_pTextureCom);
	// �迭 ����, ->Release ���� : �ؽ�ó ������Ʈ�� �迭 �������� �ٲ�� ������, �ϳ��ϳ� ���� �����ֵ��� �ٲ�����Ѵ�.
	for (size_t i = 0; i < TYPE_END; i++)
	{
		Safe_Release(m_pTextureCom[i]);
	}
}
