#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define NODE_RADIUS 25
#define LINE_THICKNESS 10

typedef struct TreeNode {
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

TreeNode* createNode(int value) {
    TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
    newNode->data = value;
    newNode->left = newNode->right = NULL;
    return newNode;
}

void drawTree(TreeNode* root, int x, int y, int horizontalSpacing) {
    if (root != NULL) {
        // Draw circle for the current node
        DrawCircle(x, y, NODE_RADIUS, SKYBLUE);
        DrawText(TextFormat("%d", root->data), x - 12, y - 12, 20, RAYWHITE);

        // Draw left subtree and connect with a line
        if (root->left != NULL) {
            drawTree(root->left, x - horizontalSpacing, y + 80, horizontalSpacing / 2);
            DrawLine(x, y + NODE_RADIUS, x - horizontalSpacing, y + 80 - NODE_RADIUS, DARKGRAY);
        }

        // Draw right subtree and connect with a line
        if (root->right != NULL) {
            drawTree(root->right, x + horizontalSpacing, y + 80, horizontalSpacing / 2);
            DrawLine(x, y + NODE_RADIUS, x + horizontalSpacing, y + 80 - NODE_RADIUS, DARKGRAY);
        }
    }
}

void freeTree(TreeNode* root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

TreeNode* generateRandomTree(int n) {
    if (n <= 0) {
        return NULL;
    }

    TreeNode* root = createNode(GetRandomValue(1, 100)); // Random data value for the root

    for (int i = 1; i < n; i++) {
        int value = GetRandomValue(1, 100); // Random data value for each new node
        TreeNode* newNode = createNode(value);

        // Traverse the tree to find a suitable position for the new node
        TreeNode* current = root;
        while (1) {
            // If the new value is smaller, go left
            if (value < current->data) {
                if (current->left == NULL) {
                    current->left = newNode;
                    break;
                } else {
                    current = current->left;
                }
            }
            // If the new value is larger or equal, go right
            else {
                if (current->right == NULL) {
                    current->right = newNode;
                    break;
                } else {
                    current = current->right;
                }
            }
        }
    }

    return root;
}



int main(void) {
    // Create a sample binary tree
    TreeNode* root = NULL;

    // Initialize Raylib
    InitWindow(1000, 800, "Binary Tree Visualization");

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GRAY);

        // Draw the binary tree
        drawTree(root, GetScreenWidth() / 2, 50, 200);

        // Draw menu
        DrawRectangle(10, GetScreenHeight() - 50, 470, 40, SKYBLUE);
        DrawText("Press 'R' to generate random tree", 20, GetScreenHeight() - 40, 25, RAYWHITE);

        EndDrawing();

        // Check for menu option
        if (IsKeyPressed(KEY_R)) {
            // Free the current tree
            freeTree(root);

            // Generate a new random tree with user input
            int newNodeCount = GetRandomValue(3, 10); // Adjust the range as needed
            root = generateRandomTree(newNodeCount);
        }
    }

    // Close Raylib
    CloseWindow();

    // Free memory
    freeTree(root);

    return 0;
}
