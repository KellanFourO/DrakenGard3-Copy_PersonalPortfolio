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
	//TODO m_Importer 객체가 가지고있는 ReadFile 함수를 호출한다
	//! ReadFile 함수는 모델의 정보를 담고있는 AiScene*을 리턴해준다.
	//! 추후 설명 : aiProcess_PreTransformVertices | aiProcess_GlobalScale
	
	//! aiProcess_ConvertToLeftHanded == 왼손좌표계를 사용할께
	//! aiProcessPreset_TargetRealtime_Fast == 퀄리티는 조금 떨어지더라도 가장 빠른 방식을 선택해서 읽어들일게
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
	m_iNumMeshes = m_pAIScene->mNumMeshes; //! 읽어들인 메쉬개수로 멤버변수를 채워주자

	m_Meshes.reserve(m_iNumMeshes); //! 메쉬 개수를 알게됐으니 벡터를 예약해주자

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

	/* 원형객체를 초기화한다.  */
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
