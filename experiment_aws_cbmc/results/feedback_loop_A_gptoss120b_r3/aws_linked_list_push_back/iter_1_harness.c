#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_push_back_harness(void) {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a non‑NULL node */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Assert postconditions */
    /* The new node becomes the last element */
    assert(list.tail.prev == node);
    /* Sentinel links remain unchanged */
    assert(list.tail.next == NULL);
    assert(list.head.prev == NULL);
    assert(list.head.next == old.head.next);
    /* New node links are correct */
    assert(node->next == &list.tail);
    assert(node->prev == old_last);
    /* Previous last node now points forward to the new node */
    assert(old_last->next == node);
    /* List validity invariant holds */
    assert(aws_linked_list_is_valid(&list));
}
