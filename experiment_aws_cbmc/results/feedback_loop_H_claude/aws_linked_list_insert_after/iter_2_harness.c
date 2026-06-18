#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_linked_list_insert_after_harness() {
    /* Allocate concrete nodes for 'after' and its next node */
    struct aws_linked_list_node *after = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after != NULL);

    struct aws_linked_list_node *after_next = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after_next != NULL);

    /* Wire up the bidirectional link between after and after_next */
    after->next = after_next;
    after_next->prev = after;

    /* after->prev can be anything non-null for validity, but we just need next_is_valid */
    struct aws_linked_list_node *after_prev = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after_prev != NULL);
    after->prev = after_prev;
    after_prev->next = after;

    /* after_next->next can be anything for our purposes */
    struct aws_linked_list_node *after_next_next = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after_next_next != NULL);
    after_next->next = after_next_next;
    after_next_next->prev = after_next;

    /* Allocate the node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* Ensure to_add is distinct from after and after_next */
    __CPROVER_assume(to_add != after);
    __CPROVER_assume(to_add != after_next);
    __CPROVER_assume(to_add != after_prev);
    __CPROVER_assume(to_add != after_next_next);

    /* Verify preconditions hold */
    assert(aws_linked_list_node_next_is_valid(after));

    /* Save old state */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;

    /* Call function under test */
    aws_linked_list_insert_after(after, to_add);

    /* Assert postconditions */
    assert(to_add->prev == after);
    assert(to_add->next == old_after_next);
    assert(after->next == to_add);
    assert(old_after_next->prev == to_add);
    assert(after->prev == old_after_prev);

    /* Bidirectional linkage: after <-> to_add */
    assert(after->next == to_add);
    assert(to_add->prev == after);

    /* Bidirectional linkage: to_add <-> old_after_next */
    assert(to_add->next == old_after_next);
    assert(old_after_next->prev == to_add);

    assert(aws_linked_list_node_next_is_valid(after));
    assert(aws_linked_list_node_prev_is_valid(to_add));
    assert(aws_linked_list_node_next_is_valid(to_add));
    assert(aws_linked_list_node_prev_is_valid(old_after_next));
}
