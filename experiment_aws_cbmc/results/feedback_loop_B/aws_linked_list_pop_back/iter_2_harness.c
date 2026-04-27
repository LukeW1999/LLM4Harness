#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>

void aws_linked_list_pop_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Assume list is not empty */
    struct aws_linked_list_node node;
    __CPROVER_assume(&node != NULL);
    list.head.next = &node;
    list.tail.prev = &node;
    node.next = &list.tail;
    node.prev = &list.head;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_tail = node;

    /* 3. Call function under test */
    aws_linked_list_pop_back(&list);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Success path */
    if (old_list.head.next == &old_list.tail) {
        /* List was of size 1, should be empty now */
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        /* List had more than one element */
        assert(list.tail.prev == old_tail.prev);   // new tail is the previous node
        assert(old_tail.prev->next == &list.tail); // new tail points to actual tail
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.head.prev == &list.head);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
