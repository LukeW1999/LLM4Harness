#include <assert.h>
#include <stdlib.h>
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. The list must contain at least one real node (cannot insert before head sentinel) */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Choose a node that is currently in the list to serve as 'before' */
    struct aws_linked_list_node *before = list.head.next; /* first real node */
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before != &list.tail); /* must be a real node, not the tail sentinel */

    /* 4. Allocate a new node that is not currently in any list */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    /* Ensure the node is not linked */
    to_add->next = NULL;
    to_add->prev = NULL;
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 5. Save old state for immutability checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 7. Post‑condition: the list must remain valid */
    assert(aws_linked_list_is_valid(&list));

    /* 8. The new node must be correctly linked */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(before->prev == to_add);
    assert(old_before_prev->next == to_add);
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 9. Unchanged fields of the list structure (sentinel links) */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    /* 10. The 'before' node's next pointer must be unchanged */
    assert(before->next == old_before_next);
}
