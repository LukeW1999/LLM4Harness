#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>

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

    void *out_ptr = NULL;
    int rv = aws_array_list_back((const struct aws_array_list *)&list, &out_ptr);

    if (list.length == 0) {
        assert(rv != AWS_OP_SUCCESS);
    } else {
        assert(rv == AWS_OP_SUCCESS);
    }

    aws_array_list_clean_up(&list);
}
