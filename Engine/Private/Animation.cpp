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

	//! 채널은 포인터형태로 가지고있기에 레퍼런스 카운터를 올려줘야한다. 디폴트 생성자를 사용하지않고 애니메이션클래스는 본 클래스와 다르게 따로 복사생성자를  둔 이유이기도 하다.
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


	// 해당 채널의 시작과 앤드를 알아내라
	for (size_t i = 0; i < iSize; ++i)
	{
		if (vecKeyFrames[i].fTrackPosition >= m_fTrackPosition)
		{
			returnKeyFrame = vecKeyFrames[i];
			returnKeyFrame.bTest = true;
			return returnKeyFrame;
		}
	}
	
	// TODO: 여기에 return 문을 삽입합니다.
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
		m_Channels.push_back(iter);

	return S_OK;
}

void CAnimation::Blend_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones, CAnimation* pPrevAnimation, CModel* pModel)
{
	//TODO 이전 애니메이션이 진행중인 키프레임 위치를 받아주자 

	m_fTrackPosition += 1 * fTimeDelta;
		 
		 for (size_t i = 0; i < m_iNumChannels; i++)
		 {
			 CChannel* pChannel = nullptr;
			 pChannel = pPrevAnimation->Get_SameName_Channel(m_Channels[i]); //! 현재 애니메이션의 채널과 이전 애니메이션의 채널중 같은 이름을 가진 채널을 찾아준다. 
			 
			 KEYFRAME PrevKeyFrame = pPrevAnimation->Get_CurrentKeyFrame(pChannel);

			 if (nullptr != pChannel)
			 {
				 //! 이전 애니메이션이 재생되고있는 위치의 키프레임
				 //! 채널에게 이전 애니메이션의 채널들중 같은 이름을 가진 채널의 재생되고있는 위치에 키프레임을 넘겨줬다
				 if (m_Channels[i]->Blend_TransformationMatrix(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i], PrevKeyFrame))
				 {
					 pPrevAnimation->Reset_Animation(); //! 이전 애니메이션의 트랙포지션 0으로 돌려준다.
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
