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
};

BlendState BS_Default
{
    BlendEnable[0] = false; // ����� ����Ÿ�� ������ŭ �� �� �־ �迭 �ε������·� ���� �ϴ�. �ִ� 8�������� �ε����� 0 ~ 7
};

BlendState BS_AlphaBlend_Add
{
    BlendEnable[0] = true;

    SrcBlend = SRC_ALPHA;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};