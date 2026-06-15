#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

/* Harness for aws_linked_list_push_back */
void aws_linked_list_push_back_harness(void) {
    /* 1. Declare and bound the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Declare a node to be inserted */
    struct aws_linked_list_node node;
    /* The node's fields will be overwritten by the function, no need to bound them */

    /* 3. Save old state before the call */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev; /* may be &list.head if list empty */

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, &node);

    /* 5. Post‑condition: list remains a valid linked list */
    assert(aws_linked_list_is_valid(&list));

    /* 6. Post‑condition: sentinel invariants are unchanged */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.prev == old.head.prev);   /* unchanged */
    assert(list.tail.next == old.tail.next);   /* unchanged */

    /* 7. Post‑condition: the new node is correctly linked at the back */
    assert(list.tail.prev == &node);               /* tail now points to the new node */
    assert(node.next == &list.tail);               /* node's next points to tail sentinel */
    assert(node.prev == old_last);                 /* node's prev points to the previous last element */
    assert(old_last->next == &node);               /* previous last element now points forward to node */

    /* 8. No other fields of the list are required to stay unchanged;
       they may have changed as part of the insertion (e.g., head.next when the list was empty). */
}
