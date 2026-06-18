#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_push_front_harness(void) {
    struct aws_allocator allocator = {0};
    struct aws_allocator *allocator_ptr = &allocator;

    struct aws_linked_list list;
    aws_linked_list_init(&list);

    ensure_linked_list_is_allocated(&list, allocator_ptr, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    node.prev = NULL;
    node.next = NULL;

    struct aws_linked_list old = list;

    aws_linked_list_push_front(&list, &node);

    __CPROVER_assert(aws_linked_list_is_valid(&list), "list remains valid after push_front");
    __CPROVER_assert(node.prev == NULL, "new head node prev is NULL");
    __CPROVER_assert(node.next == old.head, "new head node next points to old head");

    if (aws_linked_list_empty(&old)) {
        __CPROVER_assert(list.head == &node, "head is new node when list was empty");
        __CPROVER_assert(list.tail == &node, "tail is new node when list was empty");
    } else {
        __CPROVER_assert(list.head == &node, "head is new node when list was not empty");
        __CPROVER_assert(list.tail == old.tail, "tail unchanged when list was not empty");
        __CPROVER_assert(old.head->prev == &node, "old head prev points to new node");
    }
}
