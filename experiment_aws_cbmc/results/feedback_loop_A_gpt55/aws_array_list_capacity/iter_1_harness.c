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

void aws_array_list_set_at_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(list.alloc == NULL || list.alloc == aws_default_allocator());
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(index < SIZE_MAX);

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    size_t old_capacity = old.current_size / old.item_size;
    size_t old_len_bytes = old.length * old.item_size;

    struct store_byte_from_buffer old_byte;
    if (old_len_bytes > 0) {
        save_byte_from_array((const uint8_t *)old.data, old_len_bytes, &old_byte);
    }

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
    assert(aws_array_list_is_valid(&list));

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (old.alloc == NULL && index >= old_capacity) {
        assert(result == AWS_OP_ERR);
    }
