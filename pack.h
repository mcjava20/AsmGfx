#ifndef PACK_H
#define PACK_H

#define RUNTIME_TEMPLATE \
"#include <windows.h>\n" \
"#include <stdlib.h>\n\n" \
"float vtx[100][2];\n" \
"int edges[100][2];\n" \
"int vc = 0;\n" \
"int ecnt = 0;\n" \
"int win_w = 600;\n" \
"int win_h = 600;\n" \
"char title[256] = \"Hello\";\n" \
"int shape_size = 100;\n" \
"int pen_size = 2;\n" \
"COLORREF line_color = RGB(0,255,0);\n" \
"HWND g_hwnd;\n" \
"HDC g_hdc;\n\n" \
"#define VAR_MAX 30\n" \
"int var_val[VAR_MAX];\n" \
"char var_name[VAR_MAX][16];\n" \
"int var_cnt = 0;\n\n" \
"void __data_init();\n" \
"void vm_run(void);\n" \
"int  vm_get_var(const char *n);\n" \
"void vm_set_var(const char *n, int v);\n" \
"void vm_add(const char *a, const char *b);\n" \
"void vm_sub(const char *a, const char *b);\n" \
"void vm_mul(const char *a, const char *b);\n" \
"void vm_div(const char *a, const char *b);\n" \
"void vm_process_messages();\n\n" \
"void vm_process_messages() {\n" \
"    MSG msg;\n" \
"    while(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {\n" \
"        TranslateMessage(&msg);\n" \
"        DispatchMessageA(&msg);\n" \
"    }\n" \
"}\n\n" \
"LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)\n" \
"{\n" \
"    switch (msg)\n" \
"    {\n" \
"        case WM_DESTROY:\n" \
"            PostQuitMessage(0);\n" \
"            exit(0);\n" \
"            break;\n" \
"        default:\n" \
"            return DefWindowProc(hWnd, msg, wp, lp);\n" \
"    }\n" \
"    return 0;\n" \
"}\n\n" \
"void vm_draw_line(int idx)\n" \
"{\n" \
"    if (idx >= ecnt) return;\n" \
"    int a = edges[idx][0];\n" \
"    int b = edges[idx][1];\n" \
"    RECT rc;\n" \
"    GetClientRect(g_hwnd, &rc);\n" \
"    int cx = rc.right / 2;\n" \
"    int cy = rc.bottom / 2;\n" \
"    int x1 = cx + (int)(vtx[a][0] * shape_size);\n" \
"    int y1 = cy + (int)(vtx[a][1] * shape_size);\n" \
"    int x2 = cx + (int)(vtx[b][0] * shape_size);\n" \
"    int y2 = cy + (int)(vtx[b][1] * shape_size);\n" \
"    HPEN pen = CreatePen(PS_SOLID, pen_size, line_color);\n" \
"    SelectObject(g_hdc, pen);\n" \
"    MoveToEx(g_hdc, x1, y1, NULL);\n" \
"    LineTo(g_hdc, x2, y2);\n" \
"    DeleteObject(pen);\n" \
"}\n\n" \
"void vm_sleep(int sec)\n" \
"{\n" \
"    if(sec <= 0) sec = 1;\n" \
"    int total = sec * 1000;\n" \
"    for(int i=0;i<total;i+=10) {\n" \
"        vm_process_messages();\n" \
"        Sleep(10);\n" \
"    }\n" \
"}\n\n" \
"void vm_clear()\n" \
"{\n" \
"    RECT rc;\n" \
"    GetClientRect(g_hwnd, &rc);\n" \
"    FillRect(g_hdc, &rc, GetStockObject(BLACK_BRUSH));\n" \
"}\n\n" \
"int main()\n" \
"{\n" \
"    __data_init();\n" \
"    WNDCLASS wc = {0};\n" \
"    wc.style         = CS_HREDRAW | CS_VREDRAW;\n" \
"    wc.lpfnWndProc   = WndProc;\n" \
"    wc.hInstance     = GetModuleHandle(NULL);\n" \
"    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);\n" \
"    wc.lpszClassName = \"VMRENDER\";\n" \
"    RegisterClass(&wc);\n" \
"    g_hwnd = CreateWindowEx(0, \"VMRENDER\", title, WS_OVERLAPPEDWINDOW, 100, 100, win_w, win_h, NULL, NULL, GetModuleHandle(NULL), NULL);\n" \
"    ShowWindow(g_hwnd, SW_SHOW);\n" \
"    UpdateWindow(g_hwnd);\n" \
"    g_hdc = GetDC(g_hwnd);\n" \
"    vm_run();\n" \
"    return 0;\n" \
"}\n"

#endif