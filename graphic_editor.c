#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH 80
#define HEIGHT 24

#define EMPTY '_'
#define PIXEL '*'

#define MAX_SHAPES 100

// Shape types
enum ShapeType { LINE = 1, RECTANGLE, CIRCLE, TRIANGLE };

// Structure to track drawn shapes for editing/deletion
typedef struct {
    int id;
    enum ShapeType type;
    int params[6]; // Stores coordinates and radii based on shape type
    int active;    // 1 if active, 0 if deleted
} Shape;

char picture[HEIGHT][WIDTH];
Shape shapes[MAX_SHAPES];
int shapeCount = 0;
int nextId = 1;

// --- Core Canvas Functions ---

void clearPicture() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            picture[y][x] = EMPTY;
        }
    }
}

void displayPicture() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            putchar(picture[y][x]);
        }
        putchar('\n');
    }
}

void setPixel(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        picture[y][x] = PIXEL;
    }
}

// --- Graphics Algorithms ---
void drawLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;
// Bresenham's Line Algorithm


    while (1) {
        setPixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void drawRectangle(int x1, int y1, int x2, int y2) {
    drawLine(x1, y1, x2, y1); // Top
    drawLine(x1, y2, x2, y2); // Bottom
    drawLine(x1, y1, x1, y2); // Left
    drawLine(x2, y1, x2, y2); // Right
}

// Bresenham's Circle Algorithm
void drawCircle(int cx, int cy, int radius) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (y >= x) {
        setPixel(cx + x, cy + y);
        setPixel(cx - x, cy + y);
        setPixel(cx + x, cy - y);
        setPixel(cx - x, cy - y);
        setPixel(cx + y, cy + x);
        setPixel(cx - y, cy + x);
        setPixel(cx + y, cy - x);
        setPixel(cx - y, cy - x);
        
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x3, y3);
    drawLine(x3, y3, x1, y1);
}

// --- Canvas Re-rendering Engine ---

void redrawCanvas() {
    clearPicture();
    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].active) {
            switch (shapes[i].type) {
                case LINE:
                    drawLine(shapes[i].params[0], shapes[i].params[1], shapes[i].params[2], shapes[i].params[3]);
                    break;
                case RECTANGLE:
                    drawRectangle(shapes[i].params[0], shapes[i].params[1], shapes[i].params[2], shapes[i].params[3]);
                    break;
                case CIRCLE:
                    drawCircle(shapes[i].params[0], shapes[i].params[1], shapes[i].params[2]);
                    break;
                case TRIANGLE:
                    drawTriangle(shapes[i].params[0], shapes[i].params[1], shapes[i].params[2], shapes[i].params[3], shapes[i].params[4], shapes[i].params[5]);
                    break;
            }
        }
    }
}

void listShapes() {
    printf("\n--- Active Shapes ---\n");
    int found = 0;
    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].active) {
            found = 1;
            printf("ID %d: ", shapes[i].id);
            switch (shapes[i].type) {
                case LINE: 
                    printf("Line (%d,%d) to (%d,%d)\n", shapes[i].params[0], shapes[i].params[1], shapes[i].params[2], shapes[i].params[3]); 
                    break;
                case RECTANGLE: 
                    printf("Rectangle Top-Left:(%d,%d), Bottom-Right:(%d,%d)\n", shapes[i].params[0], shapes[i].params[1], shapes[i].params[2], shapes[i].params[3]); 
                    break;
                case CIRCLE: 
                    printf("Circle Center:(%d,%d), Radius:%d\n", shapes[i].params[0], shapes[i].params[1], shapes[i].params[2]); 
                    break;
                case TRIANGLE: 
                    printf("Triangle P1:(%d,%d), P2:(%d,%d), P3:(%d,%d)\n", shapes[i].params[0], shapes[i].params[1], shapes[i].params[2], shapes[i].params[3], shapes[i].params[4], shapes[i].params[5]); 
                    break;
            }
        }
    }
    if (!found) printf("No shapes on canvas.\n");
}

// --- Main Program Loop ---

