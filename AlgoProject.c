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

//delay function
void Delay(int milliseconds) {
    long pause;
    clock_t now, then;

    pause = milliseconds * (CLOCKS_PER_SEC / 1000);
    now = then = clock();
    while ((now - then) < pause) {
        now = clock();
    }
}

//function for redrawing the tree with highlighted nodes while searching
/*its paramaters : the root node , x root postion, y root position,space between node and children nodes,the current highlighted node,
the history of visited nodes while searching,the number of visited nodes*/
void drawSearchTree(TreeNode* root, int x, int y, int horizontalSpacing, TreeNode* highlightedNode, TreeNode** visitedNodes, int numVisitedNodes) {
    if (root != NULL) {
        // set default color
        Color nodeColor = SKYBLUE;

        // compare the current node with the visited nodes to highlight it or not
        for (int i = 0; i < numVisitedNodes; i++) {
            if (root == visitedNodes[i]) {
                nodeColor = (root == highlightedNode) ? ORANGE : SKYBLUE;
                break;
            }
        }

        // drawing tree again to move to the next highlighted node
        DrawCircle(x, y, NODE_RADIUS, nodeColor);

        // Add null check before accessing the data field
        if (root->data) {
            DrawText(TextFormat("%d", root->data), x - 12, y - 12, 20, RAYWHITE);
        }

        if (root->left != NULL) {
            drawSearchTree(root->left, x - horizontalSpacing, y + 80, horizontalSpacing / 2, highlightedNode, visitedNodes, numVisitedNodes);
            DrawLine(x, y + NODE_RADIUS, x - horizontalSpacing, y + 80 - NODE_RADIUS, DARKGRAY);
        }

        if (root->right != NULL) {
            drawSearchTree(root->right, x + horizontalSpacing, y + 80, horizontalSpacing / 2, highlightedNode, visitedNodes, numVisitedNodes);
            DrawLine(x, y + NODE_RADIUS, x + horizontalSpacing, y + 80 - NODE_RADIUS, DARKGRAY);
        }
    }
}

