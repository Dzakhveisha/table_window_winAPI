#include <windows.h>
#include <tchar.h>
#include <string>
#include <wingdi.h>
#include <vector>
#include <fstream>
#include <regex>
#include <algorithm>

using namespace std;

#define _CRT_SECURE_NO_WARNINGS

// string const
static const TCHAR szWindowClass[] = _T("DesktopAppClass");
static const TCHAR szTitle[] = _T("Dynamic table Window");

// brush for window
const HBRUSH BACKGROUND_BRUSH = CreateSolidBrush(RGB(255, 255, 255));
// window class
WNDCLASSEX wcex;

static vector< vector<string> > textVector; // vector of tables strings
static int rowCount;
static int colCount;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void loadTextFromFile();

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow)
{
    HWND hWnd; MSG msg;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_DBLCLKS;
    wcex.lpfnWndProc = WndProc; // function foe msg handling
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = BACKGROUND_BRUSH;
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = wcex.hIcon;

    RegisterClassEx(&wcex); // register WNDCLASSEX

    hWnd = CreateWindow(szWindowClass, szTitle,  // window descriptor
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
        CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    //loop to listen for the messages that Windows sends
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int cxClient, cyClient;

    switch (message)
    {
  
    case WM_CREATE: 
        loadTextFromFile();
        break;

    case WM_SIZE:
    {
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        InvalidateRect(hWnd, NULL, TRUE);
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        int cellWidth = cxClient / colCount;

        TEXTMETRIC textMetric;
        GetTextMetrics(hdc, &textMetric);
        int minCellHeight =  textMetric.tmHeight;     // todo: depends from text length

        int tableHeight = 0;

        for (int i = 0; i < textVector.size(); i++) {
            int curCellHeight = minCellHeight;
            for (int j = 0; j < textVector[i].size(); j++) {
                wstring text = wstring(textVector[i][j].begin(), textVector[i][j].end());   //vector<wchar_t>
                RECT rect = { j * cellWidth, tableHeight + minCellHeight / 5, (j + 1) * cellWidth , cyClient };
                int tempHeight = DrawText(
                    hdc,
                    text.c_str(), //const char* for drawing
                    text.size(),       // text size
                    &rect,    // rect for drawing (cell)
                    DT_CENTER | DT_WORDBREAK | DT_END_ELLIPSIS     // output parameters
                );
                if (tempHeight > curCellHeight) {
                    curCellHeight = tempHeight;
                }
            }
            tableHeight += curCellHeight + minCellHeight / 5;
            MoveToEx(hdc, 0, tableHeight, nullptr);
            LineTo(hdc, cxClient, tableHeight);
        }
        
        for (int j = 0; j < colCount; j++) {
            MoveToEx(hdc, j * cellWidth, 0, nullptr);
            LineTo(hdc, j * cellWidth, tableHeight);
        }

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int maxVectorSize(vector<vector<string>> vector) {
    int max = 0;
    for (int i = 0; i < vector.size(); i++)
        if (vector[i].size() > max) {
            max = vector[i].size();
        }
    return max;
}

void loadTextFromFile(){
        ifstream infile("tableText.txt");
        string line;
        while (std::getline(infile, line)) {
            std::regex regex{ R"([,][\s]+)" };
            std::sregex_token_iterator it{ line.begin(), line.end(), regex, -1 };
            textVector.emplace_back(vector<string>{it, {}});
        }
        rowCount = textVector.size();
        colCount = maxVectorSize(textVector);
}