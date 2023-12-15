#pragma once
#include "Component.h"

BEGIN(Engine)

//TODO �׺�Ž�
//! �� �޽��ΰ�?   =  �ﰢ������ �����̴ϱ� �޽���� �θ���
//! �̱��� �Ǵ� ������Ʈ�� ����µ�, �̱������� �̿��K�ҽ� ���߿� ��Ʈ���ϱⰡ ���ŷο�����. ������Ʈ�� �ؼ� �׺�Ž��� �̿��� ��ü���� ���� ������ �ִ� ���·� ��Ʈ���ϴ°� ����.
//! ����� ������ΰ�? �׺�޽��� �ﰢ������ ���� ����� ó���ؼ� ���� �ﰢ���� �����Ѵ�.
//! �ﰢ���� Cell Ŭ������ �߰��ؼ� ���� Ŭ������ �������ִ� ���·� ������ ���̴�.
//! Ŭ������ ���� ���� ������ Cell Ŭ�������� ������ ��.

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
	HRESULT		Make_Neighbors(); //! �̿� ���

public:
	static CNavigation* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strNavgationFilePath);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END

