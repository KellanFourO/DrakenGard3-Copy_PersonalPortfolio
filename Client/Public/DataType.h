#pragma once
#include "Engine_Defines.h"

enum class MODEL_TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

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

struct asMesh
{
	string				strName;
	_uint				iMaterialIndex;

	_uint				iNumBones;
	
	vector<_uint>		vecBoneIndices;
	vector<XMFLOAT4X4>	vecOffsetMatrices;
	vector<XMFLOAT3>	vecMeshVertexs;

	

	_bool				isAnim;
	
	vector<VTXNONANIM>	vecNonAnims;
	vector<VTXANIM>		vecAnims;
	vector<_uint>		vecIndices;

	vector<_uint>		vecBones;
};

struct asMaterial
{
	string				strDiffuseFilePath;
	string				strSpecularFilePath;
	string				strNormalFilePath;
};

struct asBone
{
	string				strName;
	XMFLOAT4X4			matTransformation; 
	_int				iParent = -1;
};

struct asKeyFrame
{
	_float				fTime;
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