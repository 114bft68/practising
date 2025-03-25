#include <stdio.h>
#include <stdlib.h>

typedef struct n {
    struct n *prev;
    void     *data;
    struct n *next;
} node;

node* createNode(void* data); // create a node    (NULL on error)
int   removeNode(node* addr); // delete a node    (1 on error)
int   rmAllNodes(void);       // delete all nodes (1 on error)

node* CurrentNode = NULL;     // the last node

/* -------------------------------------------------- SEPERATING LINE -------------------------------------------------- */
/* --------------------------------------------------    FUNCTIONS    -------------------------------------------------- */
node* createNode(void* data) {

    node *new = malloc(sizeof(node));
    if (!new) {
        puts("Memory allocation failed");
        return NULL;
    }

    if (CurrentNode) CurrentNode->next = new;
    CurrentNode = new;

    *new = (node) {
        .prev = CurrentNode,
        .data = data,
        .next = NULL
    };

    return CurrentNode;

}

int removeNode(node *addr) {

    if (addr) {

        if (addr->prev) addr->prev->next = addr->next;

        if (addr->next)
        {
            addr->next->prev = addr->prev;
        }
        else
        {
            CurrentNode = addr->prev; // if addr->next has no address, it's the last item of the linked list (CurrentNode)
        }
        
        free(addr);
        addr = NULL;

        return 0;

    }

    return 1;

}

int rmAllNodes(void) {
    
    if (CurrentNode) {
        
        node* pNode = CurrentNode;

        while (pNode->prev) {

            pNode = pNode->prev;
            
            free(pNode->next);
            pNode->next = NULL;

        }

        free(pNode);
        pNode = NULL;

        return 0;

    }

    return 1;

}
/* --------------------------------------------------    FUNCTIONS    -------------------------------------------------- */
/* -------------------------------------------------- SEPERATING LINE -------------------------------------------------- */

int main(void) {

    // int main(void) exists so that this .c file can be compiled (to make sure there are no compiler warnings or errors)
    return 0;
}