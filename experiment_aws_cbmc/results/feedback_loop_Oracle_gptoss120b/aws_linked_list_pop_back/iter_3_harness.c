#include <assert.h>
#include <stdbool.h>
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
        assert(out != NULL);
        assert(out->next == out);
        assert(out->prev == out);
    } else {
        assert(out == NULL);
    }
}
