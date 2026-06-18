#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_next_harness() {
    /* 1. Declare and set up a linked list node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* node->next must be a valid pointer (non-null) for the precondition */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    /* next_node->prev must also be valid for the postcondition */
    struct aws_linked_list_node *prev_of_next = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(prev_of_next != NULL);

    node->next = next_node;
    node->prev = NULL;

    next_node->next = NULL;
    next_node->prev = prev_of_next;

    /* prev_of_next needs valid pointers too to avoid cascading issues */
    prev_of_next->next = NULL;
    prev_of_next->prev = NULL;

    /* 2. Save old state */
    struct aws_linked_list_node *old_next = node->next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 4. Assert postconditions */
    assert(result == node->next);
    assert(result == old_next);
    assert(result == next_node);

    /* 5. Assert unchanged fields */
    assert(node->next == old_next);
}
