#include "Model.h"
#include "Mesh.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice,pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
{
}

HRESULT CModel::Initialize_Prototype(const string& strModelFilePath)
{
	//TODO m_Importer ��ü�� �������ִ� ReadFile �Լ��� ȣ���Ѵ�
	//! ReadFile �Լ��� ���� ������ ����ִ� AiScene*�� �������ش�.
	//! ���� ���� : aiProcess_PreTransformVertices | aiProcess_GlobalScale
	
	//! aiProcess_ConvertToLeftHanded == �޼���ǥ�踦 ����Ҳ�
	//! aiProcessPreset_TargetRealtime_Fast == ����Ƽ�� ���� ���������� ���� ���� ����� �����ؼ� �о���ϰ�
	m_pAIScene = m_Importer.ReadFile(strModelFilePath, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

	if(nullptr == m_pAIScene)
		return E_FAIL;

	if(FAILED(Ready_Meshes()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes; //! �о���� �޽������� ��������� ä������

	m_Meshes.reserve(m_iNumMeshes); //! �޽� ������ �˰Ե����� ���͸� ����������

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh*	pMesh = CMesh::Create(m_pDevice,m_pContext,m_pAIScene->mMeshes[i]);

		if(nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}
	
	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strModelFilePath)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	__super::Free();
}
