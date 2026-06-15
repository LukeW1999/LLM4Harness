#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_rend_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    const uint32_t max_nodes = 5;
    uint32_t node_count = __CPROVER_nondet_uint();
    __CPROVER_assume(node_count <= max_nodes);

    struct aws_linked_list_node nodes[5];
    struct aws_linked_list_node old_nodes[5];

    for (uint32_t i = 0; i < node_count; ++i) {
        aws_linked_list_node_reset(&nodes[i]);
        aws_linked_list_push_back(&list, &nodes[i]);
    }

    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list old_list = list;
    for (uint32_t i = 0; i < node_count; ++i) {
        old_nodes[i] = nodes[i];
    }

    const struct aws_linked_list_node *r = aws_linked_list_rend(&list);

    __CPROVER_assert(r != 0, "return value is non-NULL");
    __CPROVER_assert(r == &list.head, "return value points to list head sentinel");

    __CPROVER_assert(list.head.next == old_list.head.next, "list head next unchanged");
    __CPROVER_assert(list.head.prev == old_list.head.prev, "list head prev unchanged");

    for (uint32_t i = 0; i < node_count; ++i) {
        __CPROVER_assert(nodes[i].next == old_nodes[i].next, "node next unchanged");
        __CPROVER_assert(nodes[i].prev == old_nodes[i].prev, "node prev unchanged");
    }
}
