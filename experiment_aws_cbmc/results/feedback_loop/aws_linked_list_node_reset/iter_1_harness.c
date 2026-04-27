#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

void aws_linked_list_push_back_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* 2. Bound the structure (limits CBMC state space) */
    __CPROVER_assume(aws_linked_list_is_bounded(&list, MAX_LINKED_LIST_ITEM_ALLOCATION));

    /* 3. Allocate pointer members */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 4. Assume validity precondition */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 5. Save old state (for checking immutability) */
    struct aws_linked_list old_list = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)&list, sizeof(struct aws_linked_list), &old_byte);

    /* 6. Assume function-specific preconditions */
    __CPROVER_assume(node.next == NULL);
    __CPROVER_assume(node.prev == NULL);

    /* 7. Call function under test */
    aws_linked_list_push_back(&list, &node);

    /* 8. Assert postconditions (both branches) */
    assert(aws_linked_list_is_valid(&list));  // invariant always holds
    assert(list.tail.prev == &node);          // [node] is the new last element of [list]
    assert(node.next == &list.tail);          // [node] is correctly linked to the tail
    assert(node.prev == old_list.tail.prev);  // [node] is correctly linked to the previous last element
    assert(old_list.tail.prev->next == &node);// Previous last element points to [node]

    /* Check immutability of other parts of the list */
    assert_bytes_match((uint8_t *)&list.head, (uint8_t *)&old_list.head, sizeof(struct aws_linked_list_node));
    assert_bytes_match((uint8_t *)&list.tail.next, (uint8_t *)&old_list.tail.next, sizeof(struct aws_linked_list_node *));
}
