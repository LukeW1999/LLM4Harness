#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>

void aws_linked_list_push_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    __CPROVER_assume(&node != NULL);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old_list = list;

    /* 3. Call function under test */
    aws_linked_list_push_back(&list, &node);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Success path */
    assert(list.tail.prev == &node);   // push_back: tail now points to new node
    assert(node.prev == old_list.tail.prev);     // new node links back to previous last
    assert(node.next == &list.tail);   // new node points to tail

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.head.next == old_list.head.next);
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_is_in_list(&node));
}
