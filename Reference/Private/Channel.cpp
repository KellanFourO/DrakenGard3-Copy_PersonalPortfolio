#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const string strName, vector<KEYFRAME>& Keyframes, _int iBoneIndex)
{
	strcpy_s(m_szName, strName.c_str());

	m_KeyFrames.reserve(Keyframes.size());

	for (auto& iter : Keyframes)
		m_KeyFrames.push_back(iter);

	m_iNumKeyFrames = m_KeyFrames.size();

	m_iBoneIndex = iBoneIndex;
	return S_OK;

}

void CChannel::Invalidate_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones)
{
	_vector			vScale, vRotation, vPosition;
	vector<KEYFRAME> MyKeyFrames = m_KeyFrames;

	if (fCurrentTrackPosition >= MyKeyFrames.back().fTrackPosition)
	{
		vScale = XMLoadFloat3(&MyKeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&MyKeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&MyKeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		m_iCurrentKeyFrameIndex = 0;

		while (fCurrentTrackPosition >= m_KeyFrames[_int(m_iCurrentKeyFrameIndex + 1)].fTrackPosition)
			++m_iCurrentKeyFrameIndex;

		/* Ratio = ���� �ִϸ��̼��� Ʈ����ġ�� ���� Ű�������ѱ��� ������ ������� ��ġ���� ����ϱ� */
		_float fRatio = (fCurrentTrackPosition - MyKeyFrames[m_iCurrentKeyFrameIndex].fTrackPosition)  /* ���� �ִϸ��̼� �����ġ - ����Ʈ���� ������ = ���� Ű�����ӿ��� �󸶳� ����Ǿ��°�  */
			/ (MyKeyFrames[_int(m_iCurrentKeyFrameIndex + 1)].fTrackPosition - MyKeyFrames[m_iCurrentKeyFrameIndex].fTrackPosition);  /* ����Ű������ ���� - ���� Ű������ ���� = ����Ű�����ӱ����� ����  */

		_vector			vSourScale, vSourRotation, vSourPosition;
		_vector			vDestScale, vDestRotation, vDestPosition;

		vSourScale = XMLoadFloat3(&MyKeyFrames[m_iCurrentKeyFrameIndex].vScale);				/* ���� Ű�������� scale*/
		vSourRotation = XMLoadFloat4(&MyKeyFrames[m_iCurrentKeyFrameIndex].vRotation);		/* ���� Ű�������� rot*/
		vSourPosition = XMLoadFloat3(&MyKeyFrames[m_iCurrentKeyFrameIndex].vPosition);		/* ���� Ű�������� pos */

		vDestScale = XMLoadFloat3(&MyKeyFrames[_int(m_iCurrentKeyFrameIndex + 1)].vScale);			/* ���� +1 Ű�������� scale*/
		vDestRotation = XMLoadFloat4(&MyKeyFrames[_int(m_iCurrentKeyFrameIndex + 1)].vRotation);	/* ���� +1 Ű�������� rot*/
		vDestPosition = XMLoadFloat3(&MyKeyFrames[_int(m_iCurrentKeyFrameIndex + 1)].vPosition);	/* ����	+1 Ű�������� pos */

		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);
		vPosition = XMVectorSetW(vPosition, 1.f);

	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	/* Ű�����ӿ� �´� ��ķ� Bone�� �����Ű�� */
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Blend_TransformationMatrix(_float fBlendTime, _float In_fRatio, const CModel::BONES& Bones)
{
	KEYFRAME Key;
	ZeroMemory(&Key, sizeof(KEYFRAME));

	_vector				vScale, vRotation, vPosition;
	vector<KEYFRAME>	MyKeyFrames = m_KeyFrames;
	_uint				iCurrentKeyFrame = 0;

	if (fBlendTime >= MyKeyFrames.back().fTrackPosition)
	{
		vScale = XMLoadFloat3(&MyKeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&MyKeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&MyKeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}

	else
	{
		while (fBlendTime > MyKeyFrames[_int(iCurrentKeyFrame + 1)].fTrackPosition)
		{
			++iCurrentKeyFrame;
		}

		_float fRatio = (fBlendTime - MyKeyFrames[iCurrentKeyFrame].fTrackPosition)
			/ (MyKeyFrames[_int(iCurrentKeyFrame + 1)].fTrackPosition - MyKeyFrames[iCurrentKeyFrame].fTrackPosition);

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = MyKeyFrames[iCurrentKeyFrame].vScale;				/* ���� Ű�������� scale*/
		vSourRotation = MyKeyFrames[iCurrentKeyFrame].vRotation;			/* ���� Ű�������� rot*/
		vSourPosition = MyKeyFrames[iCurrentKeyFrame].vPosition;			/* ���� Ű�������� pos */

		vDestScale = MyKeyFrames[_int(iCurrentKeyFrame + 1)].vScale;			/* ���� +1 Ű�������� scale*/
		vDestRotation = MyKeyFrames[_int(iCurrentKeyFrame + 1)].vRotation;		/* ���� +1 Ű�������� rot*/
		vDestPosition = MyKeyFrames[_int(iCurrentKeyFrame + 1)].vPosition;		/* ����	+1 Ű�������� pos */

		vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio);

	}

	XMStoreFloat3(&Key.vScale, vScale);
	XMStoreFloat4(&Key.vRotation, vRotation);
	XMStoreFloat3(&Key.vPosition, vPosition);

	Bones[m_iBoneIndex]->BlendAnimation(Key, In_fRatio);
}

void CChannel::Reset_KeyFrame()
{
	/* �ִϸ��̼� ���������� �ʱ�ȭ �۾� ���� */
	m_iCurrentKeyFrameIndex = 0;
}

CChannel* CChannel::Create(const string strName, vector<KEYFRAME>& Keyframes, _int iBoneIndex)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(strName, Keyframes, iBoneIndex)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
}
