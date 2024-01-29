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
private:
	CEffect_BornFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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
	_float				m_fWidthSize = 512.f;
	_float				m_fHeightSize = 512.f;

	_int				m_iNumVer = 4;
	_int				m_iNumHor = 4;
	
	_int				m_iVerFrame = 0;
	_int				m_iHorFrame = 0;

	_int				m_iStartVer = 0;
	_int				m_iMaxVer = 3;
	_int				m_iStartHor = 0;
	_int				m_iMaxHor = 3;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEffect_BornFire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END