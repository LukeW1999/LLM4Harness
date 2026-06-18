#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    size_t index = nondet_size_t();
    uint8_t value[MAX_ITEM_SIZE + 1];

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    if (list.alloc != NULL) {
        list.alloc = aws_default_allocator();
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(index < SIZE_MAX);
    __CPROVER_assume(index + 1 <= SIZE_MAX / list.item_size);
    __CPROVER_assume(AWS_MEM_IS_READABLE(value, list.item_size));

    struct aws_array_list old = list;
    size_t old_capacity = aws_array_list_capacity(&old);
    size_t required_size = (index + 1) * old.item_size;
    size_t target_offset = index * old.item_size;

    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_byte);

    int result = aws_array_list_set_at(&list, value, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        assert(list.length >= old.length);
        assert(list.current_size >= required_size);
        assert(aws_array_list_capacity(&list) > index);
        assert_bytes_match((const uint8_t *)list.data + target_offset, value, list.item_size);

        if (index < old_capacity) {
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        } else {
            assert(old.alloc != NULL);
            assert(list.current_size > old.current_size);
            if (old.current_size <= SIZE_MAX / 2) {
                assert(list.current_size >= old.current_size * 2);
            }
        }

        if (old.alloc == NULL) {
            assert(index < old_capacity);
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(index >= old_capacity);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert_array_list_equivalence(&list, &old, &old_byte);

        if (old.alloc == NULL) {
            assert(index >= old_capacity);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (old.alloc == NULL) {
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    assert(aws_array_list_is_valid(&list));
}
