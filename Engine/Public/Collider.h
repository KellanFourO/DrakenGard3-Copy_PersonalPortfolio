#pragma once

#include "Component.h"

#include "BoundingBox_AABB.h"
#include "BoundingBox_OBB.h"
#include "BoundingBox_Sphere.h"
//!
//!/* �浹ü. */
//!/* �浹ü�� �����ϴ� ��� + �浹ü�� �׸��� ��� + �浹�� ���ϴ� ��� . */

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_Sphere, TYPE_AABB, TYPE_OBB, TYPE_END };
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	TYPE					m_eType = { TYPE_END };
	class CBoundParent*		m_pBounding = { nullptr };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*							m_pEffect = { nullptr };

private:
	ID3D11InputLayout*						m_pInputLayout = { nullptr };
#endif


public:
	static CCollider*		Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END