#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness(void) {
    /* 1. Declare and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Declare a node to be inserted */
    struct aws_linked_list_node node;
    struct aws_linked_list_node *node_ptr = &node; /* non‑NULL as required */

    /* 3. Save old state that may change */
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node_ptr);

    /* 5. Post‑condition assertions */

    /* The new node becomes the last element */
    assert(list.tail.prev == node_ptr);
    /* The new node links back to the previous last element */
    assert(node_ptr->prev == old_last);
    /* The new node links forward to the tail sentinel */
    assert(node_ptr->next == &list.tail);
    /* The previous last element now points forward to the new node */
    assert(node_ptr->prev->next == node_ptr);

    /* Unchanged sentinel invariants */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* List validity must still hold */
    assert(aws_linked_list_is_valid(&list));
}
