#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state for immutability checks */
    struct aws_array_list old = list;
    size_t data_size = old.current_size * old.item_size;
    struct store_byte_from_buffer storage;
    save_byte_from_array((uint8_t *)old.data, data_size, &storage);

    /* 3. Non-deterministic index */
    size_t index = nondet_size_t();

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Postconditions */
    /* Unchanged fields (always) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (result == AWS_OP_SUCCESS) {
        /* Success: element at index removed, length decreases by 1 */
        assert(list.length == old.length - 1);

        /* Data before index unchanged */
        if (index > 0) {
            assert_bytes_match((uint8_t *)old.data, (uint8_t *)list.data, index * list.item_size);
        }

        /* Data after index shifted left by one element */
        for (size_t i = index; i < list.length; i++) {
            assert_bytes_match(
                (uint8_t *)old.data + (i + 1) * list.item_size,
                (uint8_t *)list.data + i * list.item_size,
                list.item_size);
        }
    } else {
        /* Failure: list completely unchanged */
        assert(list.length == old.length);
        assert_byte_from_buffer_matches((uint8_t *)list.data, &storage);
    }

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
