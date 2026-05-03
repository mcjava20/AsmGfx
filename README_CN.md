# 汇编式显卡渲染语言 完整文档

# 一、文档简介

本文档详细介绍自研轻量汇编式显卡渲染语言的使用方法、核心特性、脚本语法、坐标系规则及常见操作，适用于快速上手虚拟机开发图形、动画脚本，全程无编译警告、窗口操作不崩溃，支持变量运算、循环跳转等核心功能。

# 二、编译环境与项目结构

## 2\.1 编译环境

- 编译器：GCC（推荐，已适配，无TCC依赖）

- 依赖库：user32（窗口控制）、gdi32（图形绘制）

- 运行平台：Windows 系统

- 无额外环境配置，直接编译即可使用

## 2\.2 项目文件结构

```plain text
├── main.c      // 核心：汇编脚本解析器（解析.asm脚本指令、变量、坐标等）
├── build.c     // 辅助：代码生成 + 编译驱动（生成运行时C代码，调用GCC编译）
├── pack.h      // 底层：虚拟机运行时模板（窗口创建、绘图、变量、消息处理等）
├── xxx.asm     // 自定义：图形/动画汇编脚本（用户编写，实现具体图形效果）
├── main.exe    // 最终编译器（编译.asm脚本生成可执行图形程序）
└── README.md   // 本文档（完整使用说明）
```

# 三、编译器使用方法

## 3\.1 编译编译器本体

打开CMD/PowerShell，进入项目目录，执行以下命令，生成编译器main\.exe：

```bash
gcc main.c build.c -o AsmGfx.exe -luser32 -lgdi32
#或者(CMD)
build.bat  
#或 (PowerShell)
.\build.bat
```

## 3\.2 编译汇编脚本

使用生成的main\.exe，编译自定义\.asm脚本，生成可运行的图形程序：

```bash
AsmGfx.exe 脚本名.asm -o 输出程序名.exe
# 示例：编译cube.asm，生成cube.exe
AsmGfx.exe cube.asm -o cube.exe
```

## 3\.3 运行图形程序

直接执行编译生成的\.exe文件，即可看到图形/动画效果：

```bash
cube.exe
```

# 四、汇编脚本语法（核心重点）

所有自定义脚本必须遵循「四段式结构」，顺序不可颠倒，每段功能固定，语法简洁易懂。

## 4\.1 第一段：窗口配置段 \.window:

用于配置图形窗口的基础属性，所有指令均为固定语法，可直接修改参数。

### 语法格式

```asm
.window:
INIT_W 窗口宽度    ; 单位：像素，示例：INIT_W 600
INIT_H 窗口高度    ; 单位：像素，示例：INIT_H 600
TEXT db "窗口标题" ; 窗口顶部标题，示例：TEXT db "3D旋转正方体"
color db "线条颜色" ; 图形线条颜色，支持固定取值
size 基础尺寸      ; 图形整体缩放比例，示例：size 120
sizepen 画笔粗细   ; 线条粗细，单位：像素，示例：sizepen 2
```

### 支持的线条颜色

red（红色）、green（绿色）、blue（蓝色）、white（白色）、yellow（黄色）、cyan（青蓝色）

## 4\.2 第二段：顶点坐标定义段 \.VertexDataSegment:

定义图形的所有顶点，每个顶点对应一个逻辑坐标（非像素），后续通过线段段关联顶点，形成图形。

### 语法格式

```asm
.VertexDataSegment:
顶点名 逻辑X坐标 逻辑Y坐标  ; 顶点名自定义（如A、p0、v1），坐标支持小数
; 示例（正方体8个顶点）
p0 -1.0 -1.0
p1 -1.0  1.0
p2  1.0  1.0
p3  1.0 -1.0
p4 -0.6 -0.6
p5 -0.6  0.6
p6  0.6  0.6
p7  0.6 -0.6
```

说明：顶点坐标为「归一化逻辑坐标」，具体映射规则见「第五部分 坐标系说明」。

## 4\.3 第三段：线段关联定义段 \.DataSegment:

将顶点段定义的顶点，两两关联形成线段，线段按定义顺序自动编号（1、2、3\.\.\.），后续指令通过编号绘制线段。

### 语法格式

