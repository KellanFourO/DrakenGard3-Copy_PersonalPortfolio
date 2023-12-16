#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const string strName, vector<KEYFRAME>& Keyframes, _int iBoneIndex)
{
	strcpy_s(m_szName, strName.c_str());

	m_KeyFrames.reserve(Keyframes.size());
	for (auto& iter : Keyframes)
		m_KeyFrames.push_back(iter);

	m_iNumKeyFrames = m_KeyFrames.size();

	m_iBoneIndex = iBoneIndex;
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

CChannel* CChannel::Create(const string strName, vector<KEYFRAME>& Keyframes, _int iBoneIndex)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(strName, Keyframes, iBoneIndex)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
}
