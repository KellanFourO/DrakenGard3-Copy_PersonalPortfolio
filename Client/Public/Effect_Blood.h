#pragma once

#include "Client_Defines.h"
#include "AlphaObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CRigidBody;
END

BEGIN(Client)

class CEffect_Blood final : public CAlphaObject
{	
public:
	typedef struct tagEffectBloodDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vCreatePos;
		_float	fPlaySpeed = { 1.f};
		_float  fLifeTime = {};
		_float3 vScale = { 1.f, 1.f, 1.f};
		_float3 vDir = { 1.f, 0.f, 0.f };
		_int	iAnimationSizeX = {512};
		_int	iAnimationSizeY = {512};
		_int	iSpriteSizeX = {2048};
		_int	iSpriteSizeY = {2048};
		
		_int	iStartVer = {0};
		_int	iStartHor = {0};
		_int    iMaxVer = {4};
		_int	iMaxHor = {4};
	}EFFECT_DESC;

private:
	CEffect_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Blood(const CEffect_Blood& rhs);
	virtual ~CEffect_Blood() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CRigidBody*			m_pRigidBodyCom = { nullptr};
private:
	_float				m_fFrame = { 0.0f };
	EFFECT_DESC			m_tEffectDesc = {};

	_int						m_iCurrentVer = 0;
	_int						m_iCurrentHor = 0;
	_float2						m_vAddUVPos = {};
	_float						m_fTimeAcc = 0.f;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEffect_Blood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END