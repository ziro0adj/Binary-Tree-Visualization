#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>

#define NODE_RADIUS 25

typedef struct TreeNode {
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

void Delay(int milliseconds) {
    long pause;
    clock_t now, then;

    pause = milliseconds * (CLOCKS_PER_SEC / 1000);
    now = then = clock();
    while ((now - then) < pause) {
        now = clock();
    }
}

void drawSearchTree(TreeNode* root, int x, int y, int horizontalSpacing, TreeNode* highlightedNode, TreeNode** visitedNodes, int numVisitedNodes) {
    if (root != NULL) {
        // seting  the default color node
        Color nodeColor = SKYBLUE;
        //checking if the current node was part of the visited nodes and highlighting the node if corect
        for (int i = 0; i < numVisitedNodes; i++) {
            if (root == visitedNodes[i]) {
                nodeColor = (root == highlightedNode) ? ORANGE : SKYBLUE;
                break;
            }
        }

        // Draw circle for the current node
        DrawCircle(x, y, NODE_RADIUS, nodeColor);
        DrawText(TextFormat("%d", root->data), x - 12, y - 12, 20, RAYWHITE);

        // Draw left subtree and connect with a line
        if (root->left != NULL) {
            drawSearchTree(root->left, x - horizontalSpacing, y + 80, horizontalSpacing / 2, highlightedNode, visitedNodes, numVisitedNodes);
            DrawLine(x, y + NODE_RADIUS, x - horizontalSpacing, y + 80 - NODE_RADIUS, DARKGRAY);
        }

        // Draw right subtree and connect with a line
        if (root->right != NULL) {
            drawSearchTree(root->right, x + horizontalSpacing, y + 80, horizontalSpacing / 2, highlightedNode, visitedNodes, numVisitedNodes);
            DrawLine(x, y + NODE_RADIUS, x + horizontalSpacing, y + 80 - NODE_RADIUS, DARKGRAY);
        }
    }
}


TreeNode* searchTree(TreeNode* root, int searchValue, TreeNode*** visitedNodes, int* numVisitedNodes) {
    *numVisitedNodes = 0;

    // Create a copy of the root to use during the search
    TreeNode* current = root;

    while (current != NULL) {
        // Add the current node to the visited nodes array
        *visitedNodes = (TreeNode**)realloc(*visitedNodes, (*numVisitedNodes + 1) * sizeof(TreeNode*));
        (*visitedNodes)[*numVisitedNodes] = current;
        (*numVisitedNodes)++;

        // Highlight the current node before moving to the next one
        BeginDrawing();
        drawSearchTree(root, GetScreenWidth() / 2, 50, 220, current, *visitedNodes, *numVisitedNodes);
        EndDrawing();

        Delay(800); //delay time

        if (searchValue == current->data) {
            return current;  // Node with the specified value found
        } else if (searchValue < current->data) {
            current = current->left;  // Search in the left subtree
        } else {
            current = current->right;  // Search in the right subtree
        }
    }

    return NULL;  // Node with the specified value not found
}

void freeTree(TreeNode* root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

void generateTree(TreeNode** root, int n, int maxLevels) {
    if (n <= 0 || maxLevels <= 0) {
        *root = NULL;
        return;
    }

    *root = (TreeNode*)malloc(sizeof(TreeNode));
    (*root)->data = GetRandomValue(1, 100);
    (*root)->left = (*root)->right = NULL;

    for (int i = 1; i < n; i++) {
        int value = GetRandomValue(1, 100);
        TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
        newNode->data = value;
        newNode->left = newNode->right = NULL;

        TreeNode* current = *root;
        int level = 1;

        while (1) {
            if (value < current->data) {
                if (current->left == NULL) {
                    current->left = newNode;
                    break;
                } else {
                    current = current->left;
                }
            } else {
                if (current->right == NULL) {
                    current->right = newNode;
                    break;
                } else {
                    current = current->right;
                }
            }

            level++;

            if (level >= maxLevels) {
                break;
            }
        }
    }
}

int main(void) {
    InitWindow(1000, 800, "Binary Tree Visualization");
    SetTargetFPS(60);

    TreeNode* root = NULL;
    TreeNode** visitedNodes = NULL;
    int numVisitedNodes = 0;

    char inputText[256] = "";
    bool isMouseOverRect = false;

    while (!WindowShouldClose()) {
        Vector2 mousePoint = GetMousePosition();
        isMouseOverRect = CheckCollisionPointRec(mousePoint, (Rectangle){10, (float)GetScreenHeight() - 50, 470, 40});

        if (isMouseOverRect) {
            int key = GetKeyPressed();
            if (key != 0) {
                if (key == KEY_ENTER) {
                    int searchValue = atoi(inputText);
                    inputText[0] = '\0';
                    TreeNode* foundNode = searchTree(root, searchValue, &visitedNodes, &numVisitedNodes);

                } else if (key == KEY_BACKSPACE) {
                    int length = TextLength(inputText);
                    if (length > 0) {
                        inputText[length - 1] = '\0';
                    }
                } else if ((key >= KEY_ZERO && key <= KEY_NINE) || (key >= KEY_KP_0 && key <= KEY_KP_9)) {
                    int length = TextLength(inputText);
                    if (length < sizeof(inputText) - 1) {
                        inputText[length] = (char)key;
                        inputText[length + 1] = '\0';
                    }
                }
            }
        }
        // drawing the screen
        BeginDrawing();
        ClearBackground(GRAY);
        //updating the tree image
        drawSearchTree(root, GetScreenWidth() / 2, 50, 220, NULL, visitedNodes, numVisitedNodes);
        //drawing the menu
        DrawRectangle(10, GetScreenHeight() - 100, 470, 40,DARKGRAY);
        DrawRectangleLinesEx((Rectangle){10, (float)GetScreenHeight() - 100, 470, 40}, 2, BLACK);
        DrawText("Press 'R' to Generate Binary Tree", 20, GetScreenHeight() - 90, 25, RAYWHITE);
        DrawRectangle(10, GetScreenHeight() - 50, 470, 40, isMouseOverRect ? SKYBLUE : DARKGRAY);
        DrawRectangleLinesEx((Rectangle){10, (float)GetScreenHeight() - 50, 470, 40}, 2, BLACK);
        DrawText(TextFormat("Search For a Value: %s", inputText), 20, GetScreenHeight() - 40, 20, RAYWHITE);
        EndDrawing();

        // checking for the key R to generate a random binary tree
        if (IsKeyPressed(KEY_R)) {
            freeTree(root);
            int newNodeCount = GetRandomValue(3, 10);
            generateTree(&root, newNodeCount, 5);
        }
    }

    CloseWindow();
    freeTree(root);
    free(visitedNodes);

    return 0;
}
