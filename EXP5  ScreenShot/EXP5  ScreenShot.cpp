// EXP5  ScreenShot.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "EXP5  ScreenShot.h"
#include <stdio.h>
#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������
HDC storageDC = NULL;//�������ڱ����ͼ
RECT rectOfShot = RECT();//ȫ�ֱ���,���������ͼ������λ��


// �˴���ģ���а����ĺ�����ǰ������: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EXP5SCREENSHOT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EXP5SCREENSHOT));

	// ����Ϣѭ��: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXP5SCREENSHOT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_EXP5SCREENSHOT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_HSCROLL|WS_VSCROLL|WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT,300, 200, NULL, NULL, hInstance, NULL);
   
   
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	static PAINTSTRUCT ps;
	static HDC hdc;
	static RECT myClientRect=RECT();//�ͻ�����С
	static HDC tmpDC =NULL;//���������ȡ�����ŵ�ͼ����ʾ
	static  HDC shotDC = NULL;//Ҫ��ͼ�����dc
	static bool flag4Stretch = true;//�Ƿ�Ҫ����
	static  HBITMAP hBitMap =NULL;
	static int   cyClient,cxClient, iVscrollPos,iHscrollPos;
	static bool flag4Anyarea = false;

	switch (message)
	{
	case WM_CREATE:

		break;
	case WM_SIZE:
		cyClient = HIWORD(lParam);
		cxClient = LOWORD(lParam);
		//��ز����ƶ���WM_PAINT

		SetScrollRange(hWnd, SB_VERT, 0, max(rectOfShot.bottom - rectOfShot.top - cyClient, 0), TRUE);
		SetScrollRange(hWnd, SB_HORZ, 0, max(rectOfShot.right - rectOfShot.left - cxClient, 0), TRUE);
		break;
	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			iVscrollPos -= 15;
			break;

		case SB_LINEDOWN:
			iVscrollPos += 15;
			break;

		case SB_PAGEUP:
			iVscrollPos -= myClientRect.bottom-myClientRect.top;
			break;

		case SB_PAGEDOWN:
			iVscrollPos += myClientRect.bottom-myClientRect.top;
			break;

		case SB_THUMBPOSITION:
			iVscrollPos = HIWORD(wParam);
			//InvalidateRect(hWnd, NULL, TRUE);
			//SendMessage(hWnd, WM_PAINT, 1, 1);
			break;

		default:
			break;
		}
		//��֤��������������Ļ
		iVscrollPos = max(0, min(iVscrollPos, rectOfShot.bottom-rectOfShot.top -myClientRect.bottom+myClientRect.top ));

		if (iVscrollPos != GetScrollPos(hWnd, SB_VERT))
		{
			SetScrollPos(hWnd, SB_VERT, iVscrollPos, TRUE);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_HSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			iHscrollPos -= 15;
			break;

		case SB_LINEDOWN:
			iHscrollPos += 15;
			break;

		case SB_PAGEUP:
			iHscrollPos -= myClientRect.right-myClientRect.left;
			break;

		case SB_PAGEDOWN:
			iHscrollPos += myClientRect.right-myClientRect.left;
			break;

		case SB_THUMBPOSITION:
			iHscrollPos = HIWORD(wParam);
			break;

		default:
			break;
		}
		//��֤��������������Ļ
		iHscrollPos = max(0, min(iHscrollPos, rectOfShot.right - rectOfShot.left - myClientRect.right + myClientRect.left));

		if (iHscrollPos != GetScrollPos(hWnd, SB_HORZ))
		{
			SetScrollPos(hWnd, SB_HORZ, iHscrollPos, TRUE);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��: 
		switch (wmId)
		{
		case IDM_ABOUT:
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_RAW:
			if (flag4Stretch)
			{
				InvalidateRect(hWnd, NULL, TRUE);//�������ģʽ����,����Ҫ��Ч���ͻ���
				flag4Stretch = false;
				SendMessage(hWnd, WM_PAINT, 1, 0);
			}
			//GetClientRect(GetDesktopWindow(), &myClientRect);

			//InvalidateRect(hWnd, &myClientRect, TRUE);//ϵͳ�Զ��趨��Ч����,��Ҫ��Ԥ
			break;
		case IDM_STRETCH:
			if (!flag4Stretch)
			{
				InvalidateRect(hWnd, NULL, TRUE);//ͬ������
				flag4Stretch = true;//�ı�flag�ڴ�����ͼ
				SendMessage(hWnd, WM_PAINT, 1, 0);
			}
			//GetClientRect(hWnd, &myClientRect);//ϵͳ���Զ�������Ч����,�����Ԥ
			//InvalidateRect(hWnd, &myClientRect, TRUE);
			break;
		case IDM_FULLSCREEN:
			shotDC = GetDC(GetDesktopWindow());
			GetClientRect(GetDesktopWindow(), &rectOfShot);

			storageDC = CreateCompatibleDC(shotDC);//�õ���ͼĿ��ļ���dc

			//hBitMap��ʱû������,ֻ�Ǹ���rectOfShot�Ĵ�С������һ��������DC���ݵ�λͼ 
			hBitMap = (HBITMAP)CreateCompatibleBitmap(shotDC, rectOfShot.right, rectOfShot.bottom);//�����������λͼ(Ҳ������Ļ��ʾ����),storageDC�Ͳ���,��ΪstorageDC���ڴ滷��,������Ļ��ʾ
			SelectObject(storageDC, hBitMap);
			
			BitBlt(storageDC, 0, 0, rectOfShot.right, rectOfShot.bottom,shotDC,0,0,SRCCOPY);//�����ͼ��storage��,��ʱstorageDC�в�������
			InvalidateRect(hWnd, NULL, TRUE);
			SendMessage(hWnd, WM_SIZE, 1, 0);
			ReleaseDC(GetDesktopWindow(), shotDC);
			if (!flag4Anyarea)
			{
				if (OpenClipboard(NULL))
				{
					EmptyClipboard();
					HDC clipedDC = NULL;//�����ݴ�Ҫ������������Ľ�ͼ
					clipedDC = CreateCompatibleDC(storageDC);
					HBITMAP clipedBM = (HBITMAP)CreateCompatibleBitmap(storageDC, rectOfShot.right - rectOfShot.left,
						rectOfShot.bottom - rectOfShot.top);//������ͼ����λͼ.��ʱ���λͼ�Ǻڵģ�ֻ�ǽ�˿�����DC�Ĵ�С
					SelectObject(clipedDC, clipedBM);
					BitBlt(clipedDC, 0, 0, rectOfShot.right - rectOfShot.left, rectOfShot.bottom - rectOfShot.top,
						storageDC, rectOfShot.left, rectOfShot.top, SRCCOPY);//����ѡ����clipedDC
					SetClipboardData(CF_BITMAP, clipedBM);
					CloseClipboard();
					DeleteDC(clipedDC);
					DeleteObject(clipedBM);

				}
			}
			flag4Anyarea = false;
			SetScrollRange(hWnd, SB_VERT, 0, max(0, min(iVscrollPos, rectOfShot.bottom - rectOfShot.top - myClientRect.bottom + myClientRect.top)), TRUE);
			//������䲻֪Ϊ��ʹ�ú��������ʧ��.
			//SetScrollRange(hWnd, SB_HORZ, 0, max(0, min(iHscrollPos, rectOfShot.right - rectOfShot.left - myClientRect.right + myClientRect.left)), TRUE);

			break;
		case IDM_ANYAREA:
			flag4Anyarea = true;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_SYSKEYDOWN:
		switch (wParam)
		{
		case 0X50:
			if (GetKeyState(VK_MENU) & 0X8000)
				PostMessage(hWnd, WM_COMMAND, IDM_FULLSCREEN, 1);
			break;
		case 0X73:
			if (GetKeyState(VK_MENU) & 0X8000)
				PostMessage(hWnd, WM_DESTROY, 1, 1);
		case 0X41:
			if (GetKeyState(VK_MENU) & 0X8000)
				PostMessage(hWnd, WM_COMMAND, IDM_ANYAREA, 1);
			break;
		default:
			break;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if (flag4Stretch)
		{
			//SelectObject(tmpDC, hBitMap);
			GetClientRect(hWnd, &myClientRect);
			//StretchBlt(tmpDC, myClientRect.left, myClientRect.top, myClientRect.right, myClientRect.bottom,
				//storageDC, rectOfShot.left, rectOfShot.top, rectOfShot.right, rectOfShot.bottom, SRCCOPY);
			StretchBlt(hdc, myClientRect.left, myClientRect.top, myClientRect.right-myClientRect.left, myClientRect.bottom-myClientRect.top,
				storageDC, rectOfShot.left, rectOfShot.top, rectOfShot.right-rectOfShot.left, rectOfShot.bottom-rectOfShot.top, SRCCOPY);
			//BitBlt(hdc, 0, 0, myClientRect.right, myClientRect.bottom, tmpDC, 0, 0, SRCCOPY);
		}
		else
		{
			GetClientRect(hWnd, &myClientRect);
			BitBlt(hdc, 0, 0, rectOfShot.right-rectOfShot.left, rectOfShot.bottom-rectOfShot.top, storageDC, rectOfShot.left+iHscrollPos,rectOfShot.top+iVscrollPos, SRCCOPY);
		}
		EndPaint(hWnd, &ps);

		break;
	case WM_DESTROY:
		DeleteDC(shotDC);
		DeleteDC(tmpDC);
		DeleteDC(storageDC);
		DeleteObject(hBitMap);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static RECT rectOfScreen = RECT();
	static bool flag4LbuttonDown = false;
	static HBRUSH hBrush4Dlg;
	static HPEN hPen4Dlg;
	HDC dlgDC;
	static int startX, startY, endX, endY;
	static RECT rectOfCliped=RECT();
	PAINTSTRUCT ps;
	char showbuf[64] = "";


	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_PAINT:
		dlgDC = BeginPaint(hDlg, &ps);
		BitBlt(dlgDC, 0, 0, rectOfScreen.right, rectOfScreen.bottom
			, storageDC, 0, 0, SRCCOPY);
		EndPaint(hDlg, &ps);
		break;
	case WM_INITDIALOG:
		hPen4Dlg = (HPEN)(CreatePen(PS_DASHDOT, 5, RGB(28, 68, 201)));
		dlgDC = GetDC(hDlg);
		hBrush4Dlg = (HBRUSH)GetStockObject(NULL_BRUSH);//CreateSolidBrush(RGB(30, 30, 30));
		SetWindowLong(hDlg, GWL_STYLE, WS_POPUP);
	
		//��dialog����ȫ��
		GetWindowRect(GetDesktopWindow(), &rectOfScreen);
		SendMessage(GetParent(hDlg), WM_COMMAND, IDM_FULLSCREEN, 1);//��ȡȫ��
		SetWindowPos(hDlg, HWND_TOP, 0, 0,
			rectOfScreen.right,rectOfScreen.bottom,0);
			
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC3), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC1), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDOK), SW_HIDE);
		ReleaseDC(hDlg, dlgDC);
		return (INT_PTR)TRUE;

	case WM_RBUTTONDOWN:
		SendMessage(hDlg, WM_COMMAND, IDCANCEL, 1);
		break; 
	case WM_LBUTTONDBLCLK:
		//����ĵ��ھ�����
		if ((LOWORD(lParam) > min(startX, endX)) && (LOWORD(lParam) < max(startX, endX)) &&
			(HIWORD(lParam) > min(startY, endY)) && (HIWORD(lParam) < max(startY, endY)))
		{

			if (OpenClipboard(NULL))
			{
				EmptyClipboard();

				HDC clipedDC = NULL;//�����ݴ�Ҫ������������Ľ�ͼ
				clipedDC = CreateCompatibleDC(storageDC);
				HBITMAP clipedBM= (HBITMAP)CreateCompatibleBitmap(storageDC, rectOfCliped.right-rectOfCliped.left,
					rectOfCliped.bottom-rectOfCliped.top);//������ͼ����λͼ.��ʱ���λͼ�Ǻڵģ�ֻ�ǽ�˿�����DC�Ĵ�С
				SelectObject(clipedDC, clipedBM);
				BitBlt(clipedDC, 0, 0, rectOfCliped.right-rectOfCliped.left, rectOfCliped.bottom-rectOfCliped.top, 
					storageDC, rectOfCliped.left, rectOfCliped.top, SRCCOPY);//����ѡ����clipedDC

			//	HDC storageDCBackUp = NULL;//��storage�滻�ɽ�ȡ��ͼ
				//storageDCBackUp = CreateCompatibleDC(storageDC);
				
				/*HBITMAP clipedBM2=(HBITMAP)CreateCompatibleBitmap(storageDC, rectOfCliped.right - rectOfCliped.left, 
					rectOfCliped.bottom - rectOfCliped.top);//������ͼ����λͼ.��ʱ���λͼ�Ǻڵģ�ֻ�ǽ�˿�����DC�Ĵ�С
				DeleteDC(storageDC);
				storageDC = CreateCompatibleDC(clipedDC);
				SelectObject(storageDC, clipedBM2);
				BitBlt(storageDC, 0, 0, rectOfCliped.right - rectOfCliped.left, rectOfCliped.bottom - rectOfCliped.top,
					clipedDC,0,0, SRCCOPY);//�����µĽ�ͼ��storageDC
					*/
				rectOfShot = rectOfCliped;
				SetClipboardData(CF_BITMAP, clipedBM);
				CloseClipboard();

				//�ػ�������,��ʾ�µĽ�ͼ
				InvalidateRect(GetParent(hDlg), NULL, TRUE);
				PostMessage(GetParent(hDlg), WM_SIZE, 1, 0);
				//if (storageDC != NULL)
			//		DeleteDC(storageDC);
			//	storageDC = CreateCompatibleDC(clipedDC);//�õ���ͼĿ��ļ���dc
				//����Ϊ��ͨ��hbitmap�Ĵ�С��������ˡ�storageDC�Ĵ�СΪ ����ͼ�����С
				//SelectObject(storageDC, clipedBM);
				//BitBlt(storageDC, 0, 0, rectOfCliped.right, rectOfCliped.bottom, clipedDC, 0, 0, SRCCOPY);
				
				startX = startY = endX = endY = 0;//�����㣬��ֹ�´ν�ͼ���ʱ���Ȼ��������
				DeleteObject(clipedBM);
				DeleteDC(clipedDC);
				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 1);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		//����ĵ��ھ�����
		if ((LOWORD(lParam) > min(startX, endX)) && (LOWORD(lParam) < max(startX, endX)) &&
			(HIWORD(lParam) > min(startY, endY)) && (HIWORD(lParam) < max(startY, endY)))
		{

		}
		else
		{	
			
			dlgDC= GetDC(hDlg);
			SetROP2(dlgDC, R2_XORPEN);
			SelectObject(dlgDC, hPen4Dlg);//ѡ��mousemoveͬһ������ʵ�ַ�ת,����߿�����ºۼ�
			SelectObject(dlgDC, hBrush4Dlg);//ѡ���mousemove��ͬһ����ˢ��ʵ�ַ�ת
			Rectangle(dlgDC, startX, startY, endX, endY);//��Ϊ�Ƿ�תģʽ������ͼ��ԭ
			ReleaseDC(hDlg, dlgDC);
			flag4LbuttonDown = true;
			InvalidateRect(hDlg, NULL, TRUE);
			SendMessage(hDlg, WM_PAINT, 1, 1);
			startX = LOWORD(lParam);
			startY = HIWORD(lParam);
			endX = startX;
			endY = startY;

			//rectOfCliped.left = startX;
			//rectOfCliped.right = endX;
		//	rectOfCliped.top = startY;
		//	rectOfCliped.bottom = endY;

		}
		break;
	case WM_LBUTTONUP:
		flag4LbuttonDown = false;
		
		break;
	case WM_MOUSEMOVE:
		dlgDC = GetDC(hDlg);
		SelectObject(dlgDC, hBrush4Dlg);//��ˢ���߶���ѡ��û��ϵ��û����˸
		SelectObject(dlgDC, hPen4Dlg);//��֪Ϊ�β�����ѡ�񻭱ʾͻ���˸
		//��һ������ȡ��   ����ȡ�����ָܻ�ͼ��֮ǰ����
		SetROP2(dlgDC, R2_XORPEN);
		if (flag4LbuttonDown)
		{
			
			Rectangle(dlgDC, startX, startY, endX, endY);
			endX = LOWORD(lParam);
			endY = HIWORD(lParam);
			
			Rectangle(dlgDC, startX, startY, endX, endY);
			sprintf_s(showbuf, "��ǰ��������(%d,%d)<>(%d,%d) ��СΪ(%d,%d)", startX,startY,endX, endY, abs(endX - startX), abs(endY - startY));
			SetBkColor(dlgDC, RGB(0,0,0));
			SetTextColor(dlgDC, RGB(255,255,255));
			TextOutA(dlgDC, 0,0, showbuf, lstrlenA(showbuf));
			
			rectOfCliped.left = min(startX,endX);
			rectOfCliped.right = max(startX,endX);
			rectOfCliped.top = min(startY,endY);
			rectOfCliped.bottom = max(endY,startY);
		}
		ReleaseDC(hDlg, dlgDC);

		break;


	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			DeleteObject(hBrush4Dlg);
			DeleteObject(hPen4Dlg);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
