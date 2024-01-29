#pragma once

#include "Client_Defines.h"
#include "NonAnimObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Point;
END

BEGIN(Client)

class CParticle_Object final : public CNonAnimObject
{
public:
	typedef struct tagParticleObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		wstring strTextureTag;
		_int	iShaderPathIndex;
		_float3 vCenter;
		_float	fRange;
		_float2 vSpeed;
		_float2 vScale;
		_float3 vRotation;
		_float4 vColor;
		_float2 vLifeTime;
		_float fAge;
	}PARTICLE_DESC;

private:
	CParticle_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Object(const CParticle_Object& rhs);
	virtual ~CParticle_Object() = default;

public:
	void	Start_Particle() { m_bParticleStart = true; }
	void	End_Particle() { m_bParticleStart = false; }

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CVIBuffer_Particle_Point*		m_pVIBufferCom = { nullptr };

private:
	PARTICLE_DESC					m_tParticleDesc = {};
	_bool							m_bParticleStart = false;

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();


public:
	/* 원형객체를 생성한다. */
	static CParticle_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END