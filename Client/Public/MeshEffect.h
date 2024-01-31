#pragma once
#include "NonAnimObject.h"

BEGIN(Client)

class CMeshEffect final : public CNonAnimObject
{
public:
	typedef struct tagMesh_EffectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		wstring	strModelTag;
		_uint	iShaderPassIndex;
		_float3 vCenter;
		_float  fRange;
		_float2 vSpeed;
		_float2 vScale;
		_float3 vRotation;
		_float4	vColor;
		_float2 vLifeTime;
		_float	fAge;
		_float4 vDir = { 1.f, 0.f, 0.f, 0.f };
		_float2	vRandomRotation = {};
		_bool bRandom = true;
	}MESH_EFFECTDESC;

private:
	CMeshEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshEffect(const CMeshEffect& rhs);
	virtual ~CMeshEffect() = default;


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
	
	MESH_EFFECTDESC		m_tEffectDesc = {};
	
	random_device				m_RandomDevice;
	mt19937_64					m_RandomNumber;

public:
	/* 원형객체를 생성한다. */
	static CMeshEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END

