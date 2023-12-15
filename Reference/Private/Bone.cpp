#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(string strName, _float4x4 matTransformation, _int iBoneIndex, _int iParentIndex, _uint iDepth)
{
	m_iIndex = iBoneIndex;
	m_iParentIndex = iParentIndex;
	m_iDepth = iDepth;

	strcpy_s(m_szName, strName.c_str()); //! �̸� ��������

	memcpy(&m_TransformationMatrix, &matTransformation, sizeof(_float4x4));
	
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

CBone* CBone::Create(string strName, _float4x4 matTransformation, _int iBoneIndex, _int iParentIndex, _uint iDepth)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(strName, matTransformation, iBoneIndex, iParentIndex, iDepth)))
	{
		MSG_BOX("Failed To Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
}
