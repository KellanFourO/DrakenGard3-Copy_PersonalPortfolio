#pragma once
#include "Component.h"

BEGIN(Engine)

//TODO 네비매쉬
//! 왜 메쉬인가?   =  삼각형들의 집합이니까 메쉬라고 부른다
//! 싱글턴 또는 컴포넌트로 만드는데, 싱글턴으로 이용핳할시 나중에 컨트롤하기가 번거로워진다. 컴포넌트로 해서 네비매쉬를 이용할 객체들이 각자 가지고 있는 형태로 컨트롤하는게 좋다.
//! 복사는 어떤형태인가? 네비메시의 삼각형들을 얕은 복사로 처리해서 같은 삼각형을 공유한다.
//! 삼각형은 Cell 클래스를 추가해서 현재 클래스가 가지고있는 형태로 가져갈 것이다.
//! 클래스를 따로 빼는 이유는 Cell 클래스에서 설명할 것.

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNaviDesc
	{
		_int	iCurrentIndex;
	}NAVI_DESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strNavigationFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();
	
public:
	void	Update(_fmatrix WorldMatrix);
	_bool	isMove(_fvector vPosition);

private:
	vector<class CCell*>	m_Cells;
	_int					m_iCurrentIndex = { -1 };
	static _float4x4		m_WorldMatrix;

#ifdef _DEBUG
private:
	class CShader*			m_pShader = { nullptr };
#endif

private:
	HRESULT		Make_Neighbors(); //! 이웃 등록

public:
	static CNavigation* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strNavgationFilePath);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END

