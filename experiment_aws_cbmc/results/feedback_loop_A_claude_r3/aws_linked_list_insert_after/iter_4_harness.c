#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    /* Allocate nodes with non-deterministic but valid structure */
    struct aws_linked_list_node *after = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *after_next = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));

    __CPROVER_assume(after != NULL);
    __CPROVER_assume(after_next != NULL);
    __CPROVER_assume(to_add != NULL);

    /* All nodes must be distinct */
    __CPROVER_assume(after != after_next);
    __CPROVER_assume(after != to_add);
    __CPROVER_assume(after_next != to_add);

    /* Set up the bidirectional link: after <-> after_next */
    after->next = after_next;
    after_next->prev = after;

    /* after_next->next must be valid for aws_linked_list_node_next_is_valid(after) */
    /* aws_linked_list_node_next_is_valid(after) checks after->next->prev == after */
    /* which is after_next->prev == after - already set above */

    /* after must have a valid prev pointer */
    struct aws_linked_list_node *after_prev = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after_prev != NULL);
    __CPROVER_assume(after_prev != after);
    __CPROVER_assume(after_prev != after_next);
    __CPROVER_assume(after_prev != to_add);
    after->prev = after_prev;
    after_prev->next = after;

    /* after_next needs valid next pointer for aws_linked_list_node_next_is_valid(after_next) */
    struct aws_linked_list_node *after_next_next = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(after_next_next != NULL);
    __CPROVER_assume(after_next_next != after);
    __CPROVER_assume(after_next_next != after_next);
    __CPROVER_assume(after_next_next != to_add);
    __CPROVER_assume(after_next_next != after_prev);
    after_next->next = after_next_next;
    after_next_next->prev = after_next;

    /* Set up to_add with valid next and prev pointers */
    struct aws_linked_list_node *to_add_next = malloc(sizeof(struct aws_linked_list_node));
    struct aws_linked_list_node *to_add_prev = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add_next != NULL);
    __CPROVER_assume(to_add_prev != NULL);
    to_add->next = to_add_next;
    to_add->prev = to_add_prev;
    /* aws_linked_list_node_next_is_valid(to_add): to_add->next->prev == to_add */
    to_add_next->prev = to_add;

    /* Save old state before calling */
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
}
