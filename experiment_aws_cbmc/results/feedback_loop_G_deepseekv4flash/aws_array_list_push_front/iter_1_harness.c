#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Ensure capacity will not need to grow during the call */
    __CPROVER_assume(list.current_size >= (list.length + 1) * list.item_size);

    /* 2. Save old state and a byte from the first element (if any) */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_first_byte;
    if (list.length > 0) {
        save_byte_from_array((uint8_t *)list.data, list.item_size, &old_first_byte);
    }

    /* 3. Nondeterministic val pointer — must be readable */
    const void *val;
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* length increased by 1 */
        assert(list.length == old.length + 1);

        /* first element now equals val */
        assert_bytes_match((uint8_t *)list.data, (uint8_t *)val, list.item_size);

        /* if the list was not empty, the old first element moved to index 1 */
        if (old.length > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data + list.item_size, &old_first_byte);
        }

        /* unchanged fields (since no reallocation occurred) */
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* On failure, the list must be completely unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        if (old.length > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_first_byte);
        }
    }

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
