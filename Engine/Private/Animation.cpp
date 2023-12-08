#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const CModel::BONES& Bones)
{
	//!이름을 빼두면 디버깅할 때 좋겠지?
	strcpy_s(m_szName, pAIAnimation->mName.data);
	
	m_fDuration = (_float)pAIAnimation->mDuration; //! 재생하려는 애니메이션의 총 길이

	//TODO 틱스퍼세컨드는 뭐야?
	//! 재생속도를 의미해. 나중에 틱스퍼세컨드랑 타임델타 곱을 m_fTrackPosition에 누적할거야.
	//! m_fTrackPosition이 듀레이션 보다 커지면 애니메이션이 종료되는 개념이야.
	//! m_fTrackPosition은 현재 재생되고 있는 위치를 의미해.
	   m_fTicksPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	//TODO 채널은 뭐야?
	//! 애니메이션이 사용하는 뼈의 갯수가 NumChannels야.
	//! 그럼 채널은 뭐겠어? 애니메이션이 사용하는 뼈라는 얘기겠지.
	//! C_STRUCT aiNodeAnim** mChannels;
	m_iNumChannels = pAIAnimation->mNumChannels;

	//! 이 애니메이션에서 사용하기위한 뼈(AINodeAnim 또는 Channel)의 정보를 만든다.
	
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


	//! 트랙포지션이 듀레이션(애니메이션의 총 길이)보다 커졌다는 것은 애니메이션이 끝났다는 것과 같아.
	//!  인자값으로 들어온 isLoop의 값에 땨따라 루프를 돌릴지 멈출지 처리할거야.
	if(m_fTrackPosition >= m_fDuration)
	{
		m_isFinished = true; //! 끝났다는 것을 알리기위한 불변수야.
		m_fTrackPosition = m_fDuration; //! isLoop가 False였다면 루프를 돌리지 않겠다는 거지. 총 길이의 마지막으로 유지시켜주자.

		if (true == isLoop)
		{
			m_fTrackPosition = 0.0f;
			m_isFinished = false;
		}
	}

	//! 근데! 애니메이션의 프레임마다 속도가 재생속도가 달라야 하는 경우가 있어 .  틱스퍼세컨드를 말하는거겠지?
	//! 예를 들자면, 바위를 드는게 0 프레임, 던지는게 1프레임이라고 쳤을 때. 드는 속도는 느리더라도 던지는건 빠른 경우도 있다는 거지.
	//! 이렇게 속도가 달리 정의되면 애니메이션의 퀄리티를 높일 수 있어.
	//! 그리고 트랙포지션을 듀레이션에 맞춰놓고 마이너스로 감산하면 역재생도 가능하겠지?
	//! 역재생을 이용하면 뒤로 걷기 같은것도 할 수 있어.

	//! 위에서 현재 애니메이션이 재생되고있는 위치가 누적되면서 갱신되니까.
	//! 내 애니메이션이 이용하는 전체 뼈의 상태를 갱신된 위치에 맞는 상태로 바꿔주자
	
	//! 애니메이션이 사용하는 전체뼈의 상태를 갱신하자며? 사용하는 전체 뼈 갯수 루프 돌아야지
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Invalidate_TransformationMatrix(m_fTrackPosition, Bones);
	}
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const CModel::BONES& Bones)
{
	CAnimation* pInstance = new CAnimation();

	/* 원형객체를 초기화한다.  */
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
