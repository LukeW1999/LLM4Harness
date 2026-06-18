#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Ensure list is not empty so we have a real node to use as before */
    __CPROVER_assume(list.head.next != &list.tail);

    /* Pick the first real node as before */
    struct aws_linked_list_node *before = list.head.next;

    /* Create a new node to insert, not in any list */
    struct aws_linked_list_node to_add_node;
    to_add_node.next = NULL;
    to_add_node.prev = NULL;
    struct aws_linked_list_node *to_add = &to_add_node;

    /* to_add must be distinct from before to avoid self-insertion cycles */
    __CPROVER_assume(to_add != before);

    /* Save old state */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 2. Call function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 3. Assert postconditions */

    /* to_add is now linked correctly */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(old_before_prev->next == to_add);
    assert(before->prev == to_add);

    /* before's next should remain unchanged */
    assert(before->next == old_before_next);

    /* If before was the first node, head.next should now be to_add */
    if (old_before_prev == &list.head) {
        assert(list.head.next == to_add);
    } else {
        assert(list.head.next == old_head_next);
    }

    /* tail.prev should remain unchanged because we are not inserting before the tail sentinel */
    assert(list.tail.prev == old_tail_prev);

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));
}
