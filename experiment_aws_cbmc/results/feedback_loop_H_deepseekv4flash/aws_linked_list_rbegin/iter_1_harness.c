#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_rbegin_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old pointers for immutability check */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call function under test */
    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);

    /* Postconditions:
     * 1. Return value equals list.tail.prev (trivial but explicit)
     * 2. All fields of list are unchanged (const function)
     * 3. List remains valid
     * 4. In a valid sentinel list, the node before tail always has next pointing to tail */
    assert(rval == list.tail.prev);
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);
    assert(aws_linked_list_is_valid(&list));
    assert(rval->next == &list.tail);
}
