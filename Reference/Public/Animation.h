#pragma once
#include "Base.h"

//TODO Ư�� �ִϸ��̼� ( ���, �ȱ�, �ٱ�, ������, �±�)�� ǥ���ϱ� ���� �����͵��� ������.

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation);
	void	Invalidate_TransformationMatrix(_float fTimeDelta);

private:
	_char			m_szName[MAX_PATH] = "";
	_float			m_fDuration = { 0.0f }; //! �ش� �ִϸ��̼��� ��ü ����
	_float			m_fTicksPerSecond = { 0.f }; //! �ִϸ��̼��� ����ӵ�
	_float			m_fTrackPosition = { 0.f }; //! ���� �ִϸ��̼��� ����ǰ� �ִ� ��ġ

	_uint			m_iNumChannels = { 0 }; //! �ش� �ִϸ��̼��� ����ϴ� ���� ����.
	vector<class CChannel*> m_Channels;

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation);
	virtual void Free() override;

};

END

