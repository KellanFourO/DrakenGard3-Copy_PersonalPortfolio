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
		_int	iShaderPathIndex = {0};
		_uint	iNumInstance;
		_float3 vCenter;
		_float	fRange;
		_float2 vSpeed;
		_float2 vScale;
		_float3 vRotation;
		_float4 vColor;
		_float2 vLifeTime;
		_float4 vDir = { 1.f, 0.f, 0.f, 0.f };
		_bool	bRandom = true;
		_bool	bTimeScale = false;
		_float2	vRandomRotation = {};
		_float3	vInterval = { 1.f, 1.f, 1.f };
		
		_float fAge;
		_bool	bSingleSpriteAnim = false;

		_bool	bMultiSpriteAnim = false;
		_float  fMultiSpriteCount = { 0.f };
		_float	fSpriteSpeed = {};
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

	_float				m_fAge = 0.f;

private:
	_float				m_fFrame = { 0.0f };
	

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