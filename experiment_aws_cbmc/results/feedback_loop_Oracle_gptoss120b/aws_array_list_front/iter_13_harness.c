#include <aws/common/array_list.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    uint8_t raw[256];
    size_t item_size = sizeof(int);

    if (aws_array_list_init_static(&list, raw, sizeof(raw), item_size) != AWS_OP_SUCCESS) {
        return;
    }

    size_t max_items = sizeof(raw) / item_size;
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= max_items);

    for (size_t i = 0; i < n; ++i) {
        int val = nondet_int();
        if (aws_array_list_push_back(&list, &val) != AWS_OP_SUCCESS) {
            break;
        }
    }

    const void *front = aws_array_list_front(&list);
    if (list.length > 0) {
        assert(front != NULL);
        assert(front == list.data);
    } else {
        assert(front == NULL);
    }

    aws_array_list_clean_up(&list);
}
