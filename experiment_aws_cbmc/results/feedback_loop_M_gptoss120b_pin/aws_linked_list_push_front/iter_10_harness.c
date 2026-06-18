#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_push_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    aws_linked_list_init(&list);

    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list_node node;
    /* Ensure the node is not part of any list */
    node.prev = NULL;
    node.next = NULL;

    bool was_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_head_next = list.head.next;

    aws_linked_list_push_front(&list, &node);

    assert(aws_linked_list_is_valid(&list));

    /* Head and tail sentinel invariants */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* New node connections */
    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    if (was_empty) {
        assert(node.next == &list.tail);
        assert(list.tail.prev == &node);
    } else {
        assert(node.next == old_head_next);
        assert(old_head_next->prev == &node);
        assert(list.tail.prev != &list.head); /* list is not empty */
    }
}
