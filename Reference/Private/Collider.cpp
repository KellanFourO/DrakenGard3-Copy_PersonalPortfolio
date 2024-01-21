#include "Collider.h"

#include "BoundingBox_AABB.h"
#include "BoundingBox_OBB.h"
#include "BoundingBox_Sphere.h"

#include "GameInstance.h"

_uint CCollider::g_iColliderID = 0;

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:	CComponent(pDevice, pContext)
	,	m_iID(g_iColliderID++)
{
}

CCollider::CCollider(const CCollider& rhs)
	:	CComponent(rhs)
	, m_iID(g_iColliderID++)
	, m_eType(rhs.m_eType)
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
{
	Safe_AddRef(m_pInputLayout);
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext); //! 버텍스 버퍼와 인덱스 버퍼를 알아서 셋팅해줌.  
	m_pEffect = new BasicEffect(m_pDevice); //! 기본적인 셰이더를 제공해줌
	m_pEffect->SetVertexColorEnabled(true); //! 기본 셰이더의 디폹폴트 컬러는 디폴트 텍스처의 컬러값을 이용해서 그리기 때문에 정점의 컬러값을 사용하겠다는 셋팅을 해줘야함.

	const void* pShaderByteCode = { nullptr };
	size_t	iShaderCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	CBoundParent::BOUNDING_DESC*	pBoundingDesc = static_cast<CBoundParent::BOUNDING_DESC*>(pArg);

	switch (m_eType)
	{
		case Engine::CCollider::TYPE_AABB:
		{
			m_pBounding = CBoundingBox_AABB::Create(m_pDevice,m_pContext , pBoundingDesc);
			break;
		}

		case Engine::CCollider::TYPE_OBB:
		{
			m_pBounding = CBoundingBox_OBB::Create(m_pDevice, m_pContext, pBoundingDesc);
			break;
		}

		case Engine::CCollider::TYPE_Sphere:
		{
			m_pBounding = CBoundingBox_Sphere::Create(m_pDevice, m_pContext, pBoundingDesc);
			break;
		}
	}

	return S_OK;
}

void CCollider::Update(_fmatrix TransformMatrix)
{
	m_pBounding->Update(TransformMatrix);
}

_bool CCollider::Collision(CCollider* pTargetCollider)
{
	return m_pBounding->Collision(pTargetCollider, &m_isCollision);
}

void CCollider::On_Collision(CCollider* pOther, const _float& fX, const _float& fY, const _float& fZ)
{
	
	m_eState = COLLISION_STATE_ON;
}

void CCollider::On_CollisionEnter(CCollider* pOther, const _float& fX, const _float& fY, const _float& fZ)
{
	m_eState = COLLISION_STATE_ENTER;
}

void CCollider::On_CollisionExit(CCollider* pOther, const _float& fX, const _float& fY, const _float& fZ)
{
	m_eState = COLLISION_STATE_EXIT;
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	if (nullptr == m_pBounding)
		return E_FAIL;

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pBatch->Begin();

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));


	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pContext);

	m_pBounding->Render(m_pBatch, m_isCollision == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	m_pBatch->End();


	return S_OK;
}
#endif // _DEBUG

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG

	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

	Safe_Release(m_pInputLayout);

#endif

	Safe_Release(m_pBounding);
}
