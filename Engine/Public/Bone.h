#pragma once

#include "Model.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	HRESULT Initialize(aiNode* pAINode, _int iParentIndex);
	void	Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones);

private:
	_char			m_szName[MAX_PATH] = "";//! �� �̸�
	_int			m_iParentIndex = { 0 }; //! �θ� �� �ε���

	//TODO TransformationMatrix #�������
	//!�� ������ ��������� �ǹ��ϰ�, �ִϸ��̼��� ���ư��� ���� ���ʻ������� �������� �ٲ� �� �ִ� ����̴�.
	//! �ð��� ���� ��� ���¸� ������ ����̴�.
	//! ���� m_Animation���� �� �ִϸ��̼��� ��� ���� ������ �ִ��� ���ðŰ�, �� ������ �ִ� ����� TransformationMatrix�� ������ ���ٰǵ�, 
	//! m_TransformationMatrix�� �θ������ ������ �ޱ⶧���� �ݵ�� �θ������ ��������� ������ ���� ���Ŀ�  CombindTransformationMatrix�� �������ش�.

	_float4x4		m_TransformationMatrix; //! �� ������ �������
	
	_float4x4		m_CombindTransformationMatrix; //! �� ������ ������� * �θ��� �Ĺ��� ���

public:
	static CBone*	Create(aiNode* pAINode, _int iParentIndex);
	virtual void	Free() override;

};

END

