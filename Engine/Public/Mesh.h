#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh);
	virtual HRESULT Initialize(void* pArg) override;

public:
	//TODO Create시에 인자값으로 const aiMesh* 를 받는 것을 알 수 있다.
	//! aiMesh는 어심프 라이브러리로 읽어들인 aiScene 구조체 내부에 들어있다.
	//! 그러니 당연히 읽어들이면서 Mesh를 생성해준다는 느낌으로 가면 될것.
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

};

END

