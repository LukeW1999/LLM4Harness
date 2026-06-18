#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Prepare the value to push */
    uint8_t val_buf[MAX_ITEM_SIZE];
    const void *val = val_buf;
    /* val is always readable for list->item_size bytes because it's a local array */
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(list.data, list.current_size * list.item_size, &old_byte);

    /* 4. Call function under test */
    int rval = aws_array_list_push_front(&list, val);

    /* 5. Postconditions */
    if (rval == AWS_OP_SUCCESS) {
        /* Success: length increased by 1 */
        assert(list.length == old.length + 1);
        /* item_size and allocator unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        /* capacity may have grown, but never shrinks */
        assert(list.current_size >= old.current_size);
        /* first element equals val */
        assert_bytes_match((const uint8_t *)list.data, (const uint8_t *)val, list.item_size);
        /* remaining elements shifted right */
        if (old.length > 0) {
            assert_bytes_match((const uint8_t *)list.data + list.item_size,
                               old.data,
                               old.length * old.item_size);
        }
        /* list remains valid */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* Failure: list must be completely unchanged */
        assert(rval != AWS_OP_SUCCESS);
        assert_array_list_equivalence(&list, &old, &old_byte);
        /* validity invariant still holds (same as old) */
        assert(aws_array_list_is_valid(&list));
    }
}
