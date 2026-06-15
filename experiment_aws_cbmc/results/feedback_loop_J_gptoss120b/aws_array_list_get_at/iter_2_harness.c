#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable destination buffer */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data_byte = {0};
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_data_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* 6. The array list must always be valid after the call */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* The element at the given index must now equal the destination buffer */
        if (list.data != NULL && list.item_size > 0) {
            assert_bytes_match(
                (uint8_t *)list.data + (list.item_size * index),
                val,
                list.item_size);
        }

        /* Length and other fields must remain unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (list.data != NULL && list.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_data_byte);
        }
    }

    /* Clean up */
    free(val);
}
