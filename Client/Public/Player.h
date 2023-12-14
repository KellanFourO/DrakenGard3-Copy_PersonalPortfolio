#pragma once

#include "Client_Defines.h"
#include "AnimObject.h"

BEGIN(Engine)
class CNavigation;
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayer final : public CAnimObject
{
public:
	typedef struct tagPlayerDesc : public GAMEOBJECT_DESC
	{
		int a;
	}PLAYER_DESC;
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CNavigation*		m_pNavigationCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };	
	CModel*				m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	/* ������ü�� �����Ѵ�. */
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	/* �纻��ü�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END