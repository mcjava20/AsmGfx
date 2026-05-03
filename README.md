# Complete Documentation of Assembly\-Based Graphics Card Rendering Language

# I\. Document Introduction

This document details the usage methods, core features, script syntax, coordinate system rules, and common operations of the self\-developed lightweight assembly\-based graphics card rendering language\. It is suitable for quickly getting started with developing graphics and animation scripts using the virtual machine, with no compilation warnings throughout, no window unresponsiveness or crashes during operation, and support for core functions such as variable operations and loop jumps\.

# II\. Compilation Environment and Project Structure

## 2\.1 Compilation Environment

- Compiler: GCC \(Recommended, adapted, no TCC dependency\)

- Dependency Libraries: user32 \(window control\), gdi32 \(graphics rendering\)

- Operating Platform: Windows system

- No additional environment configuration required; direct compilation is available

## 2\.2 Project File Structure

```plain text
├── main.c      // Core: Assembly script parser (parses .asm script instructions, variables, coordinates, etc.)
├── build.c     // Auxiliary: Code generation + compilation driver (generates runtime C code and calls GCC for compilation)
├── pack.h      // Underlying: Virtual machine runtime template (window creation, graphics rendering, variables, message processing, etc.)
├── xxx.asm     // Custom: Graphics/animation assembly script (written by users to achieve specific graphic effects)
├── main.exe    // Final compiler (compiles .asm scripts into executable graphics programs)
└── README.md   // This document (complete usage instructions)
```

# III\. Compiler Usage Methods

## 3\.1 Compile the Compiler Itself

Open CMD/PowerShell, enter the project directory, and execute the following command to generate the compiler main\.exe:

```bash
gcc main.c build.c -o AsmGfx.exe -luser32 -lgdi32
# Or (CMD)
build.bat  
# Or (PowerShell)
.\build.bat
```

## 3\.2 Compile Assembly Scripts

Use the generated main\.exe to compile custom \.asm scripts and generate runnable graphics programs:

```bash
AsmGfx.exe script_name.asm -o output_program_name.exe
# Example: Compile cube.asm to generate cube.exe
AsmGfx.exe cube.asm -o cube.exe
```

## 3\.3 Run the Graphics Program

Directly execute the compiled \.exe file to see the graphics/animation effect:

```bash
cube.exe
```

# IV\. Assembly Script Syntax \(Core Focus\)

All custom scripts must follow a \&\#34;four\-segment structure\&\#34; with an unchangeable order\. Each segment has a fixed function and simple, easy\-to\-understand syntax\.

## 4\.1 First Segment: Window Configuration Segment \.window:

Used to configure the basic properties of the graphics window\. All instructions have fixed syntax, and parameters can be modified directly\.

### Syntax Format

```asm
.window:
INIT_W window_width    ; Unit: pixels, Example: INIT_W 600
INIT_H window_height   ; Unit: pixels, Example: INIT_H 600
TEXT db "window_title" ; Window top title, Example: TEXT db "3D Rotating Cube"
color db "line_color"  ; Graphic line color, supports fixed values
size base_size         ; Overall graphic scaling ratio, Example: size 120
sizepen pen_thickness  ; Line thickness, Unit: pixels, Example: sizepen 2
```

### Supported Line Colors

red, green, blue, white, yellow, cyan

## 4\.2 Second Segment: Vertex Coordinate Definition Segment \.VertexDataSegment:

Defines all vertices of the graphic\. Each vertex corresponds to a logical coordinate \(not pixel\), and vertices are associated through the line segment segment to form the graphic\.

### Syntax Format

```asm
.VertexDataSegment:
vertex_name logical_X_coordinate logical_Y_coordinate  ; Vertex name is custom (e.g., A, p0, v1), coordinates support decimals
; Example (8 vertices of a cube)
p0 -1.0 -1.0
p1 -1.0  1.0
p2  1.0  1.0
p3  1.0 -1.0
p4 -0.6 -0.6
p5 -0.6  0.6
p6  0.6  0.6
p7  0.6 -0.6
```

Note: Vertex coordinates are \&\#34;normalized logical coordinates\&\#34;\. For specific mapping rules, refer to \&\#34;Part V: Coordinate System Description\&\#34;\.

