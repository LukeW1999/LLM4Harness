#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    void *val = NULL;
    if (list.item_size > 0) {
        val = allocator->mem_acquire(allocator, list.item_size);
        __CPROVER_assume(val != NULL);
        uint8_t *p = (uint8_t *)val;
        for (size_t i = 0; i < list.item_size; ++i) {
            p[i] = nondet_uint8_t();
        }
    }

    size_t index = nondet_size_t();

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        size_t expected_len = old.length;
        if (index >= old.length) {
            expected_len = index + 1;
        }
        __CPROVER_assert(list.length == expected_len, "length updated correctly");
        if (list.data) {
            assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                               (uint8_t *)val,
                               list.item_size);
        }
    } else {
        __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged on failure");
        __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged on failure");
        __CPROVER_assert(list.current_size == old.current_size, "current_size unchanged on failure");
        __CPROVER_assert(list.length == old.length, "length unchanged on failure");
        __CPROVER_assert(list.data == old.data, "data pointer unchanged on failure");
        if (list.data && list.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    __CPROVER_assert(list.alloc == old.alloc, "alloc never changes");
    __CPROVER_assert(list.item_size == old.item_size, "item_size never changes");
    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
}
