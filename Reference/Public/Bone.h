#pragma once

#include "Model.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const { return m_szName; }
	_int		 Get_Index() const { return m_iIndex; }

	_matrix Get_CombinedTransformationMatrix() const { return XMLoadFloat4x4(&m_CombinedTransformationMatrix); }
	_float4x4 Get_OffsetFloat4x4() const { return m_OffsetMatrix; }

	void	Set_TransformationMatrix(_fmatrix TransformationMatrix)
	{
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

public:
	HRESULT Initialize(string strName, _float4x4 matTransformation, _float4x4 matOffset, _int iBoneIndex, _int iParentIndex, _uint iDepth);
	void	Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones, _fmatrix PivotMatrix);

private:
	_char			m_szName[MAX_PATH] = "";//! �� �̸�
	_int			m_iParentIndex = { 0 }; //! �θ� �� �ε���
	_int			m_iIndex;
	_int			m_iDepth;

	//TODO TransformationMatrix #�������
	//!�� ������ ��������� �ǹ��ϰ�, �ִϸ��̼��� ���ư��� ���� ���ʻ������� �������� �ٲ� �� �ִ� ����̴�.
	//! �ð��� ���� ��� ���¸� ������ ����̴�.
	//! ���� m_Animation���� �� �ִϸ��̼��� ��� ���� ������ �ִ��� ���ðŰ�, �� ������ �ִ� ����� TransformationMatrix�� ������ ���ٰǵ�, 
	//! m_TransformationMatrix�� �θ������ ������ �ޱ⶧���� �ݵ�� �θ������ ��������� ������ ���� ���Ŀ�  CombindTransformationMatrix�� �������ش�.

	_float4x4		m_TransformationMatrix; //! �� ������ �������
	_float4x4		m_OffsetMatrix;
	_float4x4		m_CombinedTransformationMatrix; //! �� ������ ������� * �θ��� �Ĺ��� ���

public:
	static CBone*	Create(string strName, _float4x4 matTransformation, _float4x4 matOffset, _int iBoneIndex, _int iParentIndex, _uint iDepth);
	CBone*			Clone();
	virtual void	Free() override;

};

END

