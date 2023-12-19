
namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };

		WINMODE			eWinMode; //! ��� â��带 ����� ���ΰ�. WINMODE_FULL ��üȭ��, WINMODE_WIN â ȭ��
		HWND			hWnd; //! ��� â�ΰ�
		unsigned int	iBackBufferSizeX; //! ����ۿ��� �� â�� ���λ�����
		unsigned int	iBackBufferSizeY; //! ����ۿ��� �� â�� ���λ�����

	}GRAPHIC_DESC;

	typedef struct
	{
		class CTexture*	pMtrlTextures[AI_TEXTURE_TYPE_MAX]; //! ��������̺귯������ �����ϴ� ���� �ؽ�óŸ�� ����ü
	}MATERIAL_DESC;

	typedef struct tagPassDesc
	{
		ID3D11VertexShader* VertexShader = { nullptr };
		ID3D11PixelShader* PixelShader = { nullptr };
		ID3D11InputLayout* layout = { nullptr };
	}PASS_DESC;

	typedef struct tagRayDesc
	{
		XMFLOAT4 vOrigin;
		XMFLOAT3 vDirection;
		float	fLength;
	}RAY;

	typedef struct ENGINE_DLL tagVertex_Position
	{
		XMFLOAT3		vPosition;

		static const unsigned int				iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC   Elements[iNumElements];
	}VTXPOS;

	typedef struct ENGINE_DLL tagVertex_Position_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int				iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC   Elements[iNumElements];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Texcoord_Tangent
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent; 

		static const unsigned int					iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMESH;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Texcoord_Tangent_BlendIndices_BlendWeights
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		static const unsigned int					iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXANIMMESH;

	typedef struct ENGINE_DLL tagVertex_Dynamic_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
	}VTXDYNAMIC;

	typedef struct tagKeyFrame
	{
		XMFLOAT3	vScale, vPosition;
		XMFLOAT4	vRotation; //! ���ʹϾ����� ���־�. XMFLOAT4��.
		float		fTrackPosition; //! ������ ���ϴ� �ð��� ������?
		_bool		bTest = false;
	}KEYFRAME;
}