//function to search for a value in the tree
/*its parameters: root node,the value we are searching,visited nodes array,number of visited nodes,bool value for if found,bool value for if not found*/
TreeNode* searchTree(TreeNode* root, int searchValue, TreeNode*** visitedNodes, int* numVisitedNodes, bool* valueFound, bool* valueNotFound) {
    *numVisitedNodes = 0;
    *valueFound = false;
    *valueNotFound = false;

    TreeNode* current = root;

    while (current != NULL) {
        //updating the visited nodes array
        *visitedNodes = (TreeNode**)realloc(*visitedNodes, (*numVisitedNodes + 1) * sizeof(TreeNode*));
        (*visitedNodes)[*numVisitedNodes] = current;
        (*numVisitedNodes)++;
        // updating tree drawing
        BeginDrawing();
        drawSearchTree(root, GetScreenWidth() / 2, 100, 220, current, *visitedNodes, *numVisitedNodes);
        EndDrawing();

        Delay(800);

        if (searchValue == current->data) {
            *valueFound = true;
            return current;
        } else if (searchValue < current->data) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    *valueNotFound = true;
    return NULL;
}

//function to delete a node from from the tree, note that it gets replaced in a way that doesnt affect the binary tree balance 
TreeNode* deleteNode(TreeNode* root, int value) {
    if (root == NULL) {
        return root;
    }

    if (value < root->data) {
        root->left = deleteNode(root->left, value);
    } else if (value > root->data) {
        root->right = deleteNode(root->right, value);
    } else {
        if (root->left == NULL) {
            TreeNode* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            TreeNode* temp = root->left;
            free(root);
            return temp;
        }

        // Node with two children, find the larger value between left and right children
        TreeNode* left = root->left;
        while (left->right != NULL) {
            left = left->right;
        }

        TreeNode* right = root->right;
        while (right->left != NULL) {
            right = right->left;
        }

        // Compare left and right, replace the value with the larger one
        if (left->data > right->data) {
            root->data = left->data;
            root->left = deleteNode(root->left, left->data); // Update left child
        } else {
            root->data = right->data;
            root->right = deleteNode(root->right, right->data); // Update right child
        }
    }

    return root;
}
 
// function to insert a new node to the binary tree
TreeNode* insertNode(TreeNode* root, int value) {
    if (root == NULL) {
        TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
        newNode->data = value;
        newNode->left = newNode->right = NULL;
        return newNode;
    }

    if (value < root->data) {
        root->left = insertNode(root->left, value);
    } else if (value > root->data) {
        root->right = insertNode(root->right, value);
    }

    return root;
}

// function to free the binary tree
void freeTree(TreeNode* root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

//function to generate a random binary tree
/*its paramaters : the root node , n number of nodes, max levels as in the max hight of the tree*/
void generateTree(TreeNode** root, int n, int maxLevels) {
    //if n or hight is less or equals 0 returns NULL
    if (n <= 0 || maxLevels <= 0) {
        *root = NULL;
        return;
    }
    //creating root
    *root = (TreeNode*)malloc(sizeof(TreeNode));
    (*root)->data = GetRandomValue(1, 100);
    (*root)->left = (*root)->right = NULL;
    //creating the rest of the tree
    for (int i = 1; i < n; i++) {
        //get a random value for the node from 0 to 100 
        int value = GetRandomValue(1, 100);
        TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
        newNode->data = value;
        newNode->left = newNode->right = NULL;

        TreeNode* current = *root;
        int level = 1;
        //choosing the order of the binary tree based on the node's value , a bigger value node becomes a right child and a smaller value node becomes a left child
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

            // checks if the hight of the tree reaches the max wanted hight
            if (level >= maxLevels) {
                break;
            }
        }
        
    }

}

int main(void){
    InitWindow(1000, 800, "Binary Tree Visualization");
    SetTargetFPS(60);

    //variables
    TreeNode* root = NULL;
    TreeNode** visitedNodes = NULL;
    int numVisitedNodes = 0;
    bool valueFound = false;
    bool valueNotFound = false;
    char inputText1[4] = "";
    char inputText2[4] = "";
    char inputText3[4] = "";
    bool isMouseOverRect1 = false;
    bool isMouseOverRect2 = false;
    bool isMouseOverRect3 = false;
    bool showMessage = false;

    //main loop
    while (!WindowShouldClose()) {
        //setting mouse position tracking variables
        Vector2 mousePoint = GetMousePosition();
        isMouseOverRect1 = CheckCollisionPointRec(mousePoint, (Rectangle){10, (float)GetScreenHeight() - 150, 470, 40});
        isMouseOverRect2 = CheckCollisionPointRec(mousePoint, (Rectangle){10, (float)GetScreenHeight() - 50, 470, 40});
        isMouseOverRect3 = CheckCollisionPointRec(mousePoint, (Rectangle){10, (float)GetScreenHeight() - 100, 470, 40});
        //search and delete nodes
        TreeNode* foundNode;
        TreeNode* foundDeleteNode;

        //checks if mouse is over the search rectangle
        if (isMouseOverRect1) {
            int key = GetKeyPressed();
            if (key != 0) {
                if (key == KEY_ENTER) {
                    int searchValue = atoi(inputText1);
                    inputText1[0] = '\0';
                    foundNode = searchTree(root, searchValue, &visitedNodes, &numVisitedNodes, &valueFound, &valueNotFound);
                    showMessage = true;
                } else if (key == KEY_BACKSPACE) {
                    int length = TextLength(inputText1);
                    if (length > 0) {
                        inputText1[length - 1] = '\0';
                    }
                } else if ((key >= KEY_ZERO && key <= KEY_NINE) || (key >= KEY_KP_0 && key <= KEY_KP_9)) {
                    int length = TextLength(inputText1);
                    if (length < sizeof(inputText1) - 1) {
                        inputText1[length] = (char)key;
                        inputText1[length + 1] = '\0';
                    }
                }
            }
        }

        //checks if mouse is over delete rectangle
        if (isMouseOverRect2) {
            int key = GetKeyPressed();
            if (key != 0) {
                if (key == KEY_ENTER) {
                    int deleteValue = atoi(inputText2);
                    inputText2[0] = '\0';
                    foundDeleteNode = searchTree(root, deleteValue, &visitedNodes, &numVisitedNodes, &valueFound, &valueNotFound);

                    if (foundDeleteNode != NULL) {
                        deleteNode(root, deleteValue);
                        showMessage = true;}
                    
                } else if (key == KEY_BACKSPACE) {
                    int length = TextLength(inputText2);
                    if (length > 0) {
                        inputText2[length - 1] = '\0';
                    }
                } else if ((key >= KEY_ZERO && key <= KEY_NINE) || (key >= KEY_KP_0 && key <= KEY_KP_9)) {
                    int length = TextLength(inputText2);
                    if (length < sizeof(inputText2) - 1) {
                        inputText2[length] = (char)key;
                        inputText2[length + 1] = '\0';
                    }
                }
            }
        }

        //checks if mouse is over insert rectangle
        if (isMouseOverRect3) {
            int key = GetKeyPressed();
            if (key != 0) {
                if (key == KEY_ENTER) {
                    int insertValue = atoi(inputText3);
                    inputText3[0] = '\0';
                    insertNode(root, insertValue);

                } else if (key == KEY_BACKSPACE) {
                    int length = TextLength(inputText3);
                    if (length > 0) {
                        inputText3[length - 1] = '\0';
                    }
                } else if ((key >= KEY_ZERO && key <= KEY_NINE) || (key >= KEY_KP_0 && key <= KEY_KP_9)) {
                    int length = TextLength(inputText3);
                    if (length < sizeof(inputText3) - 1) {
                        inputText3[length] = (char)key;
                        inputText3[length + 1] = '\0';
                    }
                }
            }
        }

        //content drawing
        BeginDrawing();

        ClearBackground(GRAY);
        drawSearchTree(root, GetScreenWidth() / 2, 100, 220, NULL, visitedNodes, numVisitedNodes);

        //search results
        if (showMessage) {
            if (valueFound) {
                DrawText("Value Found!", 500, GetScreenHeight() - 140 , 20, GREEN);
            } else if (valueNotFound) {
                DrawText("Value Not Found!", 500, GetScreenHeight() - 140, 20, RED);
            }
        }

        //drawing the menu
        //generating tree sentance
        DrawRectangle(10, GetScreenHeight() - 200, 470, 40, DARKGRAY);
        DrawRectangleLinesEx((Rectangle){10, (float)GetScreenHeight() - 200, 470, 40}, 2, BLACK);
        DrawText("Press 'R' to Generate Binary Tree", 20, GetScreenHeight() - 190, 25, RAYWHITE);
        //search option
        DrawRectangle(10, GetScreenHeight() - 150, 470, 40, isMouseOverRect1 ? ORANGE : DARKGRAY);
        DrawRectangleLinesEx((Rectangle){10, (float)GetScreenHeight() - 150, 470, 40}, 2, BLACK);
        DrawText(TextFormat("Search For a Value : %s", inputText1), 20, GetScreenHeight() - 140, 20, RAYWHITE);
        //insert option
        DrawRectangle(10, GetScreenHeight() - 100, 470, 40, isMouseOverRect3 ? SKYBLUE : DARKGRAY);
        DrawRectangleLinesEx((Rectangle){10, (float)GetScreenHeight() - 100, 470, 40}, 2, BLACK);
        DrawText(TextFormat("Insert a Value : %s", inputText3), 20, GetScreenHeight() - 90, 20, RAYWHITE);
        //delete option, not that theres still a bug not fixed in this option(when deleting the root sometimes it crashes)
        DrawRectangle(10, GetScreenHeight() - 50, 470, 40, isMouseOverRect2 ? RED : DARKGRAY);
        DrawRectangleLinesEx((Rectangle){10, (float)GetScreenHeight() - 50, 470, 40}, 2, BLACK);
        DrawText(TextFormat("Delete a Value : %s", inputText2), 20, GetScreenHeight() - 40, 20, RAYWHITE);

        EndDrawing();
        //checking for key R to generate tree
        if (IsKeyPressed(KEY_R)) {
            freeTree(root);
            int newNodeCount = GetRandomValue(3, 10);
            generateTree(&root, newNodeCount, 5);
            showMessage = false;
            valueFound = false;
            valueNotFound = false;
        }
    }

    CloseWindow();
    //freeing the binary tree and the visited nodes array
    freeTree(root);
    free(visitedNodes);

    return 0;
}


