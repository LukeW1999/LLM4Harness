#include <aws/common/linked_list.h>
#include <aws/common/memory.h>
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
    __CPROVER_assert(aws_linked_list_is_valid(&list), "list remains valid after push_front");

    size_t new_cnt = count_nodes(&list);
    __CPROVER_assert(new_cnt == old_cnt + 1, "node count increased by one");

    /* Verify node links and list head */
    __CPROVER_assert(list.head == node, "head points to new node");
    __CPROVER_assert(node->next == old_head, "new node next points to old head");
    __CPROVER_assert(node->prev == NULL, "new node prev is NULL");

    if (old_head == &list.tail) {
        /* List was empty before the push */
        __CPROVER_assert(list.tail.prev == node, "tail.prev points to new node in empty list");
    } else {
        /* Old first node should now point back to the new node */
        __CPROVER_assert(old_head->prev == node, "old head prev points to new node");
    }
}
