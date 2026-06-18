#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    if (list.alloc != NULL) {
        list.alloc = aws_default_allocator();
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_havoc_object(val);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_byte);

    int result = aws_array_list_set_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        size_t index_plus_one = 0;
        assert(aws_add_size_checked(index, 1, &index_plus_one) == AWS_OP_SUCCESS);

        assert(list.length > index);

        if (index >= old.length) {
            assert(list.length == index_plus_one);
        } else {
            assert(list.length == old.length);
        }

        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        if (old.alloc == NULL) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
            assert(index < old.current_size / old.item_size);
        } else {
            assert(list.current_size >= old.current_size);
            if (index < old.current_size / old.item_size) {
                assert(list.data == old.data);
                assert(list.current_size == old.current_size);
            }
        }

        size_t offset = 0;
        size_t end = 0;
        assert(aws_mul_size_checked(index, list.item_size, &offset) == AWS_OP_SUCCESS);
        assert(aws_add_size_checked(offset, list.item_size, &end) == AWS_OP_SUCCESS);
        assert(end <= list.current_size);
        assert_bytes_match((const uint8_t *)list.data + offset, val, list.item_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert_array_list_equivalence(&list, &old, &old_byte);

        if (old.alloc == NULL) {
            assert(index >= old.current_size / old.item_size);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
