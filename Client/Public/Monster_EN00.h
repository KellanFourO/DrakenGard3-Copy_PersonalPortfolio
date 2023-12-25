#pragma once
#include "AnimObject.h"

BEGIN(Engine)
class CShader;
class CCollider;
class CModel;
END

BEGIN(Client)

class CMonster_EN00 final : public CAnimObject
{
private:
	CMonster_EN00(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_EN00(const CMonster_EN00& rhs);
	virtual ~CMonster_EN00() = default;

public:
	CPartObject* Find_PartObject(const wstring& strPartTag);

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
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT	Ready_States();
	HRESULT	Add_PartObject(const wstring& strPrototypeTag, const wstring& strPartTag, void* pArg);
	
	HRESULT Bind_ShaderResources();

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr }; //#버퍼컴에서_모델컴으로_변경됨
	CCollider*		m_pColliderCom = { nullptr };
	CStateMachine*	m_pStateCom = { nullptr };

private:
	map<const wstring, class CPartObject*>	m_PartObjects;
	

public:
	/* 원형객체를 생성한다. */
	static CMonster_EN00* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;

};

END

