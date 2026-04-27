#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_get_at_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t initial_length = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_length <= MAX_ITEM_ALLOC / item_size);
    __CPROVER_assume(aws_array_list_init(&list, allocator, initial_length, item_size) == AWS_OP_SUCCESS);
    size_t index = nondet_size_t();
    __CPROVER_assume(index < list.length || list.length == 0);
    uint8_t val[item_size];
    uint8_t old_val[item_size];
    memcpy(old_val, val, item_size);

    struct aws_array_list old_list = list;
    int result = aws_array_list_get_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        // val should now contain the value at the specified index
        if (index < list.length) {
            for (size_t i = 0; i < item_size; i++) {
                assert(val[i] == ((uint8_t *)list.data)[index * item_size + i]);
            }
        }
    } else {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        // val should remain unchanged
        for (size_t i = 0; i < item_size; i++) {
            assert(val[i] == old_val[i]);
        }
    }

    assert(aws_array_list_is_valid(&list));
    aws_array_list_clean_up(&list);
}
