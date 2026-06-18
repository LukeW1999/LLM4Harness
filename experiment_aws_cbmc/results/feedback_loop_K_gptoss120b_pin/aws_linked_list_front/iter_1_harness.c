/*  
Preconditions:  
- `list` is a non‑null pointer.  
- `list` points to a properly initialized `struct aws_linked_list` that satisfies `aws_linked_list_is_valid(list)`.  

Postconditions (validity):  
- The function does not modify the list; after the call `aws_linked_list_is_valid(list)` still holds.  
- The returned pointer `rval` is equal to `list->head.next`.  
- If the list is empty (`list->head.next == &list->tail`), then `rval == &list->tail`.  
- Otherwise `rval` is a node that is in the list, i.e., `rval->prev == &list->head` and `aws_linked_list_node_is_in_list(rval)` is true.  

Postconditions (frame):  
- No memory locations other than those read (`list->head.next`) are modified.  
*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_front_harness(void) {
    /* Allocate and initialize a list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Non‑deterministically populate the list with a bounded number of nodes */
    const size_t max_nodes = 5;
    size_t num_nodes = nondet_size_t();
    __CPROVER_assume(num_nodes <= max_nodes);

    for (size_t i = 0; i < num_nodes; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        aws_linked_list_push_back(&list, node);
    }

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Record the original head.next pointer (the value that should be returned) */
    struct aws_linked_list_node *original_head_next = list.head.next;

    /* Call the function under test */
    struct aws_linked_list_node *rval = aws_linked_list_front(&list);

    /* Verify that the list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* Verify that the function did not modify the list structure */
    assert(list.head.next == original_head_next);
    assert(list.head.prev == &list.tail);
    assert(list.tail.next == &list.head);
    assert(list.tail.prev == original_head_next);

    /* Verify the returned pointer */
    if (original_head_next == &list.tail) {
        /* Empty list case */
        assert(rval == &list.tail);
    } else {
        /* Non‑empty list case */
        assert(rval != NULL);
        assert(rval->prev == &list.head);
        assert(aws_linked_list_node_is_in_list(rval));
    }

    /* Clean up allocated nodes */
    struct aws_linked_list_node *cur = aws_linked_list_begin(&list);
    while (cur != aws_linked_list_end(&list)) {
        struct aws_linked_list_node *next = aws_linked_list_next(cur);
        free(cur);
        cur = next;
    }

    return 0;
}
