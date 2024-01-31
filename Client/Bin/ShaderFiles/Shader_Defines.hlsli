sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler ClampSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = clamp;
    AddressV = clamp;
};


sampler BorderSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = border;
    AddressV = border;
};

sampler PointSampler = sampler_state
{
    filter = min_mag_mip_Point;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointClampSampler = sampler_state
{
    filter = min_mag_mip_point;
    AddressU = clamp;
    AddressV = clamp;
};

sampler sRGBSampler
{
    filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
    MaxAnisotropy = 16;
    BorderColor = float4(0, 0, 0, 0);
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
    FrontCounterClockWise = false; // �ݽð�� �߷����� ����
};

DepthStencilState DSS_Default
{
    DepthEnable = true; // �����׽�Ʈ ���� �� ������
    DepthWriteMask = All; // ���̱���� ������ ������
    DepthFunc = Less_Equal; //�����׽�Ʈ ������ �۰ų� �������� �׸���.
};

DepthStencilState DSS_None
{
    DepthEnable = false; // �����׽�Ʈ ���� �� �� ��.
    DepthWriteMask = Zero; // ���̱�� ���� �� �� ��.
    DepthFunc = Less_Equal;
};

DepthStencilState DSS_OnlyStencil
{ //���ٽ� �׽�Ʈ�� ���� �׽�Ʈ�� Ȱ��ȭ �ؾ� ��
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = always;

    StencilEnable = true;
    StencilWriteMask = 0xff;

    BackFaceStencilFunc = always;
    BackFaceStencilPass = replace;
    BackFaceStencilFail = keep;
};

BlendState BS_Default
{
    BlendEnable[0] = false; // ����� ����Ÿ�� ������ŭ �� �� �־ �迭 �ε������·� ���� �ϴ�. �ִ� 8�������� �ε����� 0 ~ 7
};

BlendState BS_AlphaBlend_Add
{
    BlendEnable[0] = true;
    BlendOp = Add;
    SrcBlend = SRC_ALPHA;
    DestBlend = Inv_Src_Alpha;
};

BlendState BS_Blend_Add
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = add;
};