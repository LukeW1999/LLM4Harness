#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_ptr_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    size_t initial_capacity;
    __CPROVER_assume(initial_capacity <= 64);

    int init_result = aws_array_list_init_dynamic(&list, allocator, initial_capacity, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t length;
    __CPROVER_assume(length <= list.current_size / item_size);
    list.length = length;

    void *old_val = NULL;
    void *val = old_val;
    void **val_ptr = &val;

    size_t index;
    __CPROVER_assume(index <= list.length + 5);

    int ret = aws_array_list_get_at_ptr(&list, val_ptr, index);

    if (index < list.length) {
        assert(ret == AWS_OP_SUCCESS);
        assert(*val_ptr == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        assert(ret == AWS_ERROR_INVALID_INDEX);
        assert(*val_ptr == old_val);
    }

    assert(list.alloc == allocator);
    assert(list.item_size == item_size);
    assert(list.current_size == list.current_size);
    assert(list.length == length);
    assert(list.data == list.data);

    aws_array_list_clean_up(&list);
}
