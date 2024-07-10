#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(string strName, _float4x4 matTransformation, _float4x4 matOffset, _int iBoneIndex, _int iParentIndex, _uint iDepth)
{
	m_iIndex = iBoneIndex;
	m_iParentIndex = iParentIndex;
	m_iDepth = iDepth;

	strcpy_s(m_szName, strName.c_str()); //! �̸� ��������

	memcpy(&m_TransformationMatrix, &matTransformation, sizeof(_float4x4));
	memcpy(&m_OffsetMatrix, &matOffset, sizeof(_float4x4));
	//! AINode���� �о�� ��������� �츮�� ����ϴ� ��İ� ��� ���� ������ �������ֱ⿡ �ݵ�� ��ġ������Ѵ�.
	//! �Ʒ����� ��ġ������
	
	//XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity()); //! �Ĺ����� ���� �θ���� ������ ���⿡ �׵�.

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones, _fmatrix PivotMatrix, _float3& vRootOutPos)
{

	if (-1 == m_iParentIndex) //!  Unsigned�� �ƴ� Signed�� ����� ������ �θ������ ���� ����� ��쿡�� �ε����� -1�� ǥ���ϱ����ؼ�����. �װ� �̿��ؼ� �б�������
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PivotMatrix);
	
	else
	{
	//! �Ĺ��� ����� �� ������İ� �θ��� �Ĺ�������� ������� �Ѵٰ� �߾���. ��������.
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentIndex]->m_CombinedTransformationMatrix));
	}

	if (!strcmp(m_szName, "M_ROOT_$AssimpFbx$_Translation"))
	{
		
		vRootOutPos.x = m_CombinedTransformationMatrix._41;
		vRootOutPos.y = m_CombinedTransformationMatrix._42; 
		vRootOutPos.z = m_CombinedTransformationMatrix._43;

		m_CombinedTransformationMatrix._41 = 0;
		m_CombinedTransformationMatrix._42 = 0;
		m_CombinedTransformationMatrix._43 = 0;
	}
}

void CBone::BlendAnimation(const KEYFRAME& tCurKey, _float fRatio)
{
	_vector			vScale, vRotation, vPosition;

	vPosition = XMVectorLerp(XMLoadFloat3(&m_tPreKeyFrame.vPosition), XMLoadFloat3(&tCurKey.vPosition), fRatio);
	vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_tPreKeyFrame.vRotation), XMLoadFloat4(&tCurKey.vRotation), fRatio);
	vScale = XMVectorLerp(XMLoadFloat3(&m_tPreKeyFrame.vScale), XMLoadFloat3(&tCurKey.vScale), fRatio);

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Set_TransformationMatrix(TransformationMatrix);
}

void CBone::Setup_PreKeyFrame()
{

	/* Position */
	XMStoreFloat3(&m_tPreKeyFrame.vPosition, XMLoadFloat4x4(&m_TransformationMatrix).r[3]);

	/* Rotation */
	_matrix  tempRot = XMLoadFloat4x4(&m_TransformationMatrix);
	_matrix ResultMat;

	for (int i(0); i < 3; i++)
		ResultMat.r[i] = XMVector3Normalize(tempRot.r[i]);

	XMStoreFloat4(&m_tPreKeyFrame.vRotation, XMQuaternionRotationMatrix(ResultMat));

	/* Scale */
	tempRot = XMLoadFloat4x4(&m_TransformationMatrix);

	_vector ResultVec;
	ResultVec = XMVectorSet(XMVectorGetX(XMVector3Length(tempRot.r[0])), XMVectorGetX(XMVector3Length(tempRot.r[1])), XMVectorGetX(XMVector3Length(tempRot.r[2])), 0.f);

	XMStoreFloat3(&m_tPreKeyFrame.vScale, ResultVec);
}

void CBone::Reset_Bone()
{
	m_tPreKeyFrame = {};
}

CBone* CBone::Create(string strName, _float4x4 matTransformation, _float4x4 matOffset, _int iBoneIndex, _int iParentIndex, _uint iDepth)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(strName, matTransformation, matOffset, iBoneIndex, iParentIndex, iDepth)))
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

