1. Architectural Strategy:
1. The Render Pipeline PatternIn many primitive paint programs, drawing an object changes the pixels of the canvas permanently.
 This becomes problematic when a user tries to delete or modify an existing shape, because the program would not know which pixels belong to which shape,
 or what was underneath them before they were drawn.Your code beautifully resolves this by acting like a modern vector graphics engine (similar to SVG or Adobe Illustrator).
The Database (shapes array): Instead of only saving pixels, the code keeps a running registry of shapes, their parameters, and a flag indicating whether they are active (visible) or not.
The Pipeline (redrawCanvas): When an object is deleted or updated, the canvas matrix buffer is completely wiped (clearPicture()). The engine then iterates through the registry
and re-plots only the active shapes back onto the clean canvas from scratch.
2. Deep Dive Into the Core AlgorithmsRasters (character matrices or pixel grids) are discrete structures, while math shapes are continuous.
Converting continuous lines or curves into discrete grid positions without using heavy decimal arithmetic (float or double) requires optimization.
 his program leverages two classic computer graphics algorithms:
A. Bresenham's Line Algorithm (drawLine)If you try to draw a diagonal line on a character grid,
 mply calculating $y = mx + b$ introduces rounding errors and requires slow floating-point arithmetic.Bresenham's algorithm utilizes purely integer addition and shifting.
It increments along the primary axis ($x$ or $y$) step-by-step and uses a running "error" term (err) to evaluate whether the next pixel should stay on the same row/column
or step diagonally closer to the destination.Significance in your code: It uses a generalized version of Bresenham's algorithm that naturally accommodates lines spanning all 8 octants
 (lines going left, right, up, down, or at any custom diagonal slope) cleanly using direction signs (sx, sy).
B. Midpoint (Bresenham's) Circle Algorithm
(drawCircle)Computing a circular path usually involves trigonometry ($\sin(\theta), \cos(\theta)$) or square roots ($\sqrt{r^2 - x^2}$), which are computationally expensive.
The Midpoint Circle Algorithm avoids this by computing pixel coordinates for only one octant (a $45^\circ$ slice) of the circle using a decision parameter d. It then exploits the 8-way
 spatial symmetry of a circle to map that single computed point to all eight corresponding locations around the center point (cx, cy) simultaneously:CsetPixel(cx + x, cy + y); // Octant 1

setPixel(cx - x, cy + y); // Octant 2
setPixel(cx + x, cy - y); // Octant 3
setPixel(cx - x, cy - y); // Octant 4
setPixel(cx + y, cy + x); // Octant 5
setPixel(cx - y, cy + x); // Octant 6
setPixel(cx + y, cy - x); // Octant 7
setPixel(cx - y, cy - x); // Octant 8
3. Structural Limits and ConstraintsMaximum Capacity (MAX_SHAPES = 100):
The application caps out once 100 history items have been created. Note that even if you delete a shape, shapeCount continues to rise because deleted items remain in the 
registry with their active state flag flipped to 0.Array Mapping Safety (setPixel): It contains a crucial defensive guard clause:Cif (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
If a user inputs coordinates that fall outside your 80x24 boundary grid (e.g., drawing a massive circle whose edges spill over), 
this guard prevents the code from executing out-of-bounds memory writes, protecting your execution stack from crashing or facing memory corruption.
4. Interactive State Machine (Main Program Execution)The program runs an infinite state loop (while(1)) processing commands entered via standard input (scanf).
Below is an execution map tracking how a shape travels through the system:[ User Inputs Parameters via Menu ]
                │
                ▼
   [ Create New Shape Instance ] ──► Assigns Unique ID & active = 1
                │
                ▼
     [ Append to shapes[] ]
                │
                ▼
       [ Invoke redrawCanvas() ]
                │
                ├─► 1. clearPicture() [Wipe canvas with '_']
                │
                └─► 2. Loop through all active shapes
                            │
                            ├─► Line     ──► drawLine()      ┐
                            ├─► Rect     ──► drawRectangle() ├─► setPixel()
                            ├─► Circle   ──► drawCircle()    │
                            └─► Triangle ──► drawTriangle()  ┘
Potential Enhancements for Your Code BaseGarbage Collection:
If you want the program to handle endless edits, modify the addition sequence so that if shapeCount == MAX_SHAPES, it defragments the array by purging items where active == 0.
Parametric Validation: Adding validation filters checking if a circle's radius is negative before processing saves calculation runtime cycles on corrupted variables.
