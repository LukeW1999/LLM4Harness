#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= 1024);

    size_t current_size = item_size * capacity;
    __CPROVER_assume(item_size == 0 || current_size / item_size == capacity);

    uint8_t *buffer = malloc(current_size);
    __CPROVER_assume(buffer != NULL);
    for (size_t i = 0; i < current_size; ++i) {
        buffer[i] = nondet_uint8_t();
    }

    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);

    list.alloc = alloc;
    list.item_size = item_size;
    list.current_size = current_size;
    list.length = length;
    list.data = buffer;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t old_length = list.length;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    void *old_data_ptr = list.data;

    int ret = aws_array_list_pop_back(&list);

    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);
    assert(list.current_size == old_current_size);
    assert(list.data == old_data_ptr);

    if (old_length > 0) {
        assert(ret == AWS_OP_SUCCESS);
        assert(list.length == old_length - 1);
        size_t removed_offset = old_item_size * (old_length - 1);
        for (size_t i = 0; i < old_item_size; ++i) {
            assert(((uint8_t *)list.data)[removed_offset + i] == 0);
        }
    } else {
        assert(ret != AWS_OP_SUCCESS);
        assert(list.length == 0);
    }

    free(buffer);
}
