#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_push_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    ensure_linked_list_is_valid(&list, allocator);

    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* PRE-CALL SNAPSHOT */
    struct aws_linked_list_node *orig_tail_prev = list.tail.prev;

    /* CALL */
    aws_linked_list_push_back(&list, node);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* list tail should now point to the newly added node */
    assert(list.tail.prev == node);
    /* the new node's prev should be the previous tail.prev */
    assert(node->prev == orig_tail_prev);
    /* the new node's next should be the tail sentinel */
    assert(node->next == &list.tail);
    /* the previous tail.prev's next should now point to the new node */
    assert(orig_tail_prev->next == node);
    /* the new node's prev's next should be the new node (consistency) */
    assert(node->prev->next == node);
    /* the new node's next's prev should be the new node (consistency) */
    assert(node->next->prev == node);
    /* sentinel invariants must still hold */
    assert(list.tail.next == &list.tail);
    assert(list.head.prev == &list.head);
}
