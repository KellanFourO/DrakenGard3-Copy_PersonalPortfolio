#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Model_Instance;
END

BEGIN(Client)

class CNonAnimObject abstract : public CGameObject
{
protected:
	CNonAnimObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNonAnimObject(const CNonAnimObject& rhs);
	virtual ~CNonAnimObject() = default;

protected:
	LEVEL				m_eCurrentLevelID = { LEVEL_END };

protected:
	
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CVIBuffer_Model_Instance* m_pVIBuffer_Instance = { nullptr };

public:
	virtual void Free() override;
};

END

