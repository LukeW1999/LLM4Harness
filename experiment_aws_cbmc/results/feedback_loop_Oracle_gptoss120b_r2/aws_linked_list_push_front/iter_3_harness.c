#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness(void) {
    /* allocator (not used directly in this harness) */
    struct aws_allocator *alloc = aws_default_allocator();
    (void)alloc;

    /* Initialize the list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Create a nondeterministic number of existing nodes (bounded) */
    size_t pre_len = nondet_uint();
    __CPROVER_assume(pre_len <= 5);

    struct aws_linked_list_node *existing = NULL;
    if (pre_len > 0) {
        existing = malloc(pre_len * sizeof(*existing));
        __CPROVER_assume(existing != NULL);
    }

    /* Populate the list with the existing nodes */
    for (size_t i = 0; i < pre_len; ++i) {
        aws_linked_list_node_reset(&existing[i]);
        aws_linked_list_push_back(&list, &existing[i]);
    }

    /* Snapshot the list and the existing nodes (frame condition) */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_next = NULL;
    struct aws_linked_list_node *old_prev = NULL;
    if (pre_len > 0) {
        old_next = malloc(pre_len * sizeof(*old_next));
        old_prev = malloc(pre_len * sizeof(*old_prev));
        __CPROVER_assume(old_next != NULL && old_prev != NULL);
        for (size_t i = 0; i < pre_len; ++i) {
            old_next[i] = existing[i].next;
            old_prev[i] = existing[i].prev;
        }
    }

    /* Create a fresh node to push to the front */
    struct aws_linked_list_node new_node;
    aws_linked_list_node_reset(&new_node);

    /* Call the function under verification */
    aws_linked_list_push_front(&list, &new_node);

    /* Post‑condition: the list must be valid */
    assert(aws_linked_list_is_valid(&list));

    /* Compute the length after the operation */
    size_t post_len = 0;
    for (struct aws_linked_list_node *it = list.head.next; it != &list.tail; it = it->next) {
        ++post_len;
    }
    /* Length must have increased by exactly one */
    assert(post_len == pre_len + 1);

    /* The new node must be the first element */
    assert(list.head.next == &new_node);
    assert(new_node.prev == &list.head);
    assert(new_node.next != NULL);
    if (pre_len == 0) {
        assert(new_node.next == &list.tail);
    } else {
        assert(new_node.next == &existing[0]);
    }

    /* The remaining order of existing nodes must be unchanged */
    struct aws_linked_list_node *it = new_node.next;
    for (size_t i = 0; i < pre_len; ++i) {
        assert(it == &existing[i]);
        it = it->next;
    }
    assert(it == &list.tail);

    /* Frame condition: existing nodes' internal fields (next/prev) unchanged except first node's prev */
    if (pre_len > 0) {
        /* First existing node's prev should now point to the new node */
        assert(existing[0].prev == &new_node);
        /* Its next should be unchanged */
        assert(existing[0].next == old_next[0]);

        for (size_t i = 1; i < pre_len; ++i) {
            assert(existing[i].next == old_next[i]);
            assert(existing[i].prev == old_prev[i]);
        }
    }

    /* Frame condition: list head/tail sentinel links unchanged */
    assert(old_list.head.prev == list.head.prev);
    assert(old_list.tail.next == list.tail.next);
    assert(old_list.tail.prev == list.tail.prev || list.tail.prev == (pre_len == 0 ? &new_node : &existing[pre_len - 1]));

    /* Clean up */
    free(existing);
    free(old_next);
    free(old_prev);
}
