#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

size_t nondet_size_t(void);

void aws_linked_list_remove_harness(void) {
    const size_t MAX_NODES = 5;

    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    size_t num_nodes = nondet_size_t();
    __CPROVER_assume(num_nodes > 0);
    __CPROVER_assume(num_nodes <= MAX_NODES);

    struct aws_linked_list_node nodes[5];
    for (size_t i = 0; i < num_nodes; ++i) {
        aws_linked_list_node_init(&nodes[i]);
        aws_linked_list_push_back(&list, &nodes[i]);
    }
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *old_next[5];
    struct aws_linked_list_node *old_prev[5];
    for (size_t i = 0; i < num_nodes; ++i) {
        old_next[i] = nodes[i].next;
        old_prev[i] = nodes[i].prev;
    }

    size_t idx = nondet_size_t() % num_nodes;
    struct aws_linked_list_node *node = &nodes[idx];
    __CPROVER_assume(node != NULL);

    struct aws_linked_list_node *prev = node->prev;
    struct aws_linked_list_node *next = node->next;

    aws_linked_list_remove(node);

    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(aws_linked_list_is_valid(&list));

    if (prev != NULL) {
        assert(prev->next == next);
    } else {
        assert(list.head == next);
    }
    if (next != NULL) {
        assert(next->prev == prev);
    } else {
        assert(list.tail == prev);
    }

    for (size_t i = 0; i < num_nodes; ++i) {
        struct aws_linked_list_node *cur = &nodes[i];
        if (cur != node) {
            assert(cur->next == old_next[i]);
            assert(cur->prev == old_prev[i]);
        }
    }
}
