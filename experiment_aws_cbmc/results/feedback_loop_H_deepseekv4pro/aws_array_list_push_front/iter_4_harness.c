#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>

void aws_array_list_push_front_harness() {
    /* Create and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* Value to push */
    uint8_t val_buf[MAX_ITEM_SIZE];
    const void *val = val_buf;

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(list.data, list.current_size * list.item_size, &old_byte);

    /* Because data may be freed on a successful realloc, copy old contents for later comparison */
    uint8_t old_data[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    size_t old_length = old.length;
    size_t old_item_size = old.item_size;
    size_t old_data_size = old.current_size * old.item_size;
    __CPROVER_assume(old_data_size <= sizeof(old_data));
    for (size_t i = 0; i < old_data_size; ++i) {
        old_data[i] = ((uint8_t *)old.data)[i];
    }

    /* Call the function under verification */
    int rval = aws_array_list_push_front(&list, val);

    /* Postconditions */
    if (rval == AWS_OP_SUCCESS) {
        /* Success: length increased by 1 */
        assert(list.length == old_length + 1);
        /* item_size and allocator unchanged */
        assert(list.item_size == old_item_size);
        assert(list.alloc == old.alloc);
        /* Capacity never shrinks */
        assert(list.current_size >= old.current_size);
        /* The first element equals val */
        assert_bytes_match((const uint8_t *)list.data, (const uint8_t *)val, list.item_size);
        /* The rest of the elements are the old content shifted right */
        if (old_length > 0) {
            assert_bytes_match((const uint8_t *)list.data + list.item_size,
                               old_data,
                               old_length * old_item_size);
        }
        /* The list remains valid */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* Failure: list state is unmodified */
        assert_array_list_equivalence(&list, &old, &old_byte);
        assert(aws_array_list_is_valid(&list));
    }
}
