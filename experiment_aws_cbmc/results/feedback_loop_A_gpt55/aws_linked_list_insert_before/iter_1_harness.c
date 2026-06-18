#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node to_add;

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    to_add.next = NULL;
    to_add.prev = NULL;
    __CPROVER_assume(!aws_linked_list_node_is_in_list(&to_add));

    struct aws_linked_list_node *before = nondet_bool() ? list.head.next : &list.tail;

    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before != &list.head);
    __CPROVER_assume(before != &to_add);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(before, sizeof(*before)));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(before));

    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;
    struct aws_linked_list_node *old_prev_prev = old_before_prev->prev;

    __CPROVER_assume(old_before_prev != NULL);
    __CPROVER_assume(old_before_prev != &to_add);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(old_before_prev, sizeof(*old_before_prev)));
    __CPROVER_assume(old_before_prev->next == before);

    aws_linked_list_insert_before(before, &to_add);

    assert(to_add.next == before);
    assert(to_add.prev == old_before_prev);
    assert(before->prev == &to_add);
    assert(before->next == old_before_next);
    assert(old_before_prev->next == &to_add);
    assert(old_before_prev->prev == old_prev_prev);

    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    if (old_before_prev == &list.head) {
        assert(list.head.next == &to_add);
    } else {
        assert(list.head.next == old_list.head.next);
    }

    if (before == &list.tail) {
        assert(list.tail.prev == &to_add);
    } else {
        assert(list.tail.prev == old_list.tail.prev);
    }

    assert(aws_linked_list_node_is_in_list(&to_add));
    assert(aws_linked_list_node_next_is_valid(old_before_prev));
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_node_prev_is_valid(&to_add));
    assert(aws_linked_list_node_prev_is_valid(before));

    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
