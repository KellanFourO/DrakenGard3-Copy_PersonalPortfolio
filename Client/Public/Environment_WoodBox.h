#pragma once
#include "NonAnimObject.h"


BEGIN(Client)

class CEnvironment_WoodBox final : public CNonAnimObject
{
public:
	typedef struct tagTreeDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vPos = { 0.f, 0.f, 0.f, 0.f };
	}TREE_DESC;

private:
	CEnvironment_WoodBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnvironment_WoodBox(const CEnvironment_WoodBox& rhs);
	virtual ~CEnvironment_WoodBox() = default;

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
	
public:
	/* ������ü�� �����Ѵ�. */
	static CEnvironment_WoodBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* �纻��ü�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END

