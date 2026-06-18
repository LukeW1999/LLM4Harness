#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for the value to set */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        for (size_t i = 0; i < list.item_size; ++i) {
            val[i] = nondet_uint8_t();
        }
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Save a byte from the original data (if any) for later immutability check */
    struct store_byte_from_buffer old_byte;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    /* 6. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 7. The array list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    /* 8. Unchanged fields that are guaranteed not to change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        /* Compute the byte offset for the target element */
        size_t offset = index * list.item_size;

        /* After a successful set, the memory at the target location must match the source value */
        assert_bytes_match((uint8_t *)list.data + offset, val, list.item_size);

        /* Length handling */
        if (index >= old.length) {
            /* Length should have grown to index + 1 */
            assert(list.length == index + 1);
        } else {
            /* Length should be unchanged */
            assert(list.length == old.length);
        }
    } else {
        /* On failure the whole structure must be unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        /* Also ensure the previously saved byte (if any) is unchanged */
        if (list.data != NULL && list.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    /* Clean up */
    free(val);
}
