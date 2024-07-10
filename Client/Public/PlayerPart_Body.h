#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CPlayerPart_Body final : public CPartObject
{
private:
	CPlayerPart_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerPart_Body(const CPlayerPart_Body& rhs);
	virtual ~CPlayerPart_Body() = default;


public:
	void	Set_Loop(_bool bLoop) { m_bLoop = bLoop; }
	void	Add_BloodCount() { if(m_iBloodCount < 5)m_iBloodCount++; }
	void	Set_BloodyMode(_bool bBloodyMode) { m_bBloodyMode = bBloodyMode;}
	_bool	Get_BloodyMode() { return m_bBloodyMode;}


public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	virtual void Write_Json(json& Out_Json) override;
	virtual void Load_FromJson(const json& In_Json) override;
	virtual void Init_Desc();


private:
	_bool			m_bLoop = true;
	_bool			m_bBloodyMode = false;
	_float4			m_vBloodyColor = {};
	CTexture*		m_pBodyBloodTexture = { nullptr };
	_int			m_iBloodCount = 0;

	_float			m_fBloodyAcc = 0.f;
	_float			m_fBloodyTime = 30.f;


public:
	/* ������ü�� �����Ѵ�. */
	static CPlayerPart_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* �纻��ü�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;

};

END