int main() {
    int choice;
    clearPicture();

    printf("2D Graphics Editor\n");
    printf("Canvas size: %d x %d\n", WIDTH, HEIGHT);
    printf("Use coordinates x y.\n");
    printf("x range: 0 to %d\n", WIDTH - 1);
    printf("y range: 0 to %d\n", HEIGHT - 1);

    while (1) {
        printf("\nMenu\n");
        printf("1. Draw Line\n");
        printf("2. Draw Rectangle\n");
        printf("3. Draw Circle\n");
        printf("4. Draw Triangle\n");
        printf("5. Display Picture\n");
        printf("6. Delete Object\n");
        printf("7. Modify Object\n");
        printf("0. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) break;

        if (choice >= 1 && choice <= 4) {
            if (shapeCount >= MAX_SHAPES) {
                printf("Maximum shape limit reached!\n");
                continue;
            }
            shapes[shapeCount].id = nextId++;
            shapes[shapeCount].type = (enum ShapeType)choice;
            shapes[shapeCount].active = 1;

            if (choice == 1) {
                printf("Enter x1 y1 x2 y2: ");
                scanf("%d %d %d %d", &shapes[shapeCount].params[0], &shapes[shapeCount].params[1], &shapes[shapeCount].params[2], &shapes[shapeCount].params[3]);
            } else if (choice == 2) {
                printf("Enter top-left x y and bottom-right x y: ");
                scanf("%d %d %d %d", &shapes[shapeCount].params[0], &shapes[shapeCount].params[1], &shapes[shapeCount].params[2], &shapes[shapeCount].params[3]);
            } else if (choice == 3) {
                printf("Enter center x y and radius: ");
                scanf("%d %d %d", &shapes[shapeCount].params[0], &shapes[shapeCount].params[1], &shapes[shapeCount].params[2]);
            } else if (choice == 4) {
                printf("Enter x1 y1 x2 y2 x3 y3: ");
                scanf("%d %d %d %d %d %d", &shapes[shapeCount].params[0], &shapes[shapeCount].params[1], &shapes[shapeCount].params[2], &shapes[shapeCount].params[3], &shapes[shapeCount].params[4], &shapes[shapeCount].params[5]);
            }
            shapeCount++;
            redrawCanvas();
            printf("Shape added successfully.\n");
        }
        else if (choice == 5) {
            printf("The picture is:\n");
            displayPicture();
        }
        else if (choice == 6) {
            listShapes();
            printf("Enter Shape ID to delete: ");
            int delId;
            scanf("%d", &delId);
            
            int found = 0;
            for (int i = 0; i < shapeCount; i++) {
                if (shapes[i].id == delId && shapes[i].active) {
                    shapes[i].active = 0;
                    found = 1;
                    break;

                }
            }
            if (found) {
                redrawCanvas();
                printf("Shape deleted.\n");
            } else {
                printf("Shape ID not found.\n");
            }
        }
        else if (choice == 7) {
            listShapes();
            printf("Enter Shape ID to modify: ");
            int modId;
            scanf("%d", &modId);

            int found = -1;
            for (int i = 0; i < shapeCount; i++) {
                if (shapes[i].id == modId && shapes[i].active) {
                    found = i;
                    break;
                }
            }

        

            if (found != -1) {
                printf("Modifying shape details...\n");
                if (shapes[found].type == LINE) {
                    printf("Enter new x1 y1 x2 y2: ");
                    scanf("%d %d %d %d", &shapes[found].params[0], &shapes[found].params[1], &shapes[found].params[2], &shapes[found].params[3]);
                } else if (shapes[found].type == RECTANGLE) {
                    printf("Enter new top-left x y and bottom-right x y: ");
                    scanf("%d %d %d %d", &shapes[found].params[0], &shapes[found].params[1], &shapes[found].params[2], &shapes[found].params[3]);
                } else if (shapes[found].type == CIRCLE) {
                    printf("Enter new center x y and radius: ");
                    scanf("%d %d %d", &shapes[found].params[0], &shapes[found].params[1], &shapes[found].params[2]);
                } else if (shapes[found].type == TRIANGLE) {
                    printf("Enter new x1 y1 x2 y2 x3 y3: ");
                    scanf("%d %d %d %d %d %d", &shapes[found].params[0], &shapes[found].params[1], &shapes[found].params[2], &shapes[found].params[3], &shapes[found].params[4], &shapes[found].params[5]);
                }
                redrawCanvas();
                printf("Shape updated.\n");
            } else {
                printf("Shape ID not found.\n");
            }
        }
        else if (choice == 0) {
            printf("Exiting program.\n");
            break;
        }
        else {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}
