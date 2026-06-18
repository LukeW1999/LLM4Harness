#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

void aws_linked_list_insert_after_harness() {
    /* Initialize a valid linked list with a nondeterministic number of nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Choose the node after which to insert. We use the head sentinel for simplicity.
       In real code, after could be any node in the list. Using head is sufficient to
       cover the case where after is the very first node before any data node. */
    struct aws_linked_list_node *after = &list.head;

    /* Allocate the node to insert */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);

    /* Save the old next pointer of after, needed for checking proper linkage */
    struct aws_linked_list_node *old_after_next = after->next;

    /* Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* Postconditions */

    /* 1. The list remains a valid doubly linked list (deep check) */
    assert(aws_linked_list_is_valid_deep(&list));

    /* 2. The insertion was performed correctly */
    assert(after->next == to_add);
    assert(to_add->prev == after);
    assert(to_add->next == old_after_next);
    assert(old_after_next->prev == to_add);
}
