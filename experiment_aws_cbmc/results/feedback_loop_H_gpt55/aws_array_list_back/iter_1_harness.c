#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_LIST_DATA_SIZE (MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE)

void aws_array_list_back_harness() {
    struct aws_array_list list;
    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.length <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(list.current_size <= MAX_LIST_DATA_SIZE);
    __CPROVER_assume(list.current_size == 0 || AWS_MEM_IS_READABLE(list.data, list.current_size));

    uint8_t val[MAX_ITEM_SIZE];
    uint8_t old_val[MAX_ITEM_SIZE];

    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val[i] = nondet_uint8_t();
        old_val[i] = val[i];
    }

    uint8_t old_data[MAX_LIST_DATA_SIZE];
    for (size_t i = 0; i < MAX_LIST_DATA_SIZE; ++i) {
        if (i < list.current_size) {
            old_data[i] = ((const uint8_t *)list.data)[i];
        }
    }

    struct aws_array_list old = list;

    int result = aws_array_list_back(&list, val);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(old.length > 0);

        size_t last_item_offset = old.item_size * (old.length - 1);
        assert(last_item_offset + old.item_size <= old.current_size);
        assert_bytes_match(val, old_data + last_item_offset, old.item_size);

        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    } else {
        assert(result == AWS_OP_ERR);
        assert(old.length == 0);

        assert_bytes_match(val, old_val, old.item_size);

        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    }

    assert((old.length > 0) == (result == AWS_OP_SUCCESS));

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (list.current_size > 0) {
        assert_bytes_match((const uint8_t *)list.data, old_data, list.current_size);
    }

    assert(aws_array_list_is_valid(&list));
}
