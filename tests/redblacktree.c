#include <stdio.h>
#include <stdlib.h>

typedef enum { RED, BLACK } NodeColor;

typedef struct RBNode {
    int data;
    struct RBNode *left, *right, *parent;
    NodeColor color;
} RBNode;

RBNode* createNode(int data) {
    RBNode* newNode = (RBNode*)malloc(sizeof(RBNode));
    if (newNode) {
        newNode->data = data;
        newNode->left = newNode->right = newNode->parent = NULL;
        newNode->color = RED;
    }
    return newNode;
}

void rotateLeft(RBNode **root, RBNode *x) {
    RBNode *y = x->right;
    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL) {
        *root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rotateRight(RBNode **root, RBNode *y) {
    RBNode *x = y->left;
    y->left = x->right;
    if (x->right != NULL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == NULL) {
        *root = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}

// Fix violations after insertion
void fixViolation(RBNode **root, RBNode *z) {
    while (z != *root && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBNode *y = z->parent->parent->right;
            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rotateLeft(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rotateRight(root, z->parent->parent);
            }
        } else {
            // Same as above, with "right" and "left" exchanged
            // ...
        }
    }
    (*root)->color = BLACK;
}

void insert(RBNode **root, RBNode *z) {
    RBNode *y = NULL;
    RBNode *x = *root;

    // Standard BST insertion
    while (x != NULL) {
        y = x;
        if (z->data < x->data)
            x = x->left;
        else
            x = x->right;
    }
    z->parent = y;

    if (y == NULL)
        *root = z;
    else if (z->data < y->data)
        y->left = z;
    else
        y->right = z;

    // Fix red black tree violations
    fixViolation(root, z);
}

void inorder(RBNode *root) {
    if (root == NULL) return;
    inorder(root->left);
    printf("%d ", root->data);
    inorder(root->right);
}

int main() {
    RBNode *root = NULL;

    // Test insertion
    insert(&root, createNode(10));
    insert(&root, createNode(20));
    // ... More insertions

    inorder(root); // In-order traversal to print the tree
    // Don't forget to free the tree nodes to avoid memory leaks

    return 0;
}

