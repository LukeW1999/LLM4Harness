#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    node.next = NULL;
    node.prev = NULL;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* 3. Call function under test */
    aws_linked_list_push_back(&list, &node);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(list.tail.prev == &node);   // push_back: tail now points to new node
    assert(node.prev == old_last);     // new node links back to previous last
    assert(node.next == &list.tail);   // new node links forward to tail sentinel

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.head.prev == NULL);    // head.prev unchanged
    assert(list.head.next!= NULL);   // head.next unchanged

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));
}
