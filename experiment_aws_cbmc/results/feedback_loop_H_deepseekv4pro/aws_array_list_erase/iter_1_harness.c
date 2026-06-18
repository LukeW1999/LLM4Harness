#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_erase_harness() {
    /* Declare and bound data structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic index */
    size_t index = nondet_size_t();

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_storage;
    save_byte_from_array((const uint8_t *)list.data, list.current_size * list.item_size, &old_storage);

    /* Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Length decreased by 1 */
        assert(aws_array_list_length(&list) == old.length - 1);

        /* Data pointer, capacity, item_size, and allocator unchanged */
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* Elements before index are unchanged */
        if (index > 0) {
            assert_bytes_match((const uint8_t *)list.data,
                               (const uint8_t *)old.data,
                               index * list.item_size);
        }

        /* Elements after index (up to old length-1) are shifted left by one */
        if (index < old.length - 1) {
            size_t shift_bytes = (old.length - index - 1) * list.item_size;
            assert_bytes_match((const uint8_t *)list.data + index * list.item_size,
                               (const uint8_t *)old.data + (index + 1) * list.item_size,
                               shift_bytes);
        }

        /* Validity invariant holds */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* Failure: index out of bounds */
        assert(index >= old.length);

        /* List entirely unchanged */
        assert_array_list_equivalence(&list, &old, &old_storage);

        /* Length unchanged */
        assert(aws_array_list_length(&list) == old.length);

        /* Validity invariant holds */
        assert(aws_array_list_is_valid(&list));
    }
}
