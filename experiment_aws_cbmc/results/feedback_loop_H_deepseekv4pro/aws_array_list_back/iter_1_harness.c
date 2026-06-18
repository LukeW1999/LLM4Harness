#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

void aws_array_list_back_harness() {
    struct aws_array_list list;
    /* Bound and initialize the array list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Output buffer large enough for any element */
    uint8_t val_storage[MAX_ITEM_SIZE];
    void *val = (void *)val_storage;

    /* Save old list state for immutability checks */
    struct aws_array_list old_list = list;
    struct store_byte_from_buffer old_list_data;
    if (list.data != NULL) {
        save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_list_data);
    }

    /* Save old output buffer bytes */
    struct store_byte_from_buffer old_val_byte;
    save_byte_from_array(val_storage, list.item_size, &old_val_byte);

    /* Call the function under test */
    int result = aws_array_list_back(&list, val);

    /* Postcondition assertions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: list must have been non-empty */
        assert(aws_array_list_length(&list) > 0);
        /* The output buffer must contain the last element */
        size_t last_item_offset = list.item_size * (aws_array_list_length(&list) - 1);
        assert_bytes_match(val_storage, (const uint8_t *)list.data + last_item_offset, list.item_size);
        /* The list itself must be unchanged */
        assert_array_list_equivalence(&list, &old_list, &old_list_data);
    } else {
        /* Failure: list must have been empty */
        assert(aws_array_list_length(&list) == 0);
        /* List remains unchanged */
        assert_array_list_equivalence(&list, &old_list, &old_list_data);
        /* Output buffer must not have been modified */
        assert_byte_from_buffer_matches(val_storage, &old_val_byte);
    }

    /* The list validity invariant always holds after the call */
    assert(aws_array_list_is_valid(&list));
}
