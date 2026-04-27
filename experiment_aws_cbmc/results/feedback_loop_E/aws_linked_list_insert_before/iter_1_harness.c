#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_before_harness() {
    /* data structures */
    struct aws_linked_list list;
    struct aws_linked_list_node before;
    struct aws_linked_list_node to_add;

    /* allocation and bounding */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* non-deterministic initialization */
    before.next = (struct aws_linked_list_node *)nondet_pointer();
    before.prev = (struct aws_linked_list_node *)nondet_pointer();
    to_add.next = (struct aws_linked_list_node *)nondet_pointer();
    to_add.prev = (struct aws_linked_list_node *)nondet_pointer();

    /* assume preconditions */
    __CPROVER_assume(before.prev != NULL);
    __CPROVER_assume(before.next != NULL);
    __CPROVER_assume(before.prev->next == &before);
    __CPROVER_assume(before.next->prev == &before);
    __CPROVER_assume(to_add.next == NULL);
    __CPROVER_assume(to_add.prev == NULL);

    /* save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_before = before;
    struct aws_linked_list_node old_to_add = to_add;

    /* perform operation under verification */
    aws_linked_list_insert_before(&before, &to_add);

    /* assertions */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(before.prev == &to_add);
    assert(to_add.next == &before);
    assert(to_add.prev == old_before.prev);
    assert(old_before.prev->next == &to_add);

    /* unchanged fields */
    assert(list.head.next == old_list.head.next || list.head.next == &to_add);
    assert(list.tail.prev == old_list.tail.prev);
    assert(before.next == old_before.next);
}
