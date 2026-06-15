#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate two distinct nodes */
    struct aws_linked_list_node after_node;
    struct aws_linked_list_node to_add_node;
    __CPROVER_assume(&after_node != &to_add_node);

    /* 3. Initialise the list with a single element (after_node) */
    /* list.head <-> after_node <-> list.tail */
    list.head.next = &after_node;
    list.head.prev = &list.tail;               /* sentinel linkage, not used by insert_after */
    after_node.prev = &list.head;
    after_node.next = &list.tail;
    list.tail.prev = &after_node;
    list.tail.next = &list.head;               /* sentinel linkage, not used by insert_after */

    /* 4. Ensure the list is still valid after manual construction */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 5. Reset the node to be inserted and ensure it is not linked */
    aws_linked_list_node_reset(&to_add_node);
    __CPROVER_assume(to_add_node.next == NULL);
    __CPROVER_assume(to_add_node.prev == NULL);

    /* 6. Additional pre‑condition: after_node is not the tail sentinel */
    __CPROVER_assume(after_node.next != &list.tail);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&after_node));

    /* 7. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_after = after_node;
    struct aws_linked_list_node old_to_add = to_add_node;
    struct aws_linked_list_node *old_after_next = after_node.next;

    /* 8. Call the function under test */
    aws_linked_list_insert_after(&after_node, &to_add_node);

    /* 9. Post‑conditions: fields that must have changed */
    assert(after_node.next == &to_add_node);                     /* after now points to new node */
    assert(to_add_node.prev == &after_node);                     /* new node points back to after */
    assert(to_add_node.next == old_after_next);                  /* new node links to former successor */
    assert(old_after_next->prev == &to_add_node);                /* former successor now points back to new node */

    /* 10. Unchanged fields (list head/tail and unrelated nodes) */
    assert(list.head.next == old_list.head.next);                /* head.next unchanged (after_node still first) */
    assert(list.tail.prev == old_list.tail.prev);                /* tail.prev unchanged (after_node not last) */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(after_node.prev == old_after.prev);                   /* after_node.prev unchanged */
    assert(to_add_node.prev != old_to_add.prev);                 /* to_add_node.prev changed as expected */
    assert(to_add_node.next != old_to_add.next);                 /* to_add_node.next changed as expected */

    /* 11. Validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
