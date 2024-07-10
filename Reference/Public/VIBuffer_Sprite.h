#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Sprite final : public CVIBuffer
{
private:
	CVIBuffer_Sprite(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Sprite(const CVIBuffer_Sprite& rhs);
	virtual ~CVIBuffer_Sprite() = default;

public:
	//TODO override의 관한 이야기
	//! 오버라이드를 붙이면 부모클래스의 함수를 호출하지않고, 자식인 내 함수로 호출하라고 선언한 것
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;


	_int	m_iVerCount = 0; //! 세로 칸 개수
	_int	m_iHorCount = 0; //! 가로 칸 개수

public:
	static CVIBuffer_Sprite* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END