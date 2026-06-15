#include <proof_helpers/make_common_data_structures.h>

#define MAX_NODES 5

static size_t count_nodes(const struct aws_linked_list *list) {
    size_t cnt = 0;
    for (struct aws_linked_list_node *cur = aws_linked_list_begin(list);
         cur != aws_linked_list_end(list);
         cur = aws_linked_list_next(cur)) {
        cnt++;
    }
    return cnt;
}

static bool node_in_list(const struct aws_linked_list *list,
                         const struct aws_linked_list_node *target) {
    for (struct aws_linked_list_node *cur = aws_linked_list_begin(list);
         cur != aws_linked_list_end(list);
         cur = aws_linked_list_next(cur)) {
        if (cur == target) {
            return true;
        }
    }
    return false;
}

void aws_linked_list_remove_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_NODES);
    __CPROVER_assume(n > 0);

    struct aws_linked_list_node nodes[MAX_NODES];

    for (size_t i = 0; i < n; ++i) {
        aws_linked_list_node_reset(&nodes[i]);
        aws_linked_list_push_back(&list, &nodes[i]);
    }

    size_t idx = nondet_size_t();
    __CPROVER_assume(idx < n);
    struct aws_linked_list_node *node = &nodes[idx];
    __CPROVER_assume(node_in_list(&list, node));

    size_t len_before = count_nodes(&list);

    aws_linked_list_remove(node);

    /* Post‑conditions */
    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(!node_in_list(&list, node));
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    size_t len_after = count_nodes(&list);
    assert(len_before == len_after + 1);

    for (size_t i = 0; i < n; ++i) {
        if (&nodes[i] != node) {
            assert(node_in_list(&list, &nodes[i]));
        }
    }
}
