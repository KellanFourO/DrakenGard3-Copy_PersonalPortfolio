#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_fTicksPerSecond(rhs.m_fTicksPerSecond)
	, m_fTrackPosition(rhs.m_fTrackPosition)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_isFinished(rhs.m_isFinished)
	, m_CurrentKeyFrames(rhs.m_CurrentKeyFrames)
	, m_fAnimRatio(rhs.m_fAnimRatio)
	, m_fAnimationSpeed(rhs.m_fAnimationSpeed)
	, m_iMaxKeyFrame(rhs.m_iMaxKeyFrame)
	, m_iPrevChannelIndex(rhs.m_iPrevChannelIndex)
{
	strcpy_s(m_szName, rhs.m_szName);

	//! 채널은 포인터형태로 가지고있기에 레퍼런스 카운터를 올려줘야한다. 디폴트 생성자를 사용하지않고 애니메이션클래스는 본 클래스와 다르게 따로 복사생성자를  둔 이유이기도 하다.
	for(auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}


_uint CAnimation::Get_CurrentChannelKeyIndex() const
{
	return (_uint)(m_fTrackPosition / (m_fDuration / m_iMaxKeyFrame));
}

HRESULT CAnimation::Initialize(const _float& fDuration, const _float& fTickPerSecond, vector<class CChannel*>& Channels, const string& strName)
{
	//!이름을 빼두면 디버깅할 때 좋겠지?
	strcpy_s(m_szName, strName.c_str());
	m_fDuration = fDuration;// / 60.f;
	m_fTicksPerSecond = fTickPerSecond;

	//TODO 채널은 뭐야?
	//! 애니메이션이 사용하는 뼈의 갯수가 NumChannels야.
	//! 그럼 채널은 뭐겠어? 애니메이션이 사용하는 뼈라는 얘기겠지.
	//! C_STRUCT aiNodeAnim** mChannels;
	
	m_iNumChannels = Channels.size();
	m_CurrentKeyFrames.resize(m_iNumChannels);

	m_Channels.reserve(Channels.size());
	for (auto& iter : Channels)
	{
		if(m_iMaxKeyFrame < iter->Get_NumKeyFrames())
			m_iMaxKeyFrame = iter->Get_NumKeyFrames();

		m_Channels.push_back(iter);
	}

	
	return S_OK;
}

void CAnimation::Blend_TransformationMatrix(_float fMaxBlendTime, _float fRatio, const CModel::BONES& Bones)
{
	m_fAnimRatio = 0.f;

	for (_uint i = 0; i < m_iNumChannels; ++i)
		m_Channels[i]->Blend_TransformationMatrix(fMaxBlendTime, fRatio, Bones);
	
}

void CAnimation::Reset_Animation()
{
	/* 재생할 애니메이션 초기화 */
	m_fAnimRatio = 0.f;
	m_fTrackPosition = 0.f;
	m_iPrevChannelIndex = -1;

	for (_uint i = 0; i < m_iNumChannels; ++i)
		m_Channels[i]->Reset_KeyFrame();
}



void CAnimation::Invalidate_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones)
{
	
	/* 애니메이션 지속증가 */
	m_fTrackPosition += m_fTicksPerSecond * fTimeDelta * m_fAnimationSpeed;
	m_fAnimRatio = m_fTrackPosition / m_fDuration;

	if (m_fTrackPosition >= m_fDuration)
	{
		for (_uint i = 0; i < m_iNumChannels; ++i)
		{
			m_Channels[i]->Reset_KeyFrame();
		}

		if (true == isLoop)
		{
			m_isFinished = false;
			m_fTrackPosition = 0.f;
		}
		else if (false == isLoop)
		{
			m_isFinished = true;
			m_fTrackPosition = m_fDuration;
		}
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
		m_Channels[i]->Invalidate_TransformationMatrix(m_fTrackPosition, Bones);

	_int iCurrentKey = (_int)Get_CurrentChannelKeyIndex();

	if (iCurrentKey != m_iPrevChannelIndex)
	{
		for (_uint i = m_iPrevChannelIndex + 1; i <= (_uint)iCurrentKey; ++i)
			m_iPrevChannelIndex = iCurrentKey;
	}
}

CAnimation* CAnimation::Create(const _float& fDuration, const _float& fTickPerSecond, vector<class CChannel*>& Channels, const string& strName)
{
	CAnimation* pInstance = new CAnimation();

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(fDuration,fTickPerSecond,Channels,strName)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for(auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
