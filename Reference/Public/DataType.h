#pragma once

namespace Engine
{

	enum ENGINE_DLL MODEL_TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
	
	struct VTXNONANIM
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT3 vTexcoord;
		XMFLOAT3 vTangent;
	};
	
	struct VTXANIM
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
	
		XMUINT4			vBlendIndex;
		XMFLOAT4		vBlendWeight;
	};
	
	struct asBone
	{
		string				strName;
		XMFLOAT4X4			matTransformation;
		XMFLOAT4X4			matOffset;
	
		_int				iIndex = -1;
		_int				iParent = -1;
	
		_uint				iDepth;
	};
	
	struct asMesh
	{
		string				strName;
		_bool				isAnim;
		_uint				iMaterialIndex;
		_int				iNumFace;

		vector<VTXMESH>			vecNonAnims;
		vector<VTXANIMMESH>		vecAnims;
		vector<FACEINDICES32>	vecIndices;
		
		vector<_int>		vecBoneIndices;
		vector<XMFLOAT4X4>	vecOffsetMatrix;
	};
	
	struct asMaterial
	{
		string			strTextureFilePath[AI_TEXTURE_TYPE_MAX];
	};
	
	
	
	struct asKeyFrame
	{
		_float				fTrackPosition;
		XMFLOAT3			vScale;
		XMFLOAT4			vRotation;
		XMFLOAT3			vPosition;
	};
	
	struct asChannel
	{
		string				strName; /* Bone Name */
	
		vector<asKeyFrame*> vecKeyFrames;
	};
	
	struct asAnimation
	{
		string				strName;
	
		_float				fDuration;			/* 전체 재생 시간 */
		_float				fTicksPerSecond;	/* 초당 재생 속도 */
	
		vector<asChannel*>	vecChannels;
	};
}