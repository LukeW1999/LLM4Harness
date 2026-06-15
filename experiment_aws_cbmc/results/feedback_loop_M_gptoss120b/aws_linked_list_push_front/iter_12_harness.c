#include <proof_helpers/make_common_data_structures.h>

/* Helper to count nodes in a list (excluding the tail sentinel) */
static size_t count_nodes(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = list->head;
    while (cur != NULL && cur != &list->tail) {
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

    /* Capture the current first node (may be the tail sentinel) */
    struct aws_linked_list_node *old_head = list.head;

    /* Allocate a new node */
    struct aws_linked_list_node *node =
        (struct aws_linked_list_node *)aws_mem_acquire(allocator, sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    size_t old_cnt = count_nodes(&list);

    aws_linked_list_push_front(&list, node);

    /* List must remain valid */
    assert(aws_linked_list_is_valid(&list));

    size_t new_cnt = count_nodes(&list);
    assert(new_cnt == old_cnt + 1);

    /* Verify node links and list head */
    assert(list.head == node);
    assert(node->next == old_head);
    assert(node->prev == NULL);

    if (old_head == &list.tail) {
        /* List was empty before the push */
        assert(list.tail.prev == node);
    } else {
        /* Old first node should now point back to the new node */
        assert(old_head->prev == node);
    }
}
