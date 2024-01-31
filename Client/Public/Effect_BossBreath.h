#pragma once
#include "NonAnimObject.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CEffect_BossBreath final : public CNonAnimObject
{
public:
	struct BOSSBREATH_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vPos;
		_float4	vLook;
		CGameObject* pTarget = { nullptr };
		_float fLifeTime = 3.f;
		
	};

private:
	CEffect_BossBreath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_BossBreath(const CEffect_BossBreath& rhs);
	virtual ~CEffect_BossBreath() = default;


public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	wstring				m_strModelTag = TEXT("");
	_uint				m_iShaderPassIndex = 0;
	_float				m_fAge = 0.f;
	
	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;
	CGameObject*				m_pTarget = nullptr;
	_float4						m_vCreatePos = {};
	_float4						m_vCreateLook = {};
	_float2						m_vAddUVPos = {};
	

private:
	CTexture*					m_pTextureCom = { nullptr };

private:
	void CreateFireEffect();


public:
	/* 원형객체를 생성한다. */
	static CEffect_BossBreath* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END

