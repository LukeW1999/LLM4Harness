#include <assert.h>
#include <string.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 64);

    size_t initial_capacity = nondet_size_t();
    __CPROVER_assume(initial_capacity <= 64);

    if (aws_array_list_init(&list, allocator, initial_capacity, item_size) != AWS_OP_SUCCESS) {
        return;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    if (list.length == 0) {
        uint8_t dummy[64];
        for (size_t i = 0; i < item_size; ++i) {
            dummy[i] = nondet_uint8_t();
        }
        aws_array_list_push_back(&list, dummy);
    }

    size_t index = nondet_size_t();
    __CPROVER_assume(index < list.length);

    uint8_t val_buf[64];
    for (size_t i = 0; i < item_size; ++i) {
        val_buf[i] = nondet_uint8_t();
    }

    struct aws_array_list old = list;

    int result = aws_array_list_set_at(&list, val_buf, index);

    if (result == AWS_OP_SUCCESS) {
        uint8_t *dest = (uint8_t *)list.data + (index * list.item_size);
        assert(memcmp(dest, val_buf, list.item_size) == 0);
        assert(list.length == old.length);
    } else {
        assert(list.length == old.length);
    }

    assert(aws_array_list_is_valid(&list));

    aws_array_list_clean_up(&list);
}
