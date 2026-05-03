#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pack.h"

typedef struct {
    int w, h;
    char title[256];
    char color[32];
    int size;
    int sizepen;
} WinInfo;

typedef struct { char name[16]; float x, y; } Vtx;
typedef struct { char a[16], b[16]; } Edg;

extern int vc, ecnt;
extern WinInfo win;
extern Vtx vtx[];
extern Edg edges[];
extern char instructions[4096];

int find_vtx(const char* name);

void build_exe(const char* out) {
    remove("runner.c");
    FILE* f = fopen("runner.c", "w");
    fputs(RUNTIME_TEMPLATE, f);

    fprintf(f, "void __data_init() {\n");
    fprintf(f, "    win_w = %d;\n", win.w);
    fprintf(f, "    win_h = %d;\n", win.h);
    fprintf(f, "    strcpy(title, \"%s\");\n", win.title);
    fprintf(f, "    shape_size = %d;\n", win.size);
    fprintf(f, "    pen_size = %d;\n", win.sizepen);

    if (strcmp(win.color, "red") == 0)
        fprintf(f, "    line_color = RGB(255,0,0);\n");
    else if (strcmp(win.color, "green") == 0)
        fprintf(f, "    line_color = RGB(0,255,0);\n");
    else if (strcmp(win.color, "blue") == 0)
        fprintf(f, "    line_color = RGB(0,0,255);\n");
    else if (strcmp(win.color, "white") == 0)
        fprintf(f, "    line_color = RGB(255,255,255);\n");
    else if (strcmp(win.color, "yellow") == 0)
        fprintf(f, "    line_color = RGB(255,255,0);\n");
    else
        fprintf(f, "    line_color = RGB(0,255,0);\n");

    fprintf(f, "    vc = %d;\n", vc);
    fprintf(f, "    ecnt = %d;\n", ecnt);

    for (int i = 0; i < vc; i++) {
        fprintf(f, "    vtx[%d][0] = %.2ff;\n", i, vtx[i].x);
        fprintf(f, "    vtx[%d][1] = %.2ff;\n", i, vtx[i].y);
    }
    for (int i = 0; i < ecnt; i++) {
        int a = find_vtx(edges[i].a);
        int b = find_vtx(edges[i].b);
        fprintf(f, "    edges[%d][0] = %d;\n", i, a);
        fprintf(f, "    edges[%d][1] = %d;\n", i, b);
    }
    fprintf(f, "}\n\n");

    fprintf(f,"int vm_get_var(const char *n){\n");
    fprintf(f,"for(int i=0;i<var_cnt;i++)if(strcmp(var_name[i],n)==0)return var_val[i];\n");
    fprintf(f,"return 0;}\n");

    fprintf(f,"void vm_set_var(const char *n, int v){\n");
    fprintf(f,"int i;for(i=0;i<var_cnt;i++)if(strcmp(var_name[i],n)==0){var_val[i]=v;return;}\n");
    fprintf(f,"if(var_cnt<VAR_MAX){strcpy(var_name[var_cnt],n);var_val[var_cnt]=v;var_cnt++;}\n");
    fprintf(f,"}\n");

    fprintf(f,"void vm_add(const char *a, const char *b){vm_set_var(a, vm_get_var(a)+vm_get_var(b));}\n");
    fprintf(f,"void vm_sub(const char *a, const char *b){vm_set_var(a, vm_get_var(a)-vm_get_var(b));}\n");
    fprintf(f,"void vm_mul(const char *a, const char *b){vm_set_var(a, vm_get_var(a)*vm_get_var(b));}\n");
    fprintf(f,"void vm_div(const char *a, const char *b){int vb=vm_get_var(b);if(vb!=0)vm_set_var(a, vm_get_var(a)/vb);}\n\n");

    fprintf(f, "void vm_run(void) {\n");
    fprintf(f, "    RECT rc; GetClientRect(g_hwnd, &rc);\n");
    fprintf(f, "    FillRect(g_hdc, &rc, GetStockObject(BLACK_BRUSH));\n");

    for (int i = 1; i <= 100; i++) {
        fprintf(f, "    label_%d:\n", i);
    }

    fprintf(f, "%s", instructions);
    fprintf(f, "}\n");

    fclose(f);

    // ========== 这里修复成 gcc ==========
    char cmd[512];
    sprintf(cmd, "x86_64-win32-tcc.exe runner.c -o %s -luser32 -lgdi32", out);
    system(cmd);

    remove("runner.c");
    printf("✅ Compilation successful!\n");
}