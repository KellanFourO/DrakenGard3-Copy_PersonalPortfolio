#pragma once
#include "NonAnimObject.h"

BEGIN(Client)

class CMeshEffect final : public CNonAnimObject
{
public:
	typedef struct tagMesh_EffectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vPos = { 0.f, 0.f, 0.f, 0.f };
		wstring	strModelTag;
		_uint	iShaderPassIndex;
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
	

public:
	/* 원형객체를 생성한다. */
	static CMeshEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END

