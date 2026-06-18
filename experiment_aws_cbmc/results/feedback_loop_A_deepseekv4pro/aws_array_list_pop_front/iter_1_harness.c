#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>

void aws_array_list_pop_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state for comparison */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)old.data, old.length * old.item_size, &old_byte);

    /* 3. Call function under test */
    int result = aws_array_list_pop_front(&list);

    /* 4. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: list was not empty, first element removed, rest shifted left */
        assert(old.length > 0);
        assert(list.length == old.length - 1);
        assert_bytes_match((uint8_t *)list.data,
                           (uint8_t *)old.data + old.item_size,
                           (old.length - 1) * old.item_size);
    } else {
        /* Failure: list was empty, no change */
        assert(old.length == 0);
        assert(list.length == 0);
        assert_bytes_match((uint8_t *)list.data, (uint8_t *)old.data, 0);
    }

    /* 5. Fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
