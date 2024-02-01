#pragma once
#include "Client_Defines.h"
#include "AlphaObject.h"

BEGIN(Engine)
class CVIBuffer_Trail;
class CShader;
class CTexture;
END

BEGIN(Client)

class CEffect_Trail final : public CAlphaObject
{
public:
    struct EFFECT_TRAIL_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        _float3		vStartPos;
        _float3		vEndPos;
        _uint		iMaxCount;
    };

private:
        CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel);
        CEffect_Trail(const CEffect_Trail& rhs);
    virtual ~CEffect_Trail() = default;

public:
    void Set_TimeScaleLayer(const _uint& In_iTimeScaleLayer) {
        m_iTimeScaleLayerIndex = In_iTimeScaleLayer;
    }
   
    void Set_TextureIndex(_uint In_iDiffuseIndex, _uint In_iMaskIndex, _uint In_iNoiseIndex)
    {
        m_iDiffuseIndex = In_iDiffuseIndex;
        m_iMaskIndex = In_iMaskIndex;
        m_iNoiseIndex = In_iNoiseIndex;
    }

public:
    void                                On_Trail() { m_bTrailOn = true;}
    void                                Off_Trail() { m_bTrailOn = false;}

public:
    virtual HRESULT                     Initialize_Prototype() override;
    virtual HRESULT                     Initialize(void* pArg) override;
    virtual void                        Tick(_float fTimeDelta, _fmatrix OwnerWorldMatrix);
    virtual void                        Late_Tick(_float fTimeDelta) override;
    virtual HRESULT                     Render() override;


    void                                Reset_Points();

protected:
    HRESULT                             Ready_Components();
    HRESULT                             Bind_ShaderResources();

protected:
    CVIBuffer_Trail*    m_pVIBuffer = { nullptr };
    CShader*            m_pShaderCom = { nullptr };
    CTexture*           m_pDiffuseTextureCom = { nullptr };
    CTexture*           m_pMaskTextureCom = { nullptr };
    CTexture*           m_pNoiseTextureCom = { nullptr };

    LEVEL			    m_eCurrentLevel = LEVEL_END;

protected:
    EFFECT_TRAIL_DESC                   m_tTrailDesc = {};

    _uint                               m_iTimeScaleLayerIndex;
    _bool                               m_bTrailOn = false;
    _uint                               m_iDiffuseIndex = 0;
    _uint                               m_iMaskIndex    = 0;
    _uint                               m_iNoiseIndex   = 0;

    _float4x4                           m_OwnerWorldMatrix = {};

// public:
//     virtual void OnEnable(void* pArg) override;
//     virtual void OnDisable() override;
public:
    static CEffect_Trail*   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eCurrentLevel);
    virtual CGameObject*    Clone(void* pArg) override;
    virtual void            Free() override;
};

END