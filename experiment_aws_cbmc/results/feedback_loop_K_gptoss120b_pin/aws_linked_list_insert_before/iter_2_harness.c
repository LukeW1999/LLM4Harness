/*  
Preconditions:  
- __CPROVER_assume(before != NULL);  
- __CPROVER_assume(to_add != NULL);  
- __CPROVER_assume(aws_linked_list_is_valid(&list));  
- __CPROVER_assume(aws_linked_list_is_valid_deep(&list));  
- __CPROVER_assume(aws_linked_list_node_is_in_list(before));  
- __CPROVER_assume(to_add->next == NULL && to_add->prev == NULL); // to_add not in any list  

Postconditions (validity):  
- assert(aws_linked_list_is_valid(&list));  
- assert(aws_linked_list_is_valid_deep(&list));  

Postconditions (length):  
- let len_before be the number of elements in list before the call;  
- let len_after be the number of elements after the call;  
- assert(len_after == len_before + 1);  

Postconditions (frame):  
- All nodes other than `before` and `to_add` retain their original `next` and `prev` pointers.  
*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/* Helper to compute list length */
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

/* Helper to pick a nondet node from the list */
static struct aws_linked_list_node *pick_node(const struct aws_linked_list *list) {
    struct aws_linked_list_node *cur = aws_linked_list_begin(list);
    const struct aws_linked_list_node *end = aws_linked_list_end(list);
    struct aws_linked_list_node *chosen = NULL;
    while (cur != end) {
        if (__CPROVER_nondet_bool()) {
            chosen = cur;
            break;
        }
        cur = aws_linked_list_next(cur);
    }
    /* If none selected nondet, default to first element (list non‑empty) */
    if (chosen == NULL && cur != end) {
        chosen = cur;
    }
    return chosen;
}

/* Harness */
void aws_linked_list_insert_before_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Build a nondet length list (0..5 elements) */
    size_t max_nodes = 5;
    size_t i;
    for (i = 0; i < max_nodes; ++i) {
        if (!__CPROVER_nondet_bool()) {
            break;
        }
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        aws_linked_list_push_back(&list, node);
    }

    /* Ensure the list is non‑empty for insert_before */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* Capture pre‑state length */
    size_t len_before = list_length(&list);

    /* Pick a valid 'before' node from the list */
    struct aws_linked_list_node *before = pick_node(&list);
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(aws_linked_list_node_is_in_list(before));

    /* Allocate a node to add, ensure it is not linked */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(to_add->next == NULL && to_add->prev == NULL);

    /* Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* Post‑condition checks */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    size_t len_after = list_length(&list);
    assert(len_after == len_before + 1);

    /* Verify that to_add is now correctly linked */
    assert(to_add->next == before);
    assert(before->prev == to_add);
    assert(to_add->prev->next == to_add);
}
