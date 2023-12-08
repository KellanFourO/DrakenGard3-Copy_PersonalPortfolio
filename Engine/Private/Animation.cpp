#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const CModel::BONES& Bones)
{
	//!�̸��� ���θ� ������� �� ������?
	strcpy_s(m_szName, pAIAnimation->mName.data);
	
	m_fDuration = (_float)pAIAnimation->mDuration; //! ����Ϸ��� �ִϸ��̼��� �� ����

	//TODO ƽ���ۼ������ ����?
	//! ����ӵ��� �ǹ���. ���߿� ƽ���ۼ������ Ÿ�ӵ�Ÿ ���� m_fTrackPosition�� �����Ұž�.
	//! m_fTrackPosition�� �෹�̼� ���� Ŀ���� �ִϸ��̼��� ����Ǵ� �����̾�.
	//! m_fTrackPosition�� ���� ����ǰ� �ִ� ��ġ�� �ǹ���.
	   m_fTicksPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	//TODO ä���� ����?
	//! �ִϸ��̼��� ����ϴ� ���� ������ NumChannels��.
	//! �׷� ä���� ���ھ�? �ִϸ��̼��� ����ϴ� ����� ������.
	//! C_STRUCT aiNodeAnim** mChannels;
	m_iNumChannels = pAIAnimation->mNumChannels;

	//! �� �ִϸ��̼ǿ��� ����ϱ����� ��(AINodeAnim �Ǵ� Channel)�� ������ �����.
	
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel*	pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);

		if(nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
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
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Invalidate_TransformationMatrix(m_fTrackPosition, Bones);
	}
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const CModel::BONES& Bones)
{
	CAnimation* pInstance = new CAnimation();

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation::Free()
{
	for(auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
