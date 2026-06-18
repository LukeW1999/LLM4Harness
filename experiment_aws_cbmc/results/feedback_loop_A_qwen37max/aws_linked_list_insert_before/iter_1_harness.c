#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    struct aws_linked_list_node *before = &list.head;
    size_t steps = nondet_size_t();
    __CPROVER_assume(steps <= MAX_LINKED_LIST_ITEM_ALLOCATION + 1);
    for (size_t i = 0; i < steps; ++i) {
        if (before == &list.tail) break;
        before = before->next;
    }
    /* We cannot insert before the head sentinel */
    __CPROVER_assume(before != &list.head);

    struct aws_linked_list_node to_add;
    /* Ensure the new node is not already one of the nodes being modified */
    __CPROVER_assume(&to_add != before);
    __CPROVER_assume(&to_add != before->prev);

    struct aws_linked_list_node *old_before_prev = before->prev;

    aws_linked_list_insert_before(before, &to_add);

    /* Assert postconditions: pointer linkages are correctly updated */
    assert(to_add.next == before);
    assert(to_add.prev == old_before_prev);
    assert(old_before_prev->next == &to_add);
    assert(before->prev == &to_add);

    /* Assert validity invariant: the list remains a valid doubly linked list */
    assert(aws_linked_list_is_valid_deep(&list));
}
