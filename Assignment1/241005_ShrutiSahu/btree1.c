#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define ORDER 4

int t=3;
typedef struct { 
    int key; 
    char *value; 
} Pair;


typedef struct BTreeNode {

    Pair *keys;

    struct BTreeNode **children;
    int n;  //(no of keys)
    bool is_leaf;
} BTreeNode;


typedef struct BTree {
    BTreeNode *root;
} BTree;


void fill(BTreeNode *node, int idx);
void borrowFromPrev(BTreeNode *node, int idx);
void borrowFromNext(BTreeNode *node, int idx);
Pair getPred(BTreeNode *node, int idx);  ///
Pair getSucc(BTreeNode *node, int idx);  ///
void deleteFromNode(BTreeNode *node, int key);
void deleteKey(BTree *tree, int key) ;
void deleteFromNode(BTreeNode *node, int key);
void merge(BTreeNode *node, int idx);

BTreeNode *createNode( bool leaf) {
    BTreeNode *node = (BTreeNode *)malloc(sizeof(BTreeNode));
    // node->t = t;
    node->is_leaf = leaf;
    node->n = 0;
    node->keys = (Pair *)malloc(4 * sizeof(Pair));
    node->children = (BTreeNode **)malloc(5 * sizeof(BTreeNode *));

    for (int i = 0; i < 5; i++) {
        node->children[i] = NULL;
    }
    return node;
} 

void splitChild(BTreeNode *x, int i, BTreeNode *y) {
    // int t = y->t;
    BTreeNode *z = createNode(y->is_leaf);
    // z->n = t - 1;
        z->n =2;


    for (int j = 0; j < 2; j++) 
        z->keys[j] = y->keys[j + 2]; /// z is made after splitting y(z = same level as y)

    if (!y->is_leaf) {
        for (int j = 0; j < t  ; j++)
            z->children[j] = y->children[j + t];
    }

    y->n = t - 1;

    for (int j = x->n; j >= i + 1; j--) ///x = father of y
        x->children[j + 1] = x->children[j];
    x->children[i + 1] = z; // making z as the child of x

    for (int j = x->n - 1; j >= i; j--){
        // x->keys[j + 1].key = x->keys[j].key;
        // x->keys[j + 1].value = x->keys[j].value;
                x->keys[j+1] = x->keys[j];

    }
    x->keys[i] = y->keys[t - 1];
    x->n++;
}



void insertNonFull(BTreeNode *x, int k,char *name) {
    int i = x->n - 1;
    if (x->is_leaf) { 
        while (i >= 0 && x->keys[i].key > k) {
            // x->keys[i + 1].key = x->keys[i].key;
            // x->keys[i + 1].value = strdup(x->keys[i].value);
                        x->keys[i+1] = x->keys[i];


            i--;
        }
        x->keys[i + 1].key = k;  //inserted
        x->keys[i + 1].value = strdup(name);
        x->n++;
    } else {
        while (i >= 0 && x->keys[i].key > k){
            i--;
        }
        
        if (x->children[i+1]->n == 5) {   //if its full
            splitChild(x, i + 1, x->children[i + 1]);
            if (x->keys[i + 1].key < k)
                i++;
        }
        insertNonFull(x->children[i + 1], k,name);
    }
}

BTree* createBTree() {
    BTree* tree = (BTree*)malloc(sizeof(BTree));
    tree->root = NULL;
    // tree->root = createNode(TRUE);
    return tree;
}
bool search(BTreeNode *root, int k) {
    if (!root) return false;
    int i = 0;
    while (i < root->n && k > root->keys[i].key)
        i++;
    if (i < root->n && root->keys[i].key == k)
        return true;
    if (root->is_leaf)
        return false;
    return search(root->children[i], k);
}
void insert(BTree * tree, int k, char *name){
    if(tree -> root == NULL){
        // BTree *tree = malloc(sizeof(BTree));

        tree ->root = createNode(true);
        tree->root->keys[0].key= k;
        tree->root->keys[0].value =strdup(name); 
        tree->root->n = 1;
        
    }else{ ///exists

        if(search(tree->root , k)){
            printf("already exits");
            return;
        }else{

         if (tree->root->n == 4) {          ///creating a node
            BTreeNode *s = createNode(false);  //it isnt leaf
            s->children[0] = tree->root;
            splitChild(s, 0, tree->root);
             int i = 0;
            if (s->keys[0].key < k) {
                i++;
            }
            insertNonFull(s->children[i], k,name);
            tree->root = s;
        } else {
            insertNonFull(tree->root, k, name);
        }

    }
    }
}


void traverse(BTreeNode *root) {
    if (root == NULL) return;
    
    int i;
    for (i = 0; i < root->n; i++) {
        if (!root->is_leaf) {
            traverse(root->children[i]);
        }
        printf("%d: %s\n", root->keys[i].key, root->keys[i].value);
    }
    
    if (!root->is_leaf) {
        traverse(root->children[i]);
    }
}



