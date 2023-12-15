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
	_char			m_szName[MAX_PATH] = "";//! 뼈 이름
	_int			m_iParentIndex = { 0 }; //! 부모 뼈 인덱스
	_int			m_iIndex;
	_int			m_iDepth;

	//TODO TransformationMatrix #상태행렬
	//!이 뼈만의 상태행렬을 의미하고, 애니메이션이 돌아가기 전에 최초상태이자 언제든지 바뀔 수 있는 행렬이다.
	//! 시간에 따라서 어떠한 상태를 가지는 행렬이다.
	//! 추후 m_Animation에서 이 애니메이션은 어떠한 뼈에 영향을 주는지 얻어올거고, 그 영향을 주는 행렬을 TransformationMatrix에 갱신을 해줄건데, 
	//! m_TransformationMatrix는 부모행렬의 영향을 받기때문에 반드시 부모행렬의 상태행렬을 갱신이 끝난 이후에  CombindTransformationMatrix도 갱신해준다.

	_float4x4		m_TransformationMatrix; //! 이 뼈만의 상태행렬
	_float4x4		m_OffsetMatrix;
	_float4x4		m_CombinedTransformationMatrix; //! 이 뼈만의 상태행렬 * 부모의 컴바인 행렬

public:
	static CBone*	Create(string strName, _float4x4 matTransformation, _float4x4 matOffset, _int iBoneIndex, _int iParentIndex, _uint iDepth);
	CBone*			Clone();
	virtual void	Free() override;

};

END