```asm
.DataSegment:
顶点1 顶点2  ; 每一行定义一条线段，连接两个顶点
; 示例（正方体12条棱）
p0 p1
p1 p2
p2 p3
p3 p0
p4 p5
p5 p6
p6 p7
p7 p4
p0 p4
p1 p5
p2 p6
p3 p7
```

说明：线段编号从1开始，与定义顺序一致，绘图指令需通过编号调用线段。

## 4\.4 第四段：指令执行段 \.InstructionSegment:

核心段，用于编写指令，实现图形绘制、变量运算、延时、清屏、循环跳转等功能，指令按顺序执行，支持jmp跳转实现循环。

### 4\.4\.1 绘图指令（绘制线段）

固定语法，通过线段编号绘制指定线段，需配合「mov \+ call」使用：

```asm
mov data, 线段编号  ; 选中指定编号的线段（编号对应.DataSegment中的顺序）
mov com, 1         ; 固定写法，无需修改
call               ; 执行绘制操作，绘制选中的线段

; 示例：绘制前4条线段
mov data,1
mov com,1
call
mov data,2
mov com,1
call
mov data,3
mov com,1
call
mov data,4
mov com,1
call
```

### 4\.4\.2 控制指令（延时、清屏、跳转）

```asm
; 1. 延时指令：call sleep, 延时时间（单位：秒）
call sleep, 1      ; 延时1秒，支持变量（如call sleep, t）

; 2. 清屏指令：清空画布（黑色背景）
call clear

; 3. 跳转指令：jmp 行号，跳转到指令段第N行，实现无限循环
jmp 1              ; 跳转到指令段第1行，循环执行所有指令
```

### 4\.4\.3 变量系统（定义、赋值）

支持自定义变量，可用于控制延时、后续扩展的缩放/旋转等，全局可用：

```asm
; 1. 定义变量：var 变量名=初始值（初始值为整数）
var a=10           ; 定义变量a，初始值为10
var delay=1        ; 定义变量delay，用于控制延时

; 2. 重新赋值：set 变量名=新值
set a=50           ; 将变量a的值改为50
```

### 4\.4\.4 数学四则运算

支持变量间的加减乘除运算，运算结果直接赋值给第一个变量：

```asm
add a, b   ; a = a + b（将a和b的和赋值给a）
sub a, b   ; a = a - b（将a和b的差赋值给a）
mul a, b   ; a = a * b（将a和b的积赋值给a）
div a, b   ; a = a / b（将a和b的商赋值给a，除数不为0）
```

### 4\.4\.5 变量应用示例（延时控制）

```asm
.InstructionSegment:
var t=1             ; 定义变量t，初始值1
mov data,1
mov com,1
call
call sleep,t        ; 用变量t控制延时（1秒）
call clear
jmp 1
```

# 五、坐标系说明（关键理解）

虚拟机使用「归一化逻辑坐标」，与窗口像素坐标自动映射，核心规则如下，看懂即可轻松定义图形位置。

## 5\.1 基准原点

窗口正中心 = 坐标原点 \(0, 0\)，所有顶点坐标均围绕原点定义。

## 5\.2 坐标轴方向

|坐标轴|正方向|负方向|
|---|---|---|
|X轴|向右|向左|
|Y轴|向下|向上|

## 5\.3 坐标范围与映射

### 1\. 逻辑坐标范围

推荐使用 \-1\.0 \~ 1\.0 之间的小数（归一化），该范围下图形居中显示，比例协调，示例：

- 左上区域：\(\-1\.0, \-1\.0\)

- 右上区域：\(1\.0, \-1\.0\)

- 左下区域：\(\-1\.0, 1\.0\)

- 右下区域：\(1\.0, 1\.0\)

### 2\. 逻辑坐标 → 像素坐标映射公式

虚拟机内部自动将逻辑坐标换算为窗口像素坐标，无需手动计算，公式如下：

```plain text
像素X = 窗口中心X + 逻辑X × size
像素Y = 窗口中心Y + 逻辑Y × size
```

说明：

- 窗口中心X = INIT\_W / 2，窗口中心Y = INIT\_H / 2

- size 是脚本中定义的「基础尺寸」，控制图形整体放大缩小（size越大，图形越大）

### 5\.4 示例：正方形坐标理解

