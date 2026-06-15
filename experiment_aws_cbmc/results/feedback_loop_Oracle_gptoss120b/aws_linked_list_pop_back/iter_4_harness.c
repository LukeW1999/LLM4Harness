#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_back_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    if (__CPROVER_nondet_bool()) {
        struct aws_linked_list_node node;
        aws_linked_list_node_init(&node);
        aws_linked_list_push_back(&list, &node);
    }

    struct aws_linked_list_node *out = NULL;
    bool result = aws_linked_list_pop_back(&list, &out);

    if (result) {
        __CPROVER_assert(out != NULL, "out is not NULL when pop succeeds");
        __CPROVER_assert(out->next == out, "out->next points to itself");
        __CPROVER_assert(out->prev == out, "out->prev points to itself");
    } else {
        __CPROVER_assert(out == NULL, "out is NULL when pop fails");
    }
}
