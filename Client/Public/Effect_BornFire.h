#pragma once

#include "Client_Defines.h"
#include "AlphaObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_BornFire final : public CAlphaObject
{	
public:
	struct BORNFIRE_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vPos;
		_float4	vLook;
		CGameObject* pTarget = { nullptr };
		_float fLifeTime = 3.f;
		_bool bBossBreath = false;
		_float3 vScale;
	};
	

private:
	CEffect_BornFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel);
	CEffect_BornFire(const CEffect_BornFire& rhs);
	virtual ~CEffect_BornFire() = default;

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

private:
//! 누적
	_float				m_fTimeAcc = 0.f;
	_float				m_fAddTime = 0.25f;


//! 셋팅
	

	_int				m_fAnimationSizeX = 512.f;
	_int				m_fAnimationSizeY = 512.f;

	_int				m_fSpriteSizeX = 2048.f;
	_int				m_fSpriteSizeY = 2048.f;

	_int				m_iStartVer = 0;
	_int				m_iStartHor = 0;

	_int				m_iCurrentVer = 0;
	_int				m_iCurrentHor = 0;

	_int				m_iMaxVer = 4;
	_int				m_iMaxHor = 4;

	CGameObject* m_pTarget		= nullptr;
	_float4						m_vCreatePos = {};
	_float4						m_vCreateLook = {};
	_float2						m_vAddUVPos = {};
	_bool						m_bBreath = false;

	_float						m_fAge = 0.f;

	LEVEL						m_eCurrentLevel = LEVEL_END;
	
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();



public:
	/* 원형객체를 생성한다. */
	static CEffect_BornFire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END