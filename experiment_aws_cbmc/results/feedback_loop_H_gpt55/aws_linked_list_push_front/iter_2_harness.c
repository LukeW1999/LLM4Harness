#include <aws/common/allocator.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

extern _Bool nondet_bool(void);

static void make_empty_linked_list(struct aws_linked_list *list) {
    ensure_linked_list_is_allocated(list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(list);
    assert(aws_linked_list_is_valid(list));
    assert(aws_linked_list_empty(list));
}

void aws_linked_list_push_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    (void)allocator;

    struct aws_linked_list list;
    struct aws_linked_list_node old_node;
    struct aws_linked_list_node node;
    _Bool was_nonempty = nondet_bool();

    make_empty_linked_list(&list);
    aws_linked_list_node_reset(&old_node);
    aws_linked_list_node_reset(&node);

    if (was_nonempty) {
        aws_linked_list_push_back(&list, &old_node);
    }

    struct aws_linked_list_node *old_first = list.head.next;
    struct aws_linked_list_node *old_last = list.tail.prev;

    aws_linked_list_push_front(&list, &node);

    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == old_first);
    assert(old_first->prev == &node);

    if (was_nonempty) {
        assert(list.tail.prev == old_last);
    } else {
        assert(old_last == &list.head);
        assert(list.tail.prev == &node);
    }

    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_node_is_in_list(&node));
    assert(aws_linked_list_is_valid_deep(&list));
}
