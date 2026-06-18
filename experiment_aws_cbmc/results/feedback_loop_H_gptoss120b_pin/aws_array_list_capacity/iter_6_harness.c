#include <assert.h>
#include <stdlib.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    list.allocator = aws_default_allocator();
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        for (size_t i = 0; i < list.item_size; ++i) {
            val[i] = nondet_uint8_t();
        }
    }

    size_t index = (size_t)nondet_uint64_t();

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        if (list.data) {
            assert_bytes_match(
                (uint8_t *)list.data + (index * list.item_size),
                (uint8_t *)val,
                list.item_size);
        }

        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        if (list.data && old_byte.buffer) {
            size_t changed_start = index * list.item_size;
            size_t changed_end = changed_start + list.item_size;
            if (old_byte.offset < changed_start || old_byte.offset >= changed_end) {
                assert_byte_from_buffer_matches(list.data, &old_byte);
            }
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    assert(aws_array_list_is_valid(&list));

    free(val);
}
