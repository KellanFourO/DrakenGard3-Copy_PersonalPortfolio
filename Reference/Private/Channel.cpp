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

		/* Ratio = 현재 애니메이션의 트랙위치와 현재 키프레임총길이 사이의 상대적인 위치비율 계산하기 */
		_float fRatio = (fCurrentTrackPosition - MyKeyFrames[m_iCurrentKeyFrameIndex].fTrackPosition)  /* 현재 애니메이션 재생위치 - 현재트랙의 시작점 = 현재 키프레임에서 얼마나 진행되었는가  */
			/ (MyKeyFrames[_int(m_iCurrentKeyFrameIndex + 1)].fTrackPosition - MyKeyFrames[m_iCurrentKeyFrameIndex].fTrackPosition);  /* 다음키프레임 시작 - 현재 키프레임 시작 = 다음키프레임까지의 길이  */

		_vector			vSourScale, vSourRotation, vSourPosition;
		_vector			vDestScale, vDestRotation, vDestPosition;

		vSourScale = XMLoadFloat3(&MyKeyFrames[m_iCurrentKeyFrameIndex].vScale);				/* 현재 키프레임의 scale*/
		vSourRotation = XMLoadFloat4(&MyKeyFrames[m_iCurrentKeyFrameIndex].vRotation);		/* 현재 키프레임의 rot*/
		vSourPosition = XMLoadFloat3(&MyKeyFrames[m_iCurrentKeyFrameIndex].vPosition);		/* 현재 키프레임의 pos */

		vDestScale = XMLoadFloat3(&MyKeyFrames[_int(m_iCurrentKeyFrameIndex + 1)].vScale);			/* 현재 +1 키프레임의 scale*/
		vDestRotation = XMLoadFloat4(&MyKeyFrames[_int(m_iCurrentKeyFrameIndex + 1)].vRotation);	/* 현재 +1 키프레임의 rot*/
		vDestPosition = XMLoadFloat3(&MyKeyFrames[_int(m_iCurrentKeyFrameIndex + 1)].vPosition);	/* 현재	+1 키프레임의 pos */

		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, fRatio);
		vPosition = XMVectorSetW(vPosition, 1.f);

	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	/* 키프레임에 맞는 행렬로 Bone에 적용시키기 */
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

		vSourScale = MyKeyFrames[iCurrentKeyFrame].vScale;				/* 현재 키프레임의 scale*/
		vSourRotation = MyKeyFrames[iCurrentKeyFrame].vRotation;			/* 현재 키프레임의 rot*/
		vSourPosition = MyKeyFrames[iCurrentKeyFrame].vPosition;			/* 현재 키프레임의 pos */

		vDestScale = MyKeyFrames[_int(iCurrentKeyFrame + 1)].vScale;			/* 현재 +1 키프레임의 scale*/
		vDestRotation = MyKeyFrames[_int(iCurrentKeyFrame + 1)].vRotation;		/* 현재 +1 키프레임의 rot*/
		vDestPosition = MyKeyFrames[_int(iCurrentKeyFrame + 1)].vPosition;		/* 현재	+1 키프레임의 pos */

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
	/* 애니메이션 선형보간전 초기화 작업 수행 */
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
