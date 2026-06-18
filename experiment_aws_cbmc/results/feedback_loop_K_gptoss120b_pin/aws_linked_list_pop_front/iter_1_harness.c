/*=== Contract for aws_linked_list_pop_front ===
Preconditions:
  - __CPROVER_assume(list != NULL);
  - __CPROVER_assume(aws_linked_list_is_valid(list));
  - __CPROVER_assume(!aws_linked_list_empty(list));
Postconditions (validity):
  - returned node != NULL;
  - returned node's next == NULL && returned node's prev == NULL;
  - aws_linked_list_is_valid(list) holds after the call;
Postconditions (length):
  - let len_before be the number of user nodes in list before the call;
  - let len_after be the number of user nodes in list after the call;
  - assert(len_after == len_before - 1);
Postconditions (frame):
  - No memory outside of the list structure and the popped node is modified.
===*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/* Helper to compute list length (number of user nodes) */
static size_t list_length(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail) {
        cnt++;
        cur = cur->next;
    }
    return cnt;
}

/* Helper to allocate a node and reset it */
static struct aws_linked_list_node *alloc_node(void) {
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);
    return node;
}

/* Harness */
void aws_linked_list_pop_front_harness(void) {
    /* Initialize list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Non-deterministically decide number of elements (at least 1) */
    size_t n;
    __CPROVER_assume(n > 0);
    __CPROVER_assume(n < 10); /* bound to keep verification tractable */

    /* Populate list with n nodes */
    for (size_t i = 0; i < n; ++i) {
        struct aws_linked_list_node *node = alloc_node();
        aws_linked_list_push_back(&list, node);
    }

    /* Record pre-state length */
    size_t len_before = list_length(&list);

    /* Preconditions for the function under test */
    __CPROVER_assume(!aws_linked_list_empty(&list));
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Call the function */
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    /* Postconditions */
    assert(popped != NULL);
    assert(popped->next == NULL);
    assert(popped->prev == NULL);
    assert(aws_linked_list_is_valid(&list));

    size_t len_after = list_length(&list);
    assert(len_after + 1 == len_before);

    return 0;
}
