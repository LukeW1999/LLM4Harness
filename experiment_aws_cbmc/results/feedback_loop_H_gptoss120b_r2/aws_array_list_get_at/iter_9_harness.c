#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    aws_array_list_init(&list, allocator, item_size, capacity);

    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);
    list.length = length;

    struct aws_array_list old = list;

    size_t idx = nondet_size_t();
    __CPROVER_assume(idx < list.current_size);

    uint8_t *val = (uint8_t *)malloc(item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    int result = aws_array_list_set_at(&list, val, idx);

    if (result == AWS_OP_SUCCESS) {
        size_t expected_len = (old.length > idx) ? old.length : (idx + 1);
        assert(list.length == expected_len);
        for (size_t i = 0; i < item_size; ++i) {
            assert(((uint8_t *)list.data)[idx * item_size + i] == val[i]);
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));

    free(val);
    aws_array_list_clean_up(&list);
}
