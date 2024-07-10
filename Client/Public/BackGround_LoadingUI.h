#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackGround_LoadingUI final : public CGameObject
{
public:
	typedef struct tagBackGroundDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float		fX, fY, fSizeX, fSizeY;
	}BACKGROUND_DESC;

private:
	CBackGround_LoadingUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround_LoadingUI(const CBackGround_LoadingUI& rhs);
	virtual ~CBackGround_LoadingUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override; //! 원형객체의 초기화를 위한 함수.
	virtual HRESULT Initialize(void* pArg) override; //! 사본객체의 초기화를 위한 함수. ( void*를 매개인자로 받아 특수한 사본객체 처리가 가능하다. )
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float			m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4		m_ViewMatrix, m_ProjMatrix;

private:
	_float			m_fFrame = { 0.0f };

private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };

	CVIBuffer_Rect*	m_pVIBufferCom = { nullptr };



private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBackGround_LoadingUI*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext); //! 원형객체 생성
	virtual CGameObject*	Clone(void* pArg) override; //! 사본객체 생성
	virtual void			Free() override;
};

END

