#pragma once
#include "Model.h"

//TODO Ư�� �ִϸ��̼� ( ���, �ȱ�, �ٱ�, ������, �±�)�� ǥ���ϱ� ���� �����͵��� ������.

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

	//! �ִϸ��̼� ��������. ���� �� Ű�����Ӱ� ���� �ִϸ��̼��� Ű�������� �޾Ƽ� ���������� �ؾ��ϴµ� ���� �ִϸ��̼��� Ű�������� 0�̴� ���� ���� �� ����. �̶� �׷��� 0.2��� ���� �ִ� ���̴�.

private:
	_char					m_szName[MAX_PATH] = "";
	_float					m_fDuration = { 0.0f };			//! �ش� �ִϸ��̼��� ��ü ����
	_float					m_fAbsoulteDuration = { 0.f };
	_float					m_fTicksPerSecond = { 0.f };	//! �ִϸ��̼��� ����ӵ�
	_float					m_fTrackPosition = { 0.f };		//! ���� �ִϸ��̼��� ����ǰ� �ִ� ��ġ
	_uint					m_iNumChannels = { 0 };			//! �ش� �ִϸ��̼��� ����ϴ� ���� ����.

	//TODO �ִϸ��̼� ����
	_float4					m_RootMotionPos;
	_float4					m_vBeginRootBonePos = _float4(0.f, 0.f, 0.f, 1.f);
	_double					m_RootMotionValue = 1.0;

	//TODO ���� ����
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

