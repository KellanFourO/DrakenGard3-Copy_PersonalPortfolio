#pragma once
#include "Client_Defines.h"
#include "NonAnimObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Rect;
END

BEGIN(Client)

class CTestSnow final : public CNonAnimObject
{
private:
	CTestSnow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestSnow(const CTestSnow& rhs);
	virtual ~CTestSnow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Particle_Rect* m_pVIBufferCom = { nullptr };

private:
	HRESULT			Ready_Components();
	HRESULT			Bind_ShaderResources();

public:
	static	CTestSnow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

END
