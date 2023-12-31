#pragma once

#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const { return m_szName; }
	_int		 Get_Index() const { return m_iIndex; }

	_matrix		Get_TransformationMatrix() const { return XMLoadFloat4x4(&m_TransformationMatrix); }
	_matrix		Get_CombinedTransformationMatrix() const { return XMLoadFloat4x4(&m_CombinedTransformationMatrix); }
	void		Set_Position(_float3 vPosition) { memcpy(&m_TransformationMatrix.m[3], &vPosition, sizeof(_float3)); }
	_float4x4	Get_OffsetFloat4x4() const { return m_OffsetMatrix; }

	void		Set_TransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }
	void		BlendAnimation(const KEYFRAME& tCurKey, _float fRatio);
	void		Setup_PreKeyFrame();
	void		Reset_Bone();



public:
	HRESULT Initialize(string strName, _float4x4 matTransformation, _float4x4 matOffset, _int iBoneIndex, _int iParentIndex, _uint iDepth);
	void	Invalidate_CombinedTransformationMatrix(CModel::BONES& Bones, _fmatrix PivotMatrix);

private:
	_char			m_szName[MAX_PATH] = "";//! 뼈 이름
	_int			m_iParentIndex = { 0 }; //! 부모 뼈 인덱스
	_int			m_iIndex;
	_int			m_iDepth;

	_float4x4		m_TransformationMatrix; //! 이 뼈만의 상태행렬
	_float4x4		m_OffsetMatrix;
	_float4x4		m_CombinedTransformationMatrix; //! 이 뼈만의 상태행렬 * 부모의 컴바인 행렬

	KEYFRAME		m_tPreKeyFrame;

public:
	static CBone*	Create(string strName, _float4x4 matTransformation, _float4x4 matOffset, _int iBoneIndex, _int iParentIndex, _uint iDepth);
	CBone*			Clone();
	virtual void	Free() override;

};

END

