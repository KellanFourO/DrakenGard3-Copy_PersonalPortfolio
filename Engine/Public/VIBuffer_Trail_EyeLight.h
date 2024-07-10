// #pragma once
//#include "VIBuffer_Trail.h"
//
//BEGIN(Engine)
//
//class ENGINE_DLL CVIBuffer_Trail_EyeLight final: public CVIBuffer_Trail
//{
//	CVIBuffer_Trail_EyeLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	CVIBuffer_Trail_EyeLight(const CVIBuffer_Trail_EyeLight& rhs);
//	virtual ~CVIBuffer_Trail_EyeLight() = default;
//
//public:
//	virtual HRESULT Initialize_Prototype() override;
//	virtual HRESULT Initialize(void* pArg) override;
//	
//
//public:
//	void Update(_float _fTimeDelta, weak_ptr <CTransform> _pOwnerTransform, weak_ptr<CBoneNode> _pOwnerBoneNode, weak_ptr<MODEL_DATA> _pOwnerModel_Data);
//
//
//public:
//	static CVIBuffer_Trail_EyeLight* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
//	virtual CComponent* Clone(void* pArg) override;
//	virtual void Free() override;
//};
//
//END