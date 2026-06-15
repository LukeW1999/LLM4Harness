#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

/* Helper to count nodes in a list (excluding head and tail) */
static size_t count_nodes(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = list->head.next;
    while (cur != &list->tail) {
        cnt++;
        cur = cur->next;
    }
    return cnt;
}

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    const size_t max_items = 5;

    ensure_linked_list_is_allocated(&list, max_items, allocator);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Allocate a new node */
    struct aws_linked_list_node *node = (struct aws_linked_list_node *)aws_mem_acquire(allocator, sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    size_t old_cnt = count_nodes(&list);

    aws_linked_list_push_front(&list, node);

    assert(aws_linked_list_is_valid(&list));

    size_t new_cnt = count_nodes(&list);
    assert(new_cnt == old_cnt + 1);

    /* Verify node links */
    assert(node->prev == &list.head);
    assert(node->next == list.head.next);
    assert(list.head.next->prev == &list.head);
    assert(list.head.next == node);
}
