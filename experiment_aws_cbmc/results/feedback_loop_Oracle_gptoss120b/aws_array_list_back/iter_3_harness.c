#include <assert.h>
#include <stdint.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size < 1024);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 64);

    if (aws_array_list_init(&list, allocator, capacity, item_size) != AWS_OP_SUCCESS) {
        return;
    }

    size_t max_len = list.current_size / list.item_size;
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= max_len);

    void *out = NULL;
    (void)aws_array_list_back(&list, &out);

    if (list.length == 0) {
        assert(out == NULL);
    } else {
        assert(out == (uint8_t *)list.data + (list.length - 1) * list.item_size);
    }

    aws_array_list_clean_up(&list);
}
