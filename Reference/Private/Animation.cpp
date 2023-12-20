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



CChannel* CAnimation::Get_SameName_Channel(CChannel* _pChannel)
{
	for (auto& pChannel : m_Channels)
	{
		if(_pChannel->Get_Name() == pChannel->Get_Name())
			return pChannel;
	}
	
	return nullptr;
}

KEYFRAME& CAnimation::Get_CurrentKeyFrame(CChannel* _pChannel)
{

	_int iSize = _pChannel->Get_KeyFrames().size();
	
	KEYFRAME returnKeyFrame;

	vector<KEYFRAME> vecKeyFrames = _pChannel->Get_KeyFrames();


	// �ش� ä���� ���۰� �ص带 �˾Ƴ���
	for (size_t i = 0; i < iSize; ++i)
	{
		if (vecKeyFrames[i].fTrackPosition >= m_fTrackPosition)
		{
			returnKeyFrame = vecKeyFrames[i];
			returnKeyFrame.bTest = true;
			return returnKeyFrame;
		}
	}
	
	// TODO: ���⿡ return ���� �����մϴ�.
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

void CAnimation::Blend_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones, CAnimation* pPrevAnimation, CModel* pModel)
{
	//TODO ���� �ִϸ��̼��� �������� Ű������ ��ġ�� �޾����� 

	m_fTrackPosition += 1 * fTimeDelta;
		 
		 for (size_t i = 0; i < m_iNumChannels; i++)
		 {
			 CChannel* pChannel = nullptr;
			 pChannel = pPrevAnimation->Get_SameName_Channel(m_Channels[i]); //! ���� �ִϸ��̼��� ä�ΰ� ���� �ִϸ��̼��� ä���� ���� �̸��� ���� ä���� ã���ش�. 
			 
			 KEYFRAME PrevKeyFrame = pPrevAnimation->Get_CurrentKeyFrame(pChannel);

			 if (nullptr != pChannel)
			 {
				 //! ���� �ִϸ��̼��� ����ǰ��ִ� ��ġ�� Ű������
				 //! ä�ο��� ���� �ִϸ��̼��� ä�ε��� ���� �̸��� ���� ä���� ����ǰ��ִ� ��ġ�� Ű�������� �Ѱ����
				 if (m_Channels[i]->Blend_TransformationMatrix(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i], PrevKeyFrame))
				 {
					 pPrevAnimation->Reset_Animation(); //! ���� �ִϸ��̼��� Ʈ�������� 0���� �����ش�.
					 //m_fTrackPosition = 0.f;
					 pModel->Finished_ChangeAnim();
				 }
			 }
		 
		}
	
}

void CAnimation::Reset_Animation()
{
	m_fTrackPosition = 0.f;

	for(auto& iter : m_CurrentKeyFrames)
		iter = 0.f;
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
