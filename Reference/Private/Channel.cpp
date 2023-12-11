#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pChannel, const CModel::BONES& Bones)
{
	strcpy_s(m_szName, pChannel->mNodeName.data);


	//! ���� ����ִ� ���� Ư�� �ִϸ��̼ǿ��� ����ϴ� ���������� �̸��� ���� �� ã�Ƽ� �ε����� �����ðž�.
	_uint	iBoneIndex = { 0 };

	auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)
		{
			if (false == strcmp(m_szName, pBone->Get_Name()))
			{
				return true;
			}

			++iBoneIndex;

			return false;
		});

	if(iter == Bones.end())
		return E_FAIL;

	m_iBoneIndex = iBoneIndex; //! ã�Ҵ�

	//!#Ű�����Ӱ���_������
	//!  ������, �����̼�,�������� Ű�������� ������ ���� �ٸ� �� �־�.
	//!  �ֳ��ϸ�,  �ִϸ��̼��� �����ɶ� ũ���� ��ȭ�� ���� ȸ���� �� ���� �ְ� ���������� ��ġ�� �޶����µ� ȸ���� ���� �������� �ִ� ����.
	//! ��, ���� ��ġ �״���� ���� �ִٴ°ž�. �׷��� Ű �������� ������ �� �ٸ��� 
	//! �� Ű�������� ������ ������,�����̼�,������ �� �������ִ� Ű�������� �������� ���� ū���� ���� ������ ã�Ƽ� �����Ұž�.
	m_iNumKeyFrames = max(pChannel->mNumScalingKeys, pChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(pChannel->mNumPositionKeys, m_iNumKeyFrames);

	//TODO �� ���������� ���ߵ� ���� ��ġ�� ����ؾ߰���?
	//!  �ؿ��� ������ �����ǵ�, ������� ���� �ִϸ��̼��� 3�� �������� �������� ������������ �� �־�.
	//! �׷��� ���� ���ǹ��� �ɾ �ɷ��ٰǵ�, �� ���ǹ��� ����������Ѵٸ� �׳� 0���� ä�����ž�. 
	//! ������ �������� ������ �����س��� ���� ä���༭ ���� ���� ����ϰ� ����.
	_float3 vScale;
	_float4	vRotation;
	_float3	vPosition;

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME		KeyFrame = {};

		if (i < pChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pChannel->mScalingKeys[i].mTime;
		}

		if (i < pChannel->mNumRotationKeys)
		{
			//! ������� �����̼�Ű�� ����� aiQuaternion�̾�. x,y,z,w ���̾ƴ� w,x,y,z ������ �Ǿ��־�
			//! �츮�� ���� �Ŷ� ������ �ٸ���? �׷��� memcpy�� ����ϸ� �ȵ�.
		
			vRotation.x = pChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTrackPosition = pChannel->mRotationKeys[i].mTime;
		}

		if (i < pChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrame)
{
	//! Ŀ��Ʈ Ʈ���������� 0.0���� ������ ���� ���������� ����� ����. �׷��ϱ� �ε����� 0���� �ʱ�ȭ ��������
	if(0.0f == fCurrentTrackPosition)
		*pCurrentKeyFrame = 0;

	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;

	KEYFRAME	LastKeyFrame = m_KeyFrames.back(); //! �ִϸ��̼��� ������ �������� ����ϴ°Ű���?

	//! ������ Ű�������� �ش�ִϸ��̼��� ��ü������ �෹�̼��� ����ɶ����� �ڱ� ���¸� ����������߰���?
	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMLoadFloat3(&LastKeyFrame.vPosition);
	}
	
	//! ���� ���� �����Ӱ� ���� �����Ӱ� ���������� ���� ������ ���¸� ���Ž�����߰���?
	else //! ������ �������� ������������ ���ݾ�? �׷��� ���� ������ +1�̶�� ������ ���� �ȵ�. �ٵ� ������ if�� �ɷ����ݾ�. else ������
	{
		//! ������� �ִϸ��̼��� �ִٰ� ġ��. �����Ϸ��� �ٸ��� ���� �ִ� �ִϸ��̼��� 0�� �������̶�� �غ��ڰ�.
		//! ���� �ִϸ��̼� �����ġ�� 0�������Ӻ��� Ŀ���ٸ� ������ �����ٴ� �ִϸ��̼��� 1�� �������̶�� 1������������ �ٲ��߰���?
		
		//TODO While���� ����� ����.
		//! �������� �������� �޽ð� ������ ������ �߻���.
		//! �� �����°ɱ�? �𵨿� Play_Animation �Լ��� ȣ���ϸ鼭 ���ο��� Invalidate_Combie �Լ��� ȣ���ؼ� �ش� ���� Combine ����� �������ְ��־���
		//! �׷��� �������� �������鼭 �Ѱ��ִ� ���ڰ��̾��� TimeDelta ���� �������� �ʹ� ū ���� �Ѱ��� �����ž�.
		//! �׷���, ������ �ε����� �ѹ��� �ǳʶپ��������. ���� ���ڸ� 1���� �ѹ��� 3����.
		//! ���⼭ ���� Ű�������� 2�� ������������ž�. 4���ƴ϶�. ���⼭ ���簡 �߻��Ѱž� �̰� �������� While���� �����ذ���.
		
		while(fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrame + 1].fTrackPosition)
			++*pCurrentKeyFrame;

		//! �ҽ��� ����Ʈ��. �ҽ��� ���� �������� 0�� �������� ���� ���¸� �ǹ��ϰ�, ����Ʈ�� ���� �������� 1�� �������� ���¸� �ǹ���.
		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale		= m_KeyFrames[*pCurrentKeyFrame].vScale;
		vSourRotation	= m_KeyFrames[*pCurrentKeyFrame].vRotation;
		vSourPosition	= m_KeyFrames[*pCurrentKeyFrame].vPosition;
						
		vDestScale		= m_KeyFrames[*pCurrentKeyFrame + 1].vScale;
		vDestRotation	= m_KeyFrames[*pCurrentKeyFrame + 1].vRotation;
		vDestPosition	= m_KeyFrames[*pCurrentKeyFrame + 1].vPosition;


		//! XMVectorLerp��� �Լ��� ���� ��Ư�ϰԵ� ������ �־��ָ� ���������� ���ڰ� ���شܸ�����. ���� ������
		//!  ����ִϸ��̼���ġ ���� 
		_float	fRatio = 
		(fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrame].fTrackPosition) /
		(m_KeyFrames[*pCurrentKeyFrame + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrame].fTrackPosition);

		//! 1��° ���ڿ� 2��° ���ڸ� ������ŭ �����������༭ ���� ������͸� �ʹ���شܸ����� ���� ��Ī����
		vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
		//! ����. ���ʹϾ��� ��¼��? �̰͵� ���� �Լ��ִ� ����?
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio);
	}

	//! ������ �� ���µ�� ���Ϸ����Ѱž�? �ð����´� TransformationMatrix ������ַ��� �ߴ����ݾ�?
	//! ��� ����ž�? �����̼��� ���ʹϾ��ε�? �̰͵� �Լ��־� ���� ����
	//! ��? ���ڰ����� �����̼� ������ �޶�µ�? �����̹���? 0,0,0,1 ���ݾ� �ָ���� ����
	_matrix	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f,0.f,0.f,1.f), vRotation, vPosition);

	//! ���� �� ä�ΰ� ���� �̸��� ���� ���� ã�Ƽ� �� ���� TransformationMatrix �������ٰž�.
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const aiNodeAnim* pChannel, const CModel::BONES& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pChannel, Bones)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
}
