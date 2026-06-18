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

    if (list.alloc != NULL) {
        list.alloc = aws_default_allocator();
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    size_t index = nondet_size_t();
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(index < SIZE_MAX / list.item_size);

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_byte);

    size_t required_size = (index + 1) * old.item_size;
    size_t old_capacity = old.current_size / old.item_size;

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (old.alloc == NULL) {
        if (index < old_capacity) {
            assert(result == AWS_OP_SUCCESS);
        } else {
            assert(result == AWS_OP_ERR);
        }
    }

    if (result == AWS_OP_SUCCESS) {
        assert(list.length > index);

        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        assert(list.current_size >= required_size);
        assert_bytes_match((const uint8_t *)list.data + (index * list.item_size), val, list.item_size);

        if (old.alloc == NULL) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
            assert(index < old_capacity);
        } else {
            if (required_size <= old.current_size) {
                assert(list.data == old.data);
                assert(list.current_size == old.current_size);
            } else {
                assert(list.current_size > old.current_size);
                assert(list.current_size >= required_size);
                if (old.current_size > 0) {
                    assert(list.current_size >= old.current_size * 2);
                }
            }
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
}
