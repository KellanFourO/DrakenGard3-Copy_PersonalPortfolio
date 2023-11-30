#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_DynamicCube final : public CVIBuffer
{
private:
	CVIBuffer_DynamicCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_DynamicCube(const CVIBuffer_DynamicCube& rhs);
	virtual ~CVIBuffer_DynamicCube() = default;

public:
	virtual HRESULT Initialize_Prototype	();
	virtual HRESULT Initialize				(void* _pArg)	override;

public:
	void			Update(MESH_VTX_INFO _VtxInfo, _fmatrix _WorldMatrix);

public:
	static	CVIBuffer_DynamicCube* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
