#pragma once
#include "Client_Defines.h"
#include "Level.h"
#include "BackGround_Loading.h"

//! �ε� ������ �����ְ� ���� ����� �����ϰ� �����ش�.
//! m_eNextLevelID�� ���� ������ �ʿ��� �ڿ��� �غ��ϱ� ���� �۾��� �ϴ� �����带 �������ش�.
BEGIN(Engine)
class CTexture;
END


BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVEL					m_eNextLevelID = { LEVEL_END };
	class CLoader*			m_pLoader = { nullptr };
	CTexture*				m_pTextureCom = { nullptr};
	CBackGround_Loading*	m_pLoading = { nullptr };

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

END

