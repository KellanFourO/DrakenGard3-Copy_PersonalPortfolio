#pragma once
#include "Model.h"

//TODO 특정 애니메이션 ( 대기, 걷기, 뛰기, 때리기, 맞기)을 표현하기 위한 데이터들을 가진다.

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	_char*						Get_Name() { return m_szName; }
	_uint						Get_CurrentChannelKeyIndex() const;
	_float						Get_TrackPosition() { return m_fTrackPosition; }
	_float						Get_Duration(){ return m_fDuration;}
	_uint						Get_iNumChannels() { return m_iNumChannels; }
	vector<_uint>&				Get_CurrentKeyFrames() { return m_CurrentKeyFrames; }
	vector<class CChannel*>&	Get_Channels() { return m_Channels;}

	void						Set_Duration(_float fDuration) { m_fDuration = fDuration; }
	void						Set_AbsoluteDuration(_float fAbsolute) { m_fAbsoulteDuration = fAbsolute; }
	
	void						Set_StartAnimationKey(_uint iKeyIndex) { m_fTrackPosition = (m_fDuration / m_iMaxKeyFrame) * iKeyIndex; }
	void						Set_TrackPosition(_float fTrackposition) { m_fTrackPosition = fTrackposition; }
	
	_bool						Get_Finished() { return m_isFinished; }

	_float						Get_AnimRatio() { return m_fAnimRatio; }
	void						Set_AnimRatio(_float fRatio) { m_fAnimRatio = fRatio; }


public:
	HRESULT Initialize(const _float& fDuration, const _float& fTickPerSecond, vector<class CChannel*>& Channels, const string& strName);

public:
	_bool	Invalidate_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones, _float fAnimSpeed = 5.f);
	void	Blend_TransformationMatrix(_float fMaxBlendTime, _float fRatio, const CModel::BONES& Bones);
	void	Reset_Animation();

	//! 애니메이션 선형보간. 현재 내 키프레임과 다음 애니메이션의 키프레임을 받아서 선형보간을 해야하는데 다음 애니메이션의 키프레임은 0이니 차를 구할 수 없다. 이때 그래서 0.2라는 값을 주는 것이다.

private:
	_char					m_szName[MAX_PATH] = "";
	_float					m_fDuration = { 0.0f };			//! 해당 애니메이션의 전체 길이
	_float					m_fAbsoulteDuration = { 0.f };
	_float					m_fTicksPerSecond = { 0.f };	//! 애니메이션의 재생속도
	_float					m_fTrackPosition = { 0.f };		//! 현재 애니메이션이 재생되고 있는 위치
	_uint					m_iNumChannels = { 0 };			//! 해당 애니메이션이 사용하는 뼈의 갯수.

	//TODO 애니메이션 변수
	_float4					m_RootMotionPos;
	_float4					m_vBeginRootBonePos = _float4(0.f, 0.f, 0.f, 1.f);
	_double					m_RootMotionValue = 1.0;

	//TODO 블랜딩 변수
	_int					m_iPrevChannelIndex = { -1 };
	_float					m_fAnimRatio = { 0.f };
	_uint					m_iMaxKeyFrame = { 0 };

	vector<class CChannel*> m_Channels;
	vector<_uint>			m_CurrentKeyFrames;
	_bool					m_isFinished = { false };
	_bool					m_bTest = { false };
	

public:
	static CAnimation* Create(const _float& fDuration, const _float& fTickPerSecond, vector<class CChannel*>& Channels, const string& strName);
	CAnimation*	 Clone();
	virtual void Free() override;

};

END

