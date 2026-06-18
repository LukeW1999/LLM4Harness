#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>
#include <assert.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 10

void aws_linked_list_pop_front_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    size_t len = (size_t)(__CPROVER_nondet_uint8_t()) % (MAX_LINKED_LIST_ITEM_ALLOCATION + 1);

    struct aws_linked_list_node nodes[MAX_LINKED_LIST_ITEM_ALLOCATION];
    for (size_t i = 0; i < len; ++i) {
        aws_linked_list_push_back(&list, &nodes[i]);
        __CPROVER_assume(aws_linked_list_is_valid(&list));
    }

    struct aws_linked_list_node *out = NULL;

    if (!aws_linked_list_empty(&list)) {
        aws_linked_list_pop_front(&list, &out);
        assert(aws_linked_list_is_valid(&list));
        assert(out != NULL);
    } else {
        aws_linked_list_pop_front(&list, &out);
        assert(aws_linked_list_is_valid(&list));
        assert(out == NULL);
    }
}
