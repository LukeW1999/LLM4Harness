/*  
Preconditions:  
- __CPROVER_assume(after != NULL);  
- __CPROVER_assume(to_add != NULL);  
- __CPROVER_assume(after->next != NULL);            // after must have a successor node  
- __CPROVER_assume(aws_linked_list_is_valid(&list)); // the list containing 'after' is valid  
- __CPROVER_assume(aws_linked_list_node_is_in_list(after)); // 'after' is part of the list  
- __CPROVER_assume(to_add->next == NULL && to_add->prev == NULL); // 'to_add' is not in any list  

Postconditions (validity):  
- assert(to_add->prev == after);  
- assert(to_add->next == old_next); // old_next = after's original next node  
- assert(after->next == to_add);  
- assert(old_next->prev == to_add);  
- assert(aws_linked_list_is_valid(&list));  

Postconditions (length):  
- assert(list_length_after == list_length_before + 1);  

Postconditions (frame):  
- All other nodes' next and prev pointers remain unchanged.  
*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

/* Helper to compute list length (excluding sentinel nodes) */
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

/* Helper to capture the original next node of 'after' */
static struct aws_linked_list_node *capture_original_next(struct aws_linked_list_node *after) {
    return after->next;
}

/* Proof harness for aws_linked_list_insert_after */
void aws_linked_list_insert_after_harness(void) {
    /* Allocate and initialize a list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Allocate a node that will be in the list (node1) */
    struct aws_linked_list_node *node1 = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node1 != NULL);
    node1->next = NULL;
    node1->prev = NULL;
    aws_linked_list_push_back(&list, node1);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 'after' points to node1 (which is guaranteed to have a non‑NULL next: the tail sentinel) */
    struct aws_linked_list_node *after = node1;
    __CPROVER_assume(after != NULL);
    __CPROVER_assume(after->next != NULL);               /* tail sentinel */

    /* Capture the original next node of 'after' for post‑condition checks */
    struct aws_linked_list_node *old_next = capture_original_next(after);

    /* Allocate the node to be added */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    to_add->next = NULL;
    to_add->prev = NULL;

    /* Record list length before insertion */
    size_t len_before = list_length(&list);

    /* Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* ---- Postcondition checks ---- */

    /* Validity of pointers after insertion */
    assert(to_add->prev == after);
    assert(to_add->next == old_next);
    assert(after->next == to_add);
    assert(old_next->prev == to_add);

    /* List structural validity */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* Length increased by exactly one */
    size_t len_after = list_length(&list);
    assert(len_after == len_before + 1);

    /* Frame condition: all other nodes unchanged */
    /* Verify that node1's prev remains unchanged (it should still point to head sentinel) */
    assert(node1->prev == aws_linked_list_begin(&list)->prev);
    /* Verify that the tail sentinel's prev now points to the newly added node */
    const struct aws_linked_list_node *tail = aws_linked_list_end(&list);
    assert(tail->prev == to_add);
}
