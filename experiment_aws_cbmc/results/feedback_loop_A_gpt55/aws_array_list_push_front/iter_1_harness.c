#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_push_front_harness(void) {
    struct aws_array_list list;

    list.alloc = nondet_bool() ? aws_default_allocator() : NULL;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    for (size_t i = 0; i < list.item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    struct aws_array_list old = list;
    size_t old_len_bytes = old.length * old.item_size;
    size_t old_required_size = old_len_bytes + old.item_size;

    uint8_t *old_data = malloc(old.current_size > 0 ? old.current_size : 1);
    __CPROVER_assume(old_data != NULL);
    if (old.current_size > 0) {
        memcpy(old_data, old.data, old.current_size);
    }

    uint8_t *old_val = malloc(old.item_size);
    __CPROVER_assume(old_val != NULL);
    memcpy(old_val, val, old.item_size);

    int result = aws_array_list_push_front(&list, val);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert(list.current_size >= old_required_size);
        assert(list.current_size >= old.current_size);

        if (old.current_size >= old_required_size) {
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        } else {
            assert(old.alloc != NULL);
        }

        assert_bytes_match((const uint8_t *)list.data, old_val, old.item_size);

        if (old_len_bytes > 0) {
            assert_bytes_match((const uint8_t *)list.data + old.item_size, old_data, old_len_bytes);
        }
    } else {
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        if (old.current_size > 0) {
            assert_bytes_match((const uint8_t *)list.data, old_data, old.current_size);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert_bytes_match(val, old_val, old.item_size);
    assert(aws_array_list_is_valid(&list));
}
