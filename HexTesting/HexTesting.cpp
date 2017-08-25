#include "stdafx.h"
#include "HexTesting.h"
#include "Hexagon.h"
#include <time.h>
HINSTANCE hInst;
WCHAR szTitle[100];
WCHAR szWindowClass[100];
ATOM MyRegisterClass(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	Gdiplus::GdiplusStartupInput input;
	ULONG_PTR token;
	Gdiplus::GdiplusStartup(&token, &input, nullptr);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, 100);
    LoadStringW(hInstance, IDC_HEXTESTING, szWindowClass, 100);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow)) return FALSE;
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HEXTESTING));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
	Gdiplus::GdiplusShutdown(token);
    return (int) msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HEXTESTING));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HEXTESTING);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
   hInst = hInstance;
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd) return FALSE;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}

using Gdiplus::Bitmap;
using Gdiplus::Graphics;
using Gdiplus::Color;
using Gdiplus::Pen;
using Gdiplus::Brush;
using Gdiplus::SolidBrush;

Bitmap* b;
Graphics* g;
Layout* layout;
std::vector<Hex> map;
std::vector<int> colors;
Brush* brushes[15];
Pen* pen;
POINTS mousecoords;

void Draw() {
	for (int i = 0; i < int(map.size()); ++i) {
		g->FillPolygon(brushes[colors[i]], PolygonCorners(*layout, map[i]).data(), 6);
		g->DrawPolygon(pen, PolygonCorners(*layout, map[i]).data(), 6);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
	case WM_CREATE:
		{
			srand(unsigned(time(nullptr)));
			b = new Bitmap(512, 512);
			g = Graphics::FromImage(b);
			layout = new Layout{ flat, PointF(20.0, 20.0), PointF(256.0, 256.0) };

			for (int q = -6; q <= 6; q++) {
				int r1 = max(-6, -q - 6);
				int r2 = min(6, -q + 6);
				for (int r = r1; r <= r2; r++) {
					map.push_back(Hex(q, r));
					colors.push_back(rand() % 4);
				}
			}

			brushes[0] = new SolidBrush(Color::LightGray);
			brushes[1] = new SolidBrush(Color::Red);
			brushes[2] = new SolidBrush(Color::RoyalBlue);
			brushes[3] = new SolidBrush(Color::SlateGray);
			pen = new Pen(Color::Black);
			Draw();

			DeleteMenu(GetMenu(hWnd), 1, MF_BYPOSITION);
		}
		break;
	case WM_TIMER:
		{

		}
		break;
	case WM_LBUTTONDOWN:
		{
			
		}
		break;
	case WM_MOUSEMOVE:
		{
			mousecoords = MAKEPOINTS(lParam);
			SolidBrush brush(0x80FFFFFF);
			Draw();
			g->FillPolygon(&brush, PolygonCorners(*layout, HexRound(PixelToHex(*layout, PointF(mousecoords.x, mousecoords.y)))).data(), 6);
			InvalidateRect(hWnd, nullptr, false);
		}
		break;
	case WM_KEYDOWN:
		{
			
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			Graphics graphics(hdc);
			graphics.DrawImage(b, 0, 0);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		{
			delete(layout);
			for (int i = 0; i < 4; ++i)
				delete(brushes[i]);
			delete(pen);
			PostQuitMessage(0);
		}
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
