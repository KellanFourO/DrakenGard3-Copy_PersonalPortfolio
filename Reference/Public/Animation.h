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
	_bool	Get_Finished() { return m_isFinished; }

public:
	HRESULT Initialize(const _float& fDuration, const _float& fTickPerSecond, vector<class CChannel*>& Channels, const string& strName);
	void	Invalidate_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones);

private:
	_char					m_szName[MAX_PATH] = "";
	_float					m_fDuration = { 0.0f }; //! �ش� �ִϸ��̼��� ��ü ����
	_float					m_fTicksPerSecond = { 0.f }; //! �ִϸ��̼��� ����ӵ�
	_float					m_fTrackPosition = { 0.f }; //! ���� �ִϸ��̼��� ����ǰ� �ִ� ��ġ

	_uint					m_iNumChannels = { 0 }; //! �ش� �ִϸ��̼��� ����ϴ� ���� ����.
	vector<class CChannel*> m_Channels;
	vector<_uint>			m_CurrentKeyFrames;
	_bool					m_isFinished = { false };

public:
	static CAnimation* Create(const _float& fDuration, const _float& fTickPerSecond, vector<class CChannel*>& Channels, const string& strName);
	CAnimation*	 Clone();
	virtual void Free() override;

};

END

