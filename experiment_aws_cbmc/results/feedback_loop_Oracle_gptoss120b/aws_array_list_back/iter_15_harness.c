#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size < 1024);

    size_t capacity;
    __CPROVER_assume(capacity <= 64);

    if (aws_array_list_init(&list, allocator, capacity, item_size) != AWS_OP_SUCCESS) {
        return;
    }

    size_t length;
    __CPROVER_assume(length <= capacity);
    list.length = length;

    uint8_t out_buf[1024];
    int rv = aws_array_list_back(&list, out_buf);

    if (list.length == 0) {
        __CPROVER_assert(rv != AWS_OP_SUCCESS, "back should fail on empty list");
    } else {
        __CPROVER_assert(rv == AWS_OP_SUCCESS, "back should succeed on non‑empty list");
    }

    aws_array_list_clean_up(&list);
}
