#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
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
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);

    uint8_t val[MAX_ITEM_SIZE];
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_byte);

    size_t old_capacity = old.current_size / old.item_size;

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        size_t offset = 0;
        assert(aws_mul_size_checked(index, list.item_size, &offset) == AWS_OP_SUCCESS);

        assert(list.length >= old.length);
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        assert(list.current_size >= (index + 1) * list.item_size);
        assert_bytes_match((const uint8_t *)list.data + offset, val, list.item_size);

        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        if (index < old_capacity) {
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
        }

        if (old.alloc == NULL) {
            assert(list.current_size == old.current_size);
            assert(list.data == old.data);
            assert(index < old_capacity);
        } else {
            assert(list.current_size >= old.current_size);
            if (index >= old_capacity) {
                assert(list.current_size >= (index + 1) * list.item_size);
                if (old_capacity > 0) {
                    assert((list.current_size / list.item_size) >= old_capacity * 2);
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

        if (old.alloc == NULL && index >= old_capacity) {
            assert(result == AWS_OP_ERR);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
}
