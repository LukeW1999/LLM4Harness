/*  
Preconditions:  
- __CPROVER_assume(node != NULL);  
- __CPROVER_assume(node->prev != NULL && node->next != NULL);  
- __CPROVER_assume(aws_linked_list_is_valid_deep(list)); // the list containing node is valid  
- __CPROVER_assume(aws_linked_list_node_is_in_list(node)); // node is part of the list  

Postconditions (validity):  
- After the call, node->prev == NULL && node->next == NULL (node is reset).  
- The list remains a valid doubly‑linked list: aws_linked_list_is_valid_deep(list) == true.  

Postconditions (length):  
- The number of elements in the list is decreased by exactly one.  

Postconditions (frame):  
- All other nodes in the list retain their original next and prev pointers (i.e., memory locations of other nodes are unchanged).  
*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_NODES 5

static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    struct aws_linked_list_node *cur = aws_linked_list_begin(list);
    const struct aws_linked_list_node *end = aws_linked_list_end(list);
    while (cur != end) {
        len++;
        cur = aws_linked_list_next(cur);
    }
    return len;
}

void aws_linked_list_remove_harness(void) {
    /* Allocate and initialize a list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Allocate an array of nodes */
    struct aws_linked_list_node nodes[MAX_NODES];
    struct aws_linked_list_node before_state[MAX_NODES];

    /* Nondeterministically decide how many nodes to put in the list (1..MAX_NODES) */
    size_t n;
    __CPROVER_assume(n >= 1 && n <= MAX_NODES);

    /* Insert nodes into the list */
    for (size_t i = 0; i < n; ++i) {
        aws_linked_list_node_reset(&nodes[i]);   // ensure clean state before insertion
        aws_linked_list_push_back(&list, &nodes[i]);
    }

    /* Choose a nondeterministic node to remove */
    size_t idx;
    __CPROVER_assume(idx < n);
    struct aws_linked_list_node *node_to_remove = &nodes[idx];

    /* Record pre‑state of all nodes */
    for (size_t i = 0; i < n; ++i) {
        before_state[i] = nodes[i];
    }

    size_t len_before = list_length(&list);

    /* Preconditions for the proof */
    __CPROVER_assume(node_to_remove != NULL);
    __CPROVER_assume(node_to_remove->prev != NULL);
    __CPROVER_assume(node_to_remove->next != NULL);
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));
    __CPROVER_assume(aws_linked_list_node_is_in_list(node_to_remove));

    /* Call the function under test */
    aws_linked_list_remove(node_to_remove);

    /* Postconditions */
    /* Node is reset */
    assert(node_to_remove->prev == NULL);
    assert(node_to_remove->next == NULL);

    /* List remains valid */
    assert(aws_linked_list_is_valid_deep(&list));

    /* Length decreased by one */
    size_t len_after = list_length(&list);
    assert(len_after + 1 == len_before);

    /* Frame condition: other nodes unchanged */
    for (size_t i = 0; i < n; ++i) {
        if (i == idx) continue; /* skip the removed node */
        assert(nodes[i].prev == before_state[i].prev);
        assert(nodes[i].next == before_state[i].next);
    }

    return 0;
}
