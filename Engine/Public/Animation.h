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
	_bool						Get_Finished() { return m_isFinished; }
	_float						Get_TrackPostion() { return m_fTrackPosition; }
	vector<class CChannel*>&	Get_Channels() { return m_Channels;}
	vector<_uint>&				Get_KeyFrames() { return m_CurrentKeyFrames; }
	class CChannel*				Get_SameName_Channel(class CChannel* _pChannel);

public:
	HRESULT Initialize(const _float& fDuration, const _float& fTickPerSecond, vector<class CChannel*>& Channels, const string& strName);
	void	Invalidate_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones);
	void	Blend_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones, CAnimation* pPrevAnimation, CModel* pModel);
	void	Reset_Animation();
	//! �ִϸ��̼� ��������. ���� �� Ű�����Ӱ� ���� �ִϸ��̼��� Ű�������� �޾Ƽ� ���������� �ؾ��ϴµ� ���� �ִϸ��̼��� Ű�������� 0�̴� ���� ���� �� ����. �̶� �׷��� 0.2��� ���� �ִ� ���̴�.

private:
	_char					m_szName[MAX_PATH] = "";
	_float					m_fDuration = { 0.0f }; //! �ش� �ִϸ��̼��� ��ü ����
	_float					m_fTicksPerSecond = { 0.f }; //! �ִϸ��̼��� ����ӵ�
	_float					m_fTrackPosition = { 0.f }; //! ���� �ִϸ��̼��� ����ǰ� �ִ� ��ġ
	_float					m_fInterpolation = { 0.2f };

	_uint					m_iNumChannels = { 0 }; //! �ش� �ִϸ��̼��� ����ϴ� ���� ����.
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

