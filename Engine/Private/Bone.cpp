#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(string strName, _float4x4 matTransformation, _float4x4 matOffset, _int iBoneIndex, _int iParentIndex, _uint iDepth)
{
	m_iIndex = iBoneIndex;
	m_iParentIndex = iParentIndex;
	m_iDepth = iDepth;

	strcpy_s(m_szName, strName.c_str()); //! 이름 가져오자

	memcpy(&m_TransformationMatrix, &matTransformation, sizeof(_float4x4));
	memcpy(&m_OffsetMatrix, &matOffset, sizeof(_float4x4));
	//! AINode에서 읽어온 상태행렬은 우리가 사용하는 행렬과 행과 열의 순서가 반전되있기에 반드시 전치해줘야한다.
	//! 아래에서 전치해주자
	
	//XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity()); //! 컴바인은 아직 부모행렬 정보가 없기에 항등.

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones, _fmatrix PivotMatrix, _float3& vRootOutPos)
{

	if (-1 == m_iParentIndex) //!  Unsigned가 아닌 Signed로 줬었던 이유가 부모행렬이 없는 노드의 경우에는 인덱스를 -1로 표현하기위해서였다. 그걸 이용해서 분기쳐주자
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PivotMatrix);
	
	else
	{
	//! 컴바인 행렬은 내 상태행렬과 부모의 컴바인행렬을 곱해줘야 한다고 했었다. 진행하자.
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

