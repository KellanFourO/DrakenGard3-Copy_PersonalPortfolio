#pragma once
#include "Client_Defines.h"
#include "NonAnimObject.h"

BEGIN(Client)

class CSkyDome final : public CNonAnimObject
{
private:
	CSkyDome(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkyDome(const CSkyDome& rhs);
	virtual ~CSkyDome() = default;

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	HRESULT			Ready_Components();
	HRESULT			Bind_ShaderResources();

public:
	static	CSkyDome*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END