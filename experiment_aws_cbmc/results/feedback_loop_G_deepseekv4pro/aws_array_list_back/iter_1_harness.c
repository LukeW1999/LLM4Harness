#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_array_list_back_harness() {
    struct aws_array_list list;

    /* Bound the array list to avoid state explosion */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    /* Ensure the backing store is allocated */
    ensure_array_list_has_allocated_data_member(&list);
    /* Assume the list is valid according to the library invariants */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save a byte from the data buffer before the call to check immutability later */
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(list.data, list.current_size, &old_byte);

    struct aws_array_list old = list;

    /* Prepare the output buffer val: must be writable and non‑overlapping with the list data */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(__CPROVER_rw_ok(val, list.item_size));
    /* memcpy requires non‑overlapping source and destination; assume caller respects this */
    __CPROVER_assume((uint8_t *)val + list.item_size <= (uint8_t *)list.data ||
                     (uint8_t *)val >= (uint8_t *)list.data + list.current_size);

    int r = aws_array_list_back(&list, val);

    if (r == AWS_OP_SUCCESS) {
        /* The last element was copied into val – verify it matches the source data */
        size_t last_item_offset = list.item_size * (list.length - 1);
        assert_bytes_match(val, (uint8_t *)list.data + last_item_offset, list.item_size);

        /* The list itself must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        /* The list’s data buffer must remain unchanged */
        assert_byte_from_buffer_matches(list.data, &old_byte);
    } else {
        /* Failure – list is empty, nothing should have changed */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);

        assert_byte_from_buffer_matches(list.data, &old_byte);
    }

    /* The list must still satisfy the validity predicate after the call */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
