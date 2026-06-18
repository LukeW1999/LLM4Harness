#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_next_harness() {
    /* 1. Declare and set up a linked list node */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* Create a next node that node->next points to */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(next_node != NULL);

    /* Set up the node's next pointer to point to next_node */
    node->next = next_node;

    /* node->prev can be anything (not used by this function) */
    /* next_node's pointers can be anything */

    /* 2. Save old state */
    struct aws_linked_list_node *old_next = node->next;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 4. Assert postconditions */
    /* The function returns node->next */
    assert(result == old_next);
    assert(result == next_node);

    /* 5. Assert unchanged fields */
    /* node->next should not have been modified */
    assert(node->next == old_next);

    /* 6. The result is the next pointer of the node */
    assert(result == node->next);
}
