#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(aiNode* pAINode, _int iParentIndex)
{
	m_iParentIndex = iParentIndex;

	strcpy_s(m_szName, pAINode->mName.data); //! 이름 가져오자

	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	//! AINode에서 읽어온 상태행렬은 우리가 사용하는 행렬과 행과 열의 순서가 반전되있기에 반드시 전치해줘야한다.
	//! 아래에서 전치해주자
	
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity()); //! 컴바인은 아직 부모행렬 정보가 없기에 항등.

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones, _fmatrix PivotMatrix)
{
	if (-1 == m_iParentIndex) //!  Unsigned가 아닌 Signed로 줬었던 이유가 부모행렬이 없는 노드의 경우에는 인덱스를 -1로 표현하기위해서였다. 그걸 이용해서 분기쳐주자
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PivotMatrix);
	
	else
	{
	//! 컴바인 행렬은 내 상태행렬과 부모의 컴바인행렬을 곱해줘야 한다고 했었다. 진행하자.
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentIndex]->m_CombinedTransformationMatrix));
	}

}

CBone* CBone::Create(aiNode* pAINode, _int iParentIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, iParentIndex)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBone::Free()
{
}
