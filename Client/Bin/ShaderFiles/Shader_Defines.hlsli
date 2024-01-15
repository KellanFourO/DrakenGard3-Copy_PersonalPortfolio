sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointSampler = sampler_state
{
    filter = min_mag_mip_Point;
    AddressU = wrap;
    AddressV = wrap;
};

RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockWise = false;
};

RasterizerState RS_Cull_None
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockWise = false;
};

RasterizerState RS_Fill_Wireframe
{
    FillMode = WireFrame;
    CullMode = Back;
    FrontCounterClockWise = false; // 반시계로 추려낼것 인지
};

DepthStencilState DSS_Default
{
    DepthEnable = true; // 깊이테스트 수행 할 것인지
    DepthWriteMask = All; // 깊이기록을 수행할 것인지
    DepthFunc = Less_Equal; //깊이테스트 조건은 작거나 같을때로 그린다.
};

DepthStencilState DSS_None
{
    DepthEnable = false; // 깊이테스트 수행 안 할 것.
    DepthWriteMask = Zero; // 깊이기록 수행 안 할 것.
};

BlendState BS_Default
{
    BlendEnable[0] = false; // 블렌드는 렌더타겟 개수만큼 할 수 있어서 배열 인덱스형태로 접근 하다. 최대 8개까지니 인덱스는 0 ~ 7
};

BlendState BS_AlphaBlend_Add
{
    BlendEnable[0] = true;

    SrcBlend = SRC_ALPHA;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};