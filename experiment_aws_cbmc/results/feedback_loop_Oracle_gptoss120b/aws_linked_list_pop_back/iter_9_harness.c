#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Ensure the list is non‑empty before calling pop_back */
    struct aws_linked_list_node node;
    aws_linked_list_node_init(&node);
    aws_linked_list_push_back(&list, &node);

    struct aws_linked_list_node *out = aws_linked_list_pop_back(&list);

    __CPROVER_assert(out == &node, "pop_back returns the pushed node");
    __CPROVER_assert(out->next == out, "out->next points to itself");
    __CPROVER_assert(out->prev == out, "out->prev points to itself");
    __CPROVER_assert(aws_linked_list_empty(&list), "list is empty after pop_back");
}
