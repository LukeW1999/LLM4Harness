#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_remove_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* allocate and initialize a linked list (stack allocated) */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* allocate a node that will be removed (stack allocated) */
    struct aws_linked_list_node node;
    aws_linked_list_node_reset(&node);

    /* insert the node into the list (list now non‑empty) */
    aws_linked_list_push_back(&list, &node);

    /* optionally add additional nodes to the list */
    {
        size_t extra_nodes = nondet_size_t();
        __CPROVER_assume(extra_nodes <= 5);
        struct aws_linked_list_node extra[5];
        for (size_t i = 0; i < extra_nodes; ++i) {
            aws_linked_list_node_reset(&extra[i]);
            aws_linked_list_push_back(&list, &extra[i]);
        }
    }

    /* ensure preconditions for aws_linked_list_remove */
    __CPROVER_assume(node.prev != NULL);
    __CPROVER_assume(node.next != NULL);
    __CPROVER_assume(node.prev->next == &node);
    __CPROVER_assume(node.next->prev == &node);

    /* snapshot of relevant state before the call */
    size_t original_len = 0;
    struct aws_linked_list_node *sentinel = &list.head;
    for (struct aws_linked_list_node *cur = sentinel->next; cur != sentinel; cur = cur->next) {
        original_len++;
    }
    struct aws_linked_list_node *orig_prev = node.prev;
    struct aws_linked_list_node *orig_next = node.next;

    /* call under verification */
    aws_linked_list_remove(&node);

    /* length invariant: list length decreased by one */
    size_t new_len = 0;
    for (struct aws_linked_list_node *cur = sentinel->next; cur != sentinel; cur = cur->next) {
        new_len++;
    }
    assert(new_len == original_len - 1);

    /* list validity must be preserved */
    assert(aws_linked_list_is_valid(&list));

    /* the removed node must be isolated */
    assert(node.prev == NULL);
    assert(node.next == NULL);
    assert(!aws_linked_list_node_is_in_list(&node));

    /* surrounding nodes must be linked together */
    if (orig_prev != NULL) {
        assert(orig_prev->next == orig_next);
    }
    if (orig_next != NULL) {
        assert(orig_next->prev == orig_prev);
    }
}
