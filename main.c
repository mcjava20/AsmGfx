#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int w, h;
    char title[256];
    char color[32];
    int size;
    int sizepen;
} WinInfo;

typedef struct {
    char name[16];
    float x, y;
} Vtx;

typedef struct {
    char a[16], b[16];
} Edg;

WinInfo win = {
    600, 600,
    "Hello",
    "green",
    100,
    2
};

Vtx vtx[100];
Edg edges[100];
int vc = 0, ecnt = 0;
char instructions[4096] = {0};

int find_vtx(const char* name) {
    for (int i = 0; i < vc; i++) {
        if (!strcmp(vtx[i].name, name))
            return i;
    }
    return -1;
}

int parse_asm(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) return 0;

    char buf[512], tok1[32], tok2[32];
    int mode = 0;

    while (fgets(buf, 512, f)) {
        char* p = buf;
        while (*p == ' ' || *p == '\t') p++;
        if (!*p || *p == ';') continue;

        if (strstr(p, ".window:"))              { mode = 1; continue; }
        if (strstr(p, ".VertexDataSegment:"))   { mode = 2; continue; }
        if (strstr(p, ".DataSegment:"))         { mode = 3; continue; }
        if (strstr(p, ".InstructionSegment:"))  { mode = 4; continue; }

        if (mode == 1) {
            sscanf(p, "INIT_W %d", &win.w);
            sscanf(p, "INIT_H %d", &win.h);
            sscanf(p, "TEXT db \"%[^\"]", win.title);
            sscanf(p, "color db \"%[^\"]", win.color);
            sscanf(p, "size %d", &win.size);
            sscanf(p, "sizepen %d", &win.sizepen);
        }
        if (mode == 2) {
            float x,y;
            sscanf(p, "%s %f %f", tok1, &x, &y);
            strcpy(vtx[vc].name, tok1);
            vtx[vc].x = x; vtx[vc].y = y;
            vc++;
        }
        if (mode == 3) {
            sscanf(p, "%s %s", tok1, tok2);
            strcpy(edges[ecnt].a, tok1);
            strcpy(edges[ecnt].b, tok2);
            ecnt++;
        }
        if (mode == 4) {
            if (strstr(p, "var ")) {
                char v[32]; int n;
                sscanf(p, "var %s=%d", v, &n);
                char tmp[128];
                sprintf(tmp, "vm_set_var(\"%s\",%d);\n", v, n);
                strcat(instructions, tmp);
            }
            else if (strstr(p, "set ")) {
                char v[32]; int n;
                sscanf(p, "set %s=%d", v, &n);
                char tmp[128];
                sprintf(tmp, "vm_set_var(\"%s\",%d);\n", v, n);
                strcat(instructions, tmp);
            }
            else if (strstr(p, "add ")) {
                char a[32],b[32];
                sscanf(p, "add %s,%s", a, b);
                char tmp[128];
                sprintf(tmp, "vm_add(\"%s\",\"%s\");\n", a, b);
                strcat(instructions, tmp);
            }
            else if (strstr(p, "sub ")) {
                char a[32],b[32];
                sscanf(p, "sub %s,%s", a, b);
                char tmp[128];
                sprintf(tmp, "vm_sub(\"%s\",\"%s\");\n", a, b);
                strcat(instructions, tmp);
            }
            else if (strstr(p, "mul ")) {
                char a[32],b[32];
                sscanf(p, "mul %s,%s", a, b);
                char tmp[128];
                sprintf(tmp, "vm_mul(\"%s\",\"%s\");\n", a, b);
                strcat(instructions, tmp);
            }
            else if (strstr(p, "div ")) {
                char a[32],b[32];
                sscanf(p, "div %s,%s", a, b);
                char tmp[128];
                sprintf(tmp, "vm_div(\"%s\",\"%s\");\n", a, b);
                strcat(instructions, tmp);
            }
            else if (strstr(p, "call sleep,")) {
                char v[32];
                if(sscanf(p, "call sleep,%s", v) == 1){
                    if(v[0]>='a'&&v[0]<='z'){
                        char tmp[128];
                        sprintf(tmp, "vm_sleep(vm_get_var(\"%s\"));\n", v);
                        strcat(instructions, tmp);
                    }else{
                        int sec=atoi(v);
                        char tmp[64];
                        sprintf(tmp, "vm_sleep(%d);\n", sec);
                        strcat(instructions, tmp);
                    }
                }
            }
            else if (strstr(p, "call clear")) {
                strcat(instructions, "vm_clear();\n");
            }
            else if (strstr(p, "mov data,")) {
                int n;
                sscanf(p, "mov data, %d", &n);
                char tmp[128];
                sprintf(tmp, "vm_draw_line(%d);\n", n-1);
                strcat(instructions, tmp);
            }
            else if (strstr(p, "jmp ")) {
                int line;
                sscanf(p, "jmp %d", &line);
                char tmp[64];
                sprintf(tmp, "goto label_%d;\n", line);
                strcat(instructions, tmp);
            }
        }
    }
    fclose(f);
    return 1;
}

extern void build_exe(const char*);

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(65001);
    if (argc == 4 && !strcmp(argv[2], "-o")) {
        instructions[0] = 0;
        parse_asm(argv[1]);
        build_exe(argv[3]);
        return 0;
    }
    printf("Compiler usage: AsmGfx.exe script.asm -o output.exe");
    return 0;
}