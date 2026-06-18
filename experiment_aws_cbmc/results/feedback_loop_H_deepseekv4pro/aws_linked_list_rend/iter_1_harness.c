#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_rend_harness() {
    /* Declare and bound list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state of list's head and tail sentinel nodes */
    const struct aws_linked_list_node *old_head_next = list.head.next;
    const struct aws_linked_list_node *old_head_prev = list.head.prev;
    const struct aws_linked_list_node *old_tail_next = list.tail.next;
    const struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call function under test */
    const struct aws_linked_list_node *rval = aws_linked_list_rend(&list);

    /* Postconditions */
    assert(rval == &list.head);                               /* returns pointer to one before first element */
    assert(aws_linked_list_is_valid(&list));                  /* list remains valid */
    assert(list.head.next == old_head_next);                  /* head.next unchanged */
    assert(list.head.prev == old_head_prev);                  /* head.prev unchanged */
    assert(list.tail.next == old_tail_next);                  /* tail.next unchanged */
    assert(list.tail.prev == old_tail_prev);                  /* tail.prev unchanged */
}
