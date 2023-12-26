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

	//! ä���� ���������·� �������ֱ⿡ ���۷��� ī���͸� �÷�����Ѵ�. ����Ʈ �����ڸ� ��������ʰ� �ִϸ��̼�Ŭ������ �� Ŭ������ �ٸ��� ���� ��������ڸ�  �� �����̱⵵ �ϴ�.
	for(auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}


_uint CAnimation::Get_CurrentChannelKeyIndex() const
{
	return (_uint)(m_fTrackPosition / (m_fDuration / m_iMaxKeyFrame));
}

HRESULT CAnimation::Initialize(const _float& fDuration, const _float& fTickPerSecond, vector<class CChannel*>& Channels, const string& strName)
{
	//!�̸��� ���θ� ������� �� ������?
	strcpy_s(m_szName, strName.c_str());
	m_fDuration = fDuration;// / 60.f;
	m_fTicksPerSecond = fTickPerSecond;

	//TODO ä���� ����?
	//! �ִϸ��̼��� ����ϴ� ���� ������ NumChannels��.
	//! �׷� ä���� ���ھ�? �ִϸ��̼��� ����ϴ� ����� ������.
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
	/* ����� �ִϸ��̼� �ʱ�ȭ */
	m_fAnimRatio = 0.f;
	m_fTrackPosition = 0.f;
	m_iPrevChannelIndex = -1;

	for (_uint i = 0; i < m_iNumChannels; ++i)
		m_Channels[i]->Reset_KeyFrame();
}



void CAnimation::Invalidate_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones)
{
	
	/* �ִϸ��̼� �������� */
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

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
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
