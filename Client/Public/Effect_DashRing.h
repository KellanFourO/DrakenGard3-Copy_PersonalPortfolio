#pragma once

#include "Client_Defines.h"
#include "AlphaObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_DashRing final : public CAlphaObject
{
public:

	typedef struct tagDashRingDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float fLifeTime = {};
		_float4 vCreatePos;
		_float4 vCreateLook;


	}DASHRING_DESC;

private:
	CEffect_DashRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_DashRing(const CEffect_DashRing& rhs);
	virtual ~CEffect_DashRing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CTexture* m_pMaskTextureCom = { nullptr };
	CTexture* m_pNoiseTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	
	DASHRING_DESC		m_tInfo = {};

	
	_float				m_fAge = 0.f;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEffect_DashRing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END