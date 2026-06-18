#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_remove_harness() {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node and insert it into the list */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_push_back(&list, node);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Save old state of the list (sentinel nodes) */
    struct aws_linked_list old = list;

    /* 4. Call the function under test */
    aws_linked_list_remove(node);

    /* 5. Post‑conditions */

    /* The removed node's pointers are cleared */
    assert(node->next == NULL);
    assert(node->prev == NULL);

    /* The node is no longer considered part of any list */
    assert(!aws_linked_list_node_is_in_list(node));

    /* The list remains a valid doubly‑linked list */
    assert(aws_linked_list_is_valid(&list));

    /* Sentinel nodes themselves (addresses) are unchanged */
    assert(&list.head == &old.head);
    assert(&list.tail == &old.tail);
}
