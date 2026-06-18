#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_remove_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Ensure the list is non‑empty so there is a removable node */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Choose a node that is definitely in the list (first real node) */
    struct aws_linked_list_node *node = list.head.next;
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.tail);   /* not a sentinel */

    /* Save surrounding pointers for post‑condition checks */
    struct aws_linked_list_node *prev = node->prev;
    struct aws_linked_list_node *next = node->next;

    /* 4. Call the function under test */
    aws_linked_list_remove(node);

    /* 5. Post‑conditions */

    /* 5.1. The removed node is reset */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* 5.2. The surrounding nodes are now linked directly */
    assert(prev->next == next);
    assert(next->prev == prev);

    /* 5.3. The node is no longer considered part of any list */
    assert(!aws_linked_list_node_is_in_list(node));

    /* 5.4. The list remains a valid doubly‑linked list */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