## 4\.3 Third Segment: Line Segment Association Definition Segment \.DataSegment:

Associates vertices defined in the vertex segment in pairs to form line segments\. Line segments are automatically numbered \(1, 2, 3\.\.\.\) in the order of definition, and subsequent instructions draw line segments through the numbers\.

### Syntax Format

```asm
.DataSegment:
vertex1 vertex2  ; Each line defines a line segment connecting two vertices
; Example (12 edges of a cube)
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

Note: Line segment numbering starts from 1, consistent with the order of definition\. Drawing instructions need to call line segments through numbers\.

## 4\.4 Fourth Segment: Instruction Execution Segment \.InstructionSegment:

The core segment is used to write instructions to implement functions such as graphic drawing, variable operations, delay, screen clearing, and loop jumps\. Instructions are executed in order, and jmp jumps are supported to implement loops\.

### 4\.4\.1 Drawing Instructions \(Draw Line Segments\)

Fixed syntax: Draw the specified line segment through the line segment number, which needs to be used with \&\#34;mov \+ call\&\#34;:

```asm
mov data, line_segment_number  ; Select the line segment with the specified number (the number corresponds to the order in .DataSegment)
mov com, 1                     ; Fixed writing, no need to modify
call                           ; Execute the drawing operation to draw the selected line segment

; Example: Draw the first 4 line segments
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

### 4\.4\.2 Control Instructions \(Delay, Screen Clear, Jump\)

```asm
; 1. Delay instruction: call sleep, delay time (Unit: seconds)
call sleep, 1      ; Delay for 1 second, supports variables (e.g., call sleep, t)

; 2. Screen clear instruction: Clear the canvas (black background)
call clear

; 3. Jump instruction: jmp line_number, jump to the Nth line of the instruction segment to implement infinite loop
jmp 1              ; Jump to the 1st line of the instruction segment and execute all instructions cyclically
```

### 4\.4\.3 Variable System \(Definition, Assignment\)

Supports custom variables, which can be used to control delay, subsequent extended scaling/rotation, etc\., and are globally available:

```asm
; 1. Define variable: var variable_name=initial_value (initial value is an integer)
var a=10           ; Define variable a with an initial value of 10
var delay=1        ; Define variable delay for controlling delay

; 2. Reassign: set variable_name=new_value
set a=50           ; Change the value of variable a to 50
```

### 4\.4\.4 Mathematical Four Operations

Supports addition, subtraction, multiplication, and division operations between variables\. The operation result is directly assigned to the first variable:

```asm
add a, b   ; a = a + b (assign the sum of a and b to a)
sub a, b   ; a = a - b (assign the difference between a and b to a)
mul a, b   ; a = a * b (assign the product of a and b to a)
div a, b   ; a = a / b (assign the quotient of a and b to a, divisor cannot be 0)
```

### 4\.4\.5 Variable Application Example \(Delay Control\)

```asm
.InstructionSegment:
var t=1             ; Define variable t with an initial value of 1
mov data,1
mov com,1
call
call sleep,t        ; Use variable t to control delay (1 second)
call clear
jmp 1
```

# V\. Coordinate System Description \(Key Understanding\)

The virtual machine uses \&\#34;normalized logical coordinates\&\#34;, which are automatically mapped to window pixel coordinates\. The core rules are as follows; understanding them allows you to easily define graphic positions\.

## 5\.1 Reference Origin

The center of the window = coordinate origin \(0, 0\), and all vertex coordinates are defined around the origin\.

## 5\.2 Coordinate Axis Directions

|Coordinate Axis|Positive Direction|Negative Direction|
|---|---|---|
|X\-axis|To the right|To the left|
|Y\-axis|Downward|Upward|

## 5\.3 Coordinate Range and Mapping

### 1\. Logical Coordinate Range

It is recommended to use decimals between \-1\.0 \~ 1\.0 \(normalized\)\. In this range, the graphic is displayed centered with a coordinated ratio\. Examples:

- Upper left area: \(\-1\.0, \-1\.0\)

- Upper right area: \(1\.0, \-1\.0\)

- Lower left area: \(\-1\.0, 1\.0\)

- Lower right area: \(1\.0, 1\.0\)

