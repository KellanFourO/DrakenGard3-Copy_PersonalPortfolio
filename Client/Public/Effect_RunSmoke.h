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

class CEffect_RunSmoke final : public CAlphaObject
{	
public:
	typedef struct tagEffectRunSmokeDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vCreatePos;
		_float	fPlaySpeed = { 0.2f};
		_float  fLifeTime = {};
		_float3 vScale = { 1.f, 1.f, 1.f};
		_float3 vDir = { 1.f, 0.f, 0.f };
		_int	iAnimationSizeX = {64};
		_int	iAnimationSizeY = {64};
		_int	iSpriteSizeX = {512};
		_int	iSpriteSizeY = {256};
		
		_int	iStartVer = {0};
		_int	iStartHor = {0};
		_int    iMaxVer = {4};
		_int	iMaxHor = {8};
	}EFFECT_DESC;

private:
	CEffect_RunSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_RunSmoke(const CEffect_RunSmoke& rhs);
	virtual ~CEffect_RunSmoke() = default;

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
	/* ������ü�� �����Ѵ�. */
	static CEffect_RunSmoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* �纻��ü�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END