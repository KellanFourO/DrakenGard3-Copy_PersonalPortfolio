
namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };

		WINMODE			eWinMode; //! 어떠한 창모드를 사용할 것인가. WINMODE_FULL 전체화면, WINMODE_WIN 창 화면
		HWND			hWnd; //! 어떠한 창인가
		unsigned int	iBackBufferSizeX; //! 백버퍼에게 줄 창의 가로사이즈
		unsigned int	iBackBufferSizeY; //! 백버퍼에게 줄 창의 세로사이즈

	}GRAPHIC_DESC;

	typedef struct
	{
		class CTexture*	pMtrlTextures[AI_TEXTURE_TYPE_MAX]; //! 어심프라이브러리에서 제공하는 재질 텍스처타입 열거체
	}MATERIAL_DESC;



	typedef struct tagStatusDesc
	{
		enum ATTACKTYPE { NORMAL_ATTACK, RUSH_ATTACK, CHARGE_ATTACK, UPPER_ATTACK, DOWN_ATTACK, ATTACKTYPE_END  };

		_float fMaxHp;
		_float fCurrentHp;
		_float fDmg;
		ATTACKTYPE eAttackType;

	}STATUS_DESC;

	typedef struct tagModelData
	{
		wstring strBoneDataPath;
		wstring strMeshDataPath;
		wstring strMaterialDataPath;
		wstring strAnimationDataPath;
		wstring strHitAnimationDataPath;
	}MODELDATA;

	typedef struct tagVertex_Model_Instance
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;
	}VTXMODELINSTANCE;

	struct INSTANCE_MESH_DESC
	{
		_float3 vRotation;
		_float3	vScale;
		_float3 vPos;
		_float fMaxRange;
		_float3 vCenter;

		void Reset()
		{
			ZeroMemory(this, sizeof(INSTANCE_MESH_DESC));
		}

		_matrix Get_Matrix() const
		{
			_matrix TransformationMatrix;
			_matrix RotationMatrix, ScaleMatrix;

			_vector vPitchYawRoll;
			_vector vPosition;

			vPitchYawRoll = XMLoadFloat3(&vRotation);
			vPosition = XMVectorSetW(XMLoadFloat3(&vPos), 1.f);

			RotationMatrix = XMMatrixRotationRollPitchYawFromVector(vPitchYawRoll);
			ScaleMatrix = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
			TransformationMatrix = ScaleMatrix * RotationMatrix;
			TransformationMatrix.r[3] = vPosition;

			return TransformationMatrix;
		}

		void	Bake_CenterWithMatrix()
		{
			_vector vCenterFromVector = XMLoadFloat3(&vCenter);
			XMStoreFloat3(&vCenter, XMVector3TransformCoord(vCenterFromVector, Get_Matrix()));
		}
	};
// 	typedef struct tagPassDesc
// 	{
// 		ID3D11VertexShader* VertexShader = { nullptr };
// 		ID3D11PixelShader* PixelShader = { nullptr };
// 		ID3D11InputLayout* layout = { nullptr };
// 	}PASS_DESC;

	typedef struct
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };
		TYPE			eType;
		XMFLOAT4		vDirection;
		XMFLOAT4		vPosition;

		float			fRange;

		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;
	}LIGHT_DESC;

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

	typedef struct ENGINE_DLL tagVertex_Position_Size
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		static const unsigned int					iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertex_Position_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int				iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC   Elements[iNumElements];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertexCube_Position_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int					iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXCUBE;

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

	typedef struct ENGINE_DLL tagVertex_Dynamic_Field
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int					iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXFIELD;

	typedef struct tagKeyFrame
	{
		XMFLOAT3	vScale, vPosition;
		XMFLOAT4	vRotation; //! 쿼터니언으로 되있어. XMFLOAT4야.
		float		fTrackPosition; //! 동작을 취하는 시간은 언제야?
		_bool		bTest = false;
	}KEYFRAME;

	typedef struct tagStatusMatrix
	{
		XMFLOAT4	vRight, vUp, vLook, vPosition;
		XMFLOAT4	vColor;
	}VTXINSTANCE;

	typedef struct ENGINE_DLL tagParticleRect
	{
		static const unsigned int					iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTX_PARTICLE_RECT;

	typedef struct ENGINE_DLL tagParticlePoint
	{
		static const unsigned int					iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTX_PARTICLE_POINT;
}