### 2\. Logical Coordinates → Pixel Coordinates Mapping Formula

The virtual machine automatically converts logical coordinates to window pixel coordinates, no manual calculation is required\. The formula is as follows:

```plain text
Pixel X = Window Center X + Logical X × size
Pixel Y = Window Center Y + Logical Y × size
```

Note:

- Window Center X = INIT\_W / 2, Window Center Y = INIT\_H / 2

- size is the \&\#34;base size\&\#34; defined in the script, which controls the overall scaling of the graphic \(the larger the size, the larger the graphic\)

### 5\.4 Example: Understanding Square Coordinates

```asm
.VertexDataSegment:
A -0.8 -0.8  ; Upper left (logical coordinates) → Pixel coordinates: Center X - 0.8×size, Center Y - 0.8×size
B  0.8 -0.8  ; Upper right → Pixel coordinates: Center X + 0.8×size, Center Y - 0.8×size
C  0.8  0.8  ; Lower right → Pixel coordinates: Center X + 0.8×size, Center Y + 0.8×size
D -0.8  0.8  ; Lower left → Pixel coordinates: Center X - 0.8×size, Center Y + 0.8×size

.DataSegment:
A B
B C
C D
D A
```

The above code defines a centered square; the larger the size, the larger the square\.

# VI\. Complete Example Scripts

## 6\.1 Basic Example: Cyclically Draw a Square

```asm
.window:
INIT_W 600
INIT_H 600
TEXT db "Basic Square"
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
; Draw 4 edges
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

; Delay for 1 second, clear screen, jump back to the beginning to cycle
call sleep,1
call clear
jmp 1
```

## 6\.2 Advanced Example: Variable\-Controlled Cyclic Cube Drawing

```asm
.window:
INIT_W 600
INIT_H 600
TEXT db "Variable-Controlled Cube"
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
; Define variables to control delay
var delay=1

; Draw all 12 edges
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

; Delay, clear screen, cycle
call sleep,delay
call clear
jmp 1
```

# VII\. Virtual Machine Feature Description

- Window Stability: Built\-in message processing mechanism, no unresponsiveness or crashes when clicking or dragging the window, and the window can exit normally when closed\.

- No Errors or Warnings: All implicit declarations and type redefinition issues are fixed, with no warnings or errors throughout the compilation process\.

- Functional Integrity: Supports window configuration, graphic drawing, variable definition, four arithmetic operations, loop jumps, delay, and screen clearing\.

- Usability: Simple script syntax, no complex configuration, allowing beginners to quickly get started with writing graphic scripts\.

- Extensibility: Reserved functional interfaces, which can be extended with dynamic scaling, color switching, 3D rotation, keyboard control, etc\., in the future\.

# VIII\. Common Problems and Solutions

|Common Problems|Solutions|
|---|---|
|Compilation prompt: \&\#34;tcc is not recognized\&\#34;|Confirm that the compilation command in build\.c is gcc \(fixed in the latest version; directly replace pack\.h and build\.c\)\.|
|Window unresponsiveness or crash when clicked|Replace with the latest version of pack\.h, which has a built\-in window message processing mechanism to solve this problem\.|
|Compilation prompt: \&\#34;implicit declaration of function \&\#39;exit\&\#39;\&\#34;|The \&lt;stdlib\.h\&gt; header file has been added to pack\.h; use the latest version directly\.|
|The graphic is static and cannot realize animation|Ensure that the jmp jump instruction is added to the script to form a loop; the delay time should not be too long \(recommended 0\.1\~1 second\)\.|
|Graphic display offset or abnormal proportion|Adjust the vertex logical coordinates \(recommended \-1\.0\~1\.0\) or modify the size parameter to control the overall size\.|

# IX\. Future Extensible Functions \(Reserved\)

To extend the following functions, you can directly contact to modify the underlying code without changing the existing script syntax:

- Variable\-controlled graphic size \(dynamic scaling, breathing effect\)

- Conditional jump \(jmp\_eq/jmp\_neq, jump according to variable comparison results\)

- Runtime line color switching \(variable\-controlled color\)

- True 3D vertex rotation calculation \(X/Y/Z axis rotation, three\-dimensional effect\)

- Keyboard key monitoring \(control animation start/stop, speed, direction\)
