#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_push_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    list.length = nondet_size_t();
    /* Allocator can be NULL (static) or default allocator (dynamic) */
    list.alloc = (nondet_bool() ? NULL : aws_default_allocator());
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Provide a readable source buffer for val */
    uint8_t val_buf[MAX_ITEM_SIZE];
    const void *val = (const void *)val_buf;

    /* 3. Save old state for immutability checks on failure */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data != NULL) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_push_back(&list, val);

    /* 5. Always assert the list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* 6. Postconditions for success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* Success: length increased by 1 and last byte matches val */
        assert(list.length == old.length + 1);
        assert(list.length <= list.current_size / list.item_size);
        assert_bytes_match((uint8_t *)list.data + old.length * list.item_size, val, list.item_size);

        /* Fields that never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        /* Failure: the list must be completely unchanged */
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        if (old.data != NULL) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }
}
