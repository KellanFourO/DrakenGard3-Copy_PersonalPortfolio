#pragma once

#include "Client_Defines.h"
#include "AlphaObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_BossPear final : public CAlphaObject
{
public:

	typedef struct tagBossPearDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float fTickAddScaleX = { 0.1f } ;
		_float fTickAddScaleY = { 0.1f };
		_float fMaxAddScale = {1.f};
		_float fAddTime = { 0.1f };
		_float fLifeTime = {};

		_float4 vCreatePos;
		_float3 vCreateLook;
		

	}BOSSPEAR_DESC;

private:
	CEffect_BossPear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_BossPear(const CEffect_BossPear& rhs);
	virtual ~CEffect_BossPear() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float				m_fFrame = { 0.0f };
	BOSSPEAR_DESC		m_tInfo = {};

	_float				m_fTimeAcc = 0.f;
	_float				m_fAge = 0.f;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CEffect_BossPear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END