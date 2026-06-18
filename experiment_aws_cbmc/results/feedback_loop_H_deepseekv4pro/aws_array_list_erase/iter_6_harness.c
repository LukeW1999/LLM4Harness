#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness() {
    struct aws_array_list list;

    /* Bound the list fields to avoid overflow and ensure a realistic size */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Use the default (non-failing) allocator */
    list.alloc = aws_default_allocator();

    /* Allocate the data buffer */
    size_t alloc_size = list.current_size * list.item_size;
    /* The bounded assumption already guarantees no multiplication overflow */
    uint8_t *data = malloc(alloc_size);
    __CPROVER_assume(data != NULL);
    list.data = data;

    /* Enforce the full validity invariant */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save the pre-call state for postcondition checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, alloc_size, &old_byte);

    /* Non-deterministic index to erase */
    size_t index = nondet_size_t();

    /* Call the function under verification */
    int result = aws_array_list_erase(&list, index);

    if (result == AWS_OP_SUCCESS) {
        /* Length must decrease by one */
        assert(list.length == old.length - 1);

        /* Structural fields must remain unchanged */
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* Prefix before the erased element is untouched */
        if (index > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data,
                               index * list.item_size);
        }

        /* Suffix after the erased element is shifted left by one element */
        if (index < old.length - 1) {
            size_t tail_bytes = (old.length - index - 1) * list.item_size;
            assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                               (uint8_t *)old.data + (index + 1) * list.item_size,
                               tail_bytes);
        }

        /* The list must still be valid */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* Failure only occurs for out-of-bounds index */
        assert(index >= old.length);

        /* The list must be completely unchanged */
        assert_array_list_equivalence(&list, &old, &old_byte);

        /* Validity invariant still holds */
        assert(aws_array_list_is_valid(&list));
    }

    free(data);
}
