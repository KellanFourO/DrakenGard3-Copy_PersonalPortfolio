#pragma once
#include "AnimObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster final : public CAnimObject
{
private:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Write_Json(json& Out_Json) override;
	virtual void Load_FromJson(const json& In_Json) override;
	virtual void Init_Desc();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel*  m_pModelCom = { nullptr }; //#�����Ŀ���_��������_�����

private:
	//STATE_LINK_MONSTER_DESC      m_tLinkStateDesc;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	/* ������ü�� �����Ѵ�. */
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* �纻��ü�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;

};

END