void freeNode(BTreeNode* node) {
    if (node == NULL) return;


    if (!node->is_leaf) {
        for (int i = 0; i <= node->n; i++) {
            freeNode(node->children[i]);
        }
    }

    for (int i = 0; i < node->n; i++) {
        free(node->keys[i].value); 
    }

    free(node->keys);
    free(node->children);

    
    free(node);
}
void freeTree(BTree* tree) {
    if (tree == NULL) return;
    freeNode(tree->root);
    free(tree);
}


Pair getPred(BTreeNode *node, int idx) {
    BTreeNode *cur = node->children[idx];
    while (!cur->is_leaf){
       cur = cur->children[cur->n];  
    }
    return cur->keys[cur->n - 1];   
}

Pair getSucc(BTreeNode *node, int idx) {
    BTreeNode *cur = node->children[idx+1];
    while (!cur->is_leaf){
       cur = cur->children[0];  
    }
    return cur->keys[0];   
}

void borrowFromPrev(BTreeNode *node, int idx){
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx-1];
      for (int i = child->n - 1; i >= 0; i--) {
        child->keys[i + 1] = child->keys[i];
    }
    if (!child->is_leaf) {
        for (int i = child->n; i >= 0; i--)
            child->children[i + 1] = child->children[i];
    }

    child->keys[0] = node->keys[idx - 1];
    // child->values[0] = node->values[idx - 1];

    if (!child->is_leaf)
        child->children[0] = sibling->children[sibling->n];

    node->keys[idx - 1] = sibling->keys[sibling->n - 1];

    child->n ++;
    sibling->n --;
}

void borrowFromNext(BTreeNode *node, int idx) {
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx + 1];

    child->keys[child->n] = node->keys[idx];

    if (!child->is_leaf)
        child->children[child->n + 1] = sibling->children[0];

    node->keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->n; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
    }

    if (!sibling->is_leaf) {
        for (int i = 1; i <= sibling->n; ++i)
            sibling->children[i - 1] = sibling->children[i];
    }

    child->n++;
    sibling->n--;
}


void fill(BTreeNode *node, int idx) {
    if (idx != 0 && node->children[idx - 1]->n >= 4)
        borrowFromPrev(node, idx);
    else if (idx != node->n && node->children[idx + 1]->n >= 4)
        borrowFromNext(node, idx);
    else {
        if (idx != node->n)
            merge(node, idx);
        else
            merge(node, idx - 1);
    }
}

void deleteFromNode(BTreeNode *node, int key) {
    int idx = 0;
    while (idx < node->n && node->keys[idx].key< key) idx++;

    if (idx < node->n && node->keys[idx].key == key) {
        if (node->is_leaf) {
            for (int i = idx + 1; i < node->n; ++i) {
                node->keys[i - 1] = node->keys[i];
            }
            node->n--;
        } else {
            if (node->children[idx]->n >= 4) {
                Pair pred = getPred(node, idx);
                node->keys[idx] = pred;
                deleteFromNode(node->children[idx], pred.key);
            } else if (node->children[idx + 1]->n >= 4) {
                Pair succ = getSucc(node, idx);
                node->keys[idx] = succ;
                deleteFromNode(node->children[idx + 1], succ.key);
            } else {
                merge(node, idx);
                deleteFromNode(node->children[idx], key);
            }
        }
    } else {
        if (node->is_leaf) return;
        int flag = (idx == node->n);
        if (node->children[idx]->n < 4)
            fill(node, idx);
        if (flag && idx > node->n)
            deleteFromNode(node->children[idx - 1], key);
        else
            deleteFromNode(node->children[idx], key);
    }
}
void merge(BTreeNode *node, int idx) {
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx + 1];

    child->keys[4 - 1] = node->keys[idx];
    // child->values[4 - 1] = node->values[idx];

    for (int i = 0; i < sibling->n; i++) {
        child->keys[i + 4] = sibling->keys[i];
        // child->values[i + 4] = sibling->values[i];
    }
    if (!child->is_leaf) {
        for (int i = 0; i <= sibling->n; i++)
            child->children[i + 4] = sibling->children[i];
    }

    for (int i = idx + 1; i < node->n; i++) {
        node->keys[i - 1] = node->keys[i];
        node->children[i] = node->children[i + 1];
    }
    child->n += sibling->n + 1;
    node->n--;

    free(sibling);
}

void deleteKey(BTree *tree, int key) {
    deleteFromNode(tree->root, key);
    if (tree->root->n == 0) {
        BTreeNode *tmp = tree->root;
        if (!tree->root->is_leaf)
            tree->root = tree->root->children[0];
        else
            tree->root = NULL;
        free(tmp);
    }
}
// int main(){

//     printf("CHECKPOINT 123\n");
//         BTree* tree = createBTree();
//   
//         insert(tree, 8, "naman");        
//         insert(tree, 2, "ridin");
//         insert(tree, 13, "pratham");
           

//         insert(tree, 9, "burhan");
//         insert(tree, 14, "ben");
//         insert(tree, 4, "rohit");
//         insert(tree, 18, "datta");
//     traverse(tree->root);

//         printf("yoyo");
//             printf("\nDeleting 9 (leaf):\n");
//     deleteKey(tree, 9);
    

//     traverse(tree->root);
//     freeTree(tree);

// }

