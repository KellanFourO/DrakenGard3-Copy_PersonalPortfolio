#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(aiNode* pAINode, _int iParentIndex)
{
	m_iParentIndex = iParentIndex;

	strcpy_s(m_szName, pAINode->mName.data); //! �̸� ��������

	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	//! AINode���� �о�� ��������� �츮�� ����ϴ� ��İ� ��� ���� ������ �������ֱ⿡ �ݵ�� ��ġ������Ѵ�.
	//! �Ʒ����� ��ġ������
	
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity()); //! �Ĺ����� ���� �θ���� ������ ���⿡ �׵�.

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones, _fmatrix PivotMatrix)
{
	if (-1 == m_iParentIndex) //!  Unsigned�� �ƴ� Signed�� ����� ������ �θ������ ���� ����� ��쿡�� �ε����� -1�� ǥ���ϱ����ؼ�����. �װ� �̿��ؼ� �б�������
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PivotMatrix);
	
	else
	{
	//! �Ĺ��� ����� �� ������İ� �θ��� �Ĺ�������� ������� �Ѵٰ� �߾���. ��������.
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
