#include <proof_helpers/make_common_data_structures.h>

static size_t list_node_count(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = aws_linked_list_begin(list);
    const struct aws_linked_list_node *end = aws_linked_list_end(list);
    while (cur != end) {
        cnt++;
        cur = aws_linked_list_next(cur);
    }
    return cnt;
}

static void snapshot_node(const struct aws_linked_list_node *node,
                          struct aws_linked_list_node *snap) {
    snap->next = node->next;
    snap->prev = node->prev;
}

static bool compare_node(const struct aws_linked_list_node *a,
                         const struct aws_linked_list_node *b) {
    return a->next == b->next && a->prev == b->prev;
}

static void snapshot_list(const struct aws_linked_list *list,
                          struct aws_linked_list *snap) {
    snap->head.next = list->head.next;
    snap->head.prev = list->head.prev;
    snap->tail.next = list->tail.next;
    snap->tail.prev = list->tail.prev;
}

void aws_linked_list_remove_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    aws_linked_list_init(&list);
    assert(aws_linked_list_is_valid(&list));

    const size_t MAX_NODES = 5;
    size_t num_nodes = nondet_uint() % (MAX_NODES + 1);

    struct aws_linked_list_node nodes[MAX_NODES];
    struct aws_linked_list_node node_snapshots[MAX_NODES];

    for (size_t i = 0; i < num_nodes; ++i) {
        aws_linked_list_node_reset(&nodes[i]);
        aws_linked_list_push_back(&list, &nodes[i]);
    }

    struct aws_linked_list list_before;
    snapshot_list(&list, &list_before);

    for (size_t i = 0; i < num_nodes; ++i) {
        snapshot_node(&nodes[i], &node_snapshots[i]);
    }

    struct aws_linked_list_node *node_to_remove = NULL;
    if (num_nodes > 0) {
        size_t idx = nondet_uint() % num_nodes;
        node_to_remove = &nodes[idx];
    }

    size_t count_before = list_node_count(&list);

    if (node_to_remove != NULL) {
        aws_linked_list_remove(node_to_remove);
    }

    assert(aws_linked_list_is_valid(&list));

    if (node_to_remove != NULL) {
        assert(node_to_remove->next == NULL);
        assert(node_to_remove->prev == NULL);
    }

    size_t count_after = list_node_count(&list);
    if (node_to_remove != NULL) {
        assert(count_after + 1 == count_before);
    } else {
        assert(count_after == count_before);
    }

    if (num_nodes > 0) {
        for (size_t i = 0; i < num_nodes; ++i) {
            if (&nodes[i] == node_to_remove) {
                continue;
            }
            bool adjacent = false;
            if (node_to_remove != NULL) {
                adjacent = (node_snapshots[i].next == node_to_remove) ||
                           (node_snapshots[i].prev == node_to_remove);
            }
            if (!adjacent) {
                assert(compare_node(&nodes[i], &node_snapshots[i]));
            }
        }
    }

    (void)allocator;
}
