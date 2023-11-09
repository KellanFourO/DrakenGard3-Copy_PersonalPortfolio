#ifndef Engine_Struct_h__
#define Engine_Struct_h__

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

	typedef struct tagVertex_Position_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;
	}VTXPOSTEX;
}



#endif // Engine_Struct_h__
