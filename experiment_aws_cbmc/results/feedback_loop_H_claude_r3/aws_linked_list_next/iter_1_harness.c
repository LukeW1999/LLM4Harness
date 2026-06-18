#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_next_harness() {
    /* 1. Declare and set up a linked list node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* node->next must be a valid pointer (non-null) for the function to return */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    node->next = next_node;
    node->prev = NULL; /* prev not used by this function */

    /* 2. Save old state */
    struct aws_linked_list_node *old_next = node->next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 4. Assert postconditions */
    /* The function returns node->next */
    assert(result == node->next);
    assert(result == old_next);
    assert(result == next_node);

    /* 5. Assert unchanged fields */
    /* node->next should not be modified by this function */
    assert(node->next == old_next);
    /* node->prev should not be modified */
    assert(node->prev == NULL);
}
