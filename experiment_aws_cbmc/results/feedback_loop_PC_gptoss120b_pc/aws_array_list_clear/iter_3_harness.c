#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Snapshot old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes = {0};
    if (list.data && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_bytes);
    }

    /* 3. Nondeterministic inputs */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        for (size_t i = 0; i < list.item_size; ++i) {
            ((uint8_t *)val)[i] = nondet_uint8_t();
        }
    }

    size_t index = nondet_size_t();
    __CPROVER_assume(list.item_size == 0 || index < (SIZE_MAX / list.item_size));
    __CPROVER_assume(index <= SIZE_MAX - 1);

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. General return value sanity */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        size_t expected_length = (index >= old.length) ? (index + 1) : old.length;
        assert(list.length == expected_length);

        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           (uint8_t *)val,
                           list.item_size);

        if (index > 0 && old.data) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data,
                               index * list.item_size);
        }

        if (old.length > index + 1 && old.data) {
            size_t after_offset = (index + 1) * list.item_size;
            size_t after_len = (old.length - (index + 1)) * list.item_size;
            assert_bytes_match((uint8_t *)list.data + after_offset,
                               (uint8_t *)old.data + after_offset,
                               after_len);
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (list.data && list.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_bytes);
        }
    }

    /* 6. Invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
