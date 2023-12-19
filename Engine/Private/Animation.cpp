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
{
	strcpy_s(m_szName, rhs.m_szName);

	//! ä���� ���������·� �������ֱ⿡ ���۷��� ī���͸� �÷�����Ѵ�. ����Ʈ �����ڸ� ��������ʰ� �ִϸ��̼�Ŭ������ �� Ŭ������ �ٸ��� ���� ��������ڸ�  �� �����̱⵵ �ϴ�.
	for(auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

KEYFRAME& CAnimation::Get_CurrentKeyFrame(class CChannel* _pChannel)
{
	// TODO ���� ���� ���� �ִϸ��̼��̴ٶ�� �����غ���. ���� ���� ����ǰ��ִ� ��ġ�� �´� Ű�������� �Ѱ�����Ѵ�
	//! ä���� ��ȸ�غ���.
	if(nullptr == _pChannel)
		return KEYFRAME();

	
	for (auto& KeyFrame : _pChannel->Get_KeyFrames())
	{
		if (KeyFrame.fTrackPosition == m_fTrackPosition)
		{
			KeyFrame.bTest = true;
			return KeyFrame;
		}
	}
	
}

CChannel* CAnimation::Get_SameName_Channel(CChannel* _pChannel)
{
	for (auto& pChannel : m_Channels)
	{
		if(_pChannel->Get_Name() == pChannel->Get_Name())
			return pChannel;
	}
	
	return nullptr;
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
		m_Channels.push_back(iter);

	return S_OK;
}

void CAnimation::Blend_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones, CAnimation* pPrevAnimation, CModel* pModel, const float& fRatio)
{
	//TODO ���� �ִϸ��̼��� �������� Ű������ ��ġ�� �޾����� 
	
	m_fTrackPosition += m_fTicksPerSecond * fTimeDelta;
	
	if (m_fTrackPosition >= m_fDuration)
	{
		m_isFinished = true; //! �����ٴ� ���� �˸������� �Һ�����.
		m_fTrackPosition = m_fDuration; //! isLoop�� False���ٸ� ������ ������ �ʰڴٴ� ����. �� ������ ���������� ������������.

		if (true == isLoop)
		{
			m_fTrackPosition = 0.0f;
			m_isFinished = false;
		}

	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{

		if (nullptr != pPrevAnimation)
		{
			
			CChannel* pChannel = nullptr;
			pChannel = pPrevAnimation->Get_SameName_Channel(m_Channels[i]); //! ���� �ִϸ��̼��� ä�ΰ� ���� �ִϸ��̼��� ä���� ���� �̸��� ���� ä���� ã���ش�.

			_bool Test = pPrevAnimation->Get_CurrentKeyFrame(pChannel).bTest;

			//! ���� Ű�������� �������� 
			if (Test && nullptr != pChannel)
			{
				if(m_Channels[i]->Blend_TransformationMatrix(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i], pPrevAnimation->Get_CurrentKeyFrame(pChannel), fRatio))
				{
					pModel->Finished_ChangeAnim();
					pPrevAnimation->Get_CurrentKeyFrame(pChannel).fTrackPosition = 0.f;
				}
			}
		}
	}

	
}



void CAnimation::Invalidate_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones)
{
	
	m_fTrackPosition += m_fTicksPerSecond * fTimeDelta;


	//! Ʈ���������� �෹�̼�(�ִϸ��̼��� �� ����)���� Ŀ���ٴ� ���� �ִϸ��̼��� �����ٴ� �Ͱ� ����.
	//!  ���ڰ����� ���� isLoop�� ���� �x���� ������ ������ ������ ó���Ұž�.
	if(m_fTrackPosition >= m_fDuration)
	{
		m_isFinished = true; //! �����ٴ� ���� �˸������� �Һ�����.
		m_fTrackPosition = m_fDuration; //! isLoop�� False���ٸ� ������ ������ �ʰڴٴ� ����. �� ������ ���������� ������������.

		if (true == isLoop)
		{
			m_fTrackPosition = 0.0f;
			m_isFinished = false;
		}
		
	}

	//! �ٵ�! �ִϸ��̼��� �����Ӹ��� �ӵ��� ����ӵ��� �޶�� �ϴ� ��찡 �־� .  ƽ���ۼ����带 ���ϴ°Ű���?
	//! ���� ���ڸ�, ������ ��°� 0 ������, �����°� 1�������̶�� ���� ��. ��� �ӵ��� �������� �����°� ���� ��쵵 �ִٴ� ����.
	//! �̷��� �ӵ��� �޸� ���ǵǸ� �ִϸ��̼��� ����Ƽ�� ���� �� �־�.
	//! �׸��� Ʈ���������� �෹�̼ǿ� ������� ���̳ʽ��� �����ϸ� ������� �����ϰ���?
	//! ������� �̿��ϸ� �ڷ� �ȱ� �����͵� �� �� �־�.

	//! ������ ���� �ִϸ��̼��� ����ǰ��ִ� ��ġ�� �����Ǹ鼭 ���ŵǴϱ�.
	//! �� �ִϸ��̼��� �̿��ϴ� ��ü ���� ���¸� ���ŵ� ��ġ�� �´� ���·� �ٲ�����
	
	//! �ִϸ��̼��� ����ϴ� ��ü���� ���¸� �������ڸ�? ����ϴ� ��ü �� ���� ���� ���ƾ���
	//! 
	//! 
	

	for (size_t i = 0; i < m_iNumChannels; i++)
	{	
		m_Channels[i]->Invalidate_TransformationMatrix(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i]);
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
