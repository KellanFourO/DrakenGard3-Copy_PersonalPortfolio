#pragma once
#include "Model.h"

//TODO Ư�� �ִϸ��̼� ( ���, �ȱ�, �ٱ�, ������, �±�)�� ǥ���ϱ� ���� �����͵��� ������.

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	_bool	Get_Finished() { return m_isFinished; }

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const CModel::BONES& Bones);
	void	Invalidate_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones);

private:
	_char					m_szName[MAX_PATH] = "";
	_float					m_fDuration = { 0.0f }; //! �ش� �ִϸ��̼��� ��ü ����
	_float					m_fTicksPerSecond = { 0.f }; //! �ִϸ��̼��� ����ӵ�
	_float					m_fTrackPosition = { 0.f }; //! ���� �ִϸ��̼��� ����ǰ� �ִ� ��ġ

	_uint					m_iNumChannels = { 0 }; //! �ش� �ִϸ��̼��� ����ϴ� ���� ����.
	vector<class CChannel*> m_Channels;
	_bool					m_isFinished = { false };

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const CModel::BONES& Bones);
	virtual void Free() override;

};

END