```asm
.VertexDataSegment:
A -0.8 -0.8  ; 左上（逻辑坐标）→ 像素坐标：中心X-0.8×size，中心Y-0.8×size
B  0.8 -0.8  ; 右上 → 像素坐标：中心X+0.8×size，中心Y-0.8×size
C  0.8  0.8  ; 右下 → 像素坐标：中心X+0.8×size，中心Y+0.8×size
D -0.8  0.8  ; 左下 → 像素坐标：中心X-0.8×size，中心Y+0.8×size

.DataSegment:
A B
B C
C D
D A
```

上述代码定义的是一个居中显示的正方形，size越大，正方形越大。

# 六、完整示例脚本

## 6\.1 基础示例：循环绘制正方形

```asm
.window:
INIT_W 600
INIT_H 600
TEXT db "基础正方形"
color db "green"
size 100
sizepen 2

.VertexDataSegment:
A -0.8 -0.8
B  0.8 -0.8
C  0.8  0.8
D -0.8  0.8

.DataSegment:
A B
B C
C D
D A

.InstructionSegment:
; 绘制4条边
mov data,1
mov com,1
call
mov data,2
mov com,1
call
mov data,3
mov com,1
call
mov data,4
mov com,1
call

; 延时1秒，清屏，跳回开头循环
call sleep,1
call clear
jmp 1
```

## 6\.2 进阶示例：变量控制循环绘制正方体

```asm
.window:
INIT_W 600
INIT_H 600
TEXT db "变量控制正方体"
color db "cyan"
size 130
sizepen 2

.VertexDataSegment:
p0 -1.0 -1.0
p1 -1.0  1.0
p2  1.0  1.0
p3  1.0 -1.0
p4 -0.6 -0.6
p5 -0.6  0.6
p6  0.6  0.6
p7  0.6 -0.6

.DataSegment:
p0 p1
p1 p2
p2 p3
p3 p0
p4 p5
p5 p6
p6 p7
p7 p4
p0 p4
p1 p5
p2 p6
p3 p7

.InstructionSegment:
; 定义变量，控制延时
var delay=1

; 绘制全部12条棱
mov data,1
mov com,1
call
mov data,2
mov com,1
call
mov data,3
mov com,1
call
mov data,4
mov com,1
call
mov data,5
mov com,1
call
mov data,6
mov com,1
call
mov data,7
mov com,1
call
mov data,8
mov com,1
call
mov data,9
mov com,1
call
mov data,10
mov com,1
call
mov data,11
mov com,1
call
mov data,12
mov com,1
call

; 延时、清屏、循环
call sleep,delay
call clear
jmp 1
```

# 七、虚拟机特性说明

- 窗口稳定性：内置消息处理机制，点击、拖动窗口不未响应、不崩溃，关闭窗口可正常退出。

- 无报错警告：修复所有隐式声明、类型重定义问题，编译全程无警告、无错误。

- 功能完整性：支持窗口配置、图形绘制、变量定义、四则运算、循环跳转、延时清屏。

- 易用性：脚本语法简洁，无需复杂配置，新手可快速上手编写图形脚本。

- 可扩展性：预留功能接口，可后续扩展动态缩放、颜色切换、3D旋转、键盘控制等。

# 八、常见问题与解决方法

|常见问题|解决方法|
|---|---|
|编译时提示「tcc is not recognized」|确认build\.c中编译命令为gcc（已在最新版中修复，直接替换pack\.h和build\.c即可）。|
|点击窗口提示未响应、崩溃|替换最新版pack\.h，内置窗口消息处理机制，可解决该问题。|
|编译提示「implicit declaration of function \&\#39;exit\&\#39;」|pack\.h中已添加\&lt;stdlib\.h\&gt;头文件，直接使用最新版即可。|
|图形显示静止，无法实现动画|确保脚本中添加jmp跳转指令，形成循环；延时时间不宜过长（推荐0\.1\~1秒）。|
|图形显示偏移、比例异常|调整顶点逻辑坐标（推荐\-1\.0\~1\.0），或修改size参数控制整体大小。|

# 九、后续可扩展功能（预留）

如需扩展以下功能，可直接联系修改底层代码，无需修改现有脚本语法：

- 变量控制图形大小（动态缩放、呼吸效果）

- 条件跳转（jmp\_eq/jmp\_neq，根据变量比较结果跳转）

- 运行时线条颜色切换（变量控制颜色）

- 真3D顶点旋转计算（X/Y/Z轴旋转，立体效果）

- 键盘按键监听（控制动画启停、速度、方向）



