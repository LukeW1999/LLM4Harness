#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Set up allocator and initialize the array list */
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;
    aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for the value to set */
    uint8_t *val = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data = {0};
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_data);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The list must remain valid */
        assert(aws_array_list_is_valid(&list));

        /* allocator and item size never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* Length grows only when index is beyond the old length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* The element at the given index now matches the source value */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index),
                           val,
                           list.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* If the data buffer existed, its contents must be unchanged */
        if (list.data) {
            assert_byte_from_buffer_matches(list.data, &old_data);
        }

        /* The list must still be valid */
        assert(aws_array_list_is_valid(&list));
    }

    /* 7. The function always returns either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
