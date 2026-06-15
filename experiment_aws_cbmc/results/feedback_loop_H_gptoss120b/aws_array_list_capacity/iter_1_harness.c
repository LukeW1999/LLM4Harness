#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source value */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. The array list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* ----- Success path ----- */

        /* The element at the given index must now equal the source value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);

        /* Length handling */
        if (index >= old.length) {
            /* Length should have grown to index+1 */
            assert(list.length == index + 1);
        } else {
            /* Length must be unchanged */
            assert(list.length == old.length);
        }

        /* Fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* Data pointer may change (reallocation), but unchanged bytes outside the
         * written region must remain the same. */
        if (list.data != old.data && old.data != NULL) {
            /* If the buffer was reallocated, we cannot directly compare pointers,
             * but the contents that were not overwritten must be identical. */
            /* Compare a byte that was saved from the old buffer (if any) */
            if (old.current_size > 0) {
                assert_byte_from_buffer_matches(list.data, &old_byte);
            }
        } else if (list.data == old.data && old.data != NULL) {
            /* Same buffer – ensure unchanged region matches */
            if (old.current_size > 0) {
                size_t unchanged_bytes = old.length * old.item_size;
                if (index * old.item_size < unchanged_bytes) {
                    size_t prefix = index * old.item_size;
                    if (prefix > 0) {
                        assert_bytes_match((uint8_t *)list.data,
                                           (uint8_t *)old.data,
                                           prefix);
                    }
                }
                size_t suffix_start = (index + 1) * old.item_size;
                if (suffix_start < unchanged_bytes) {
                    size_t suffix_len = unchanged_bytes - suffix_start;
                    assert_bytes_match((uint8_t *)list.data + suffix_start,
                                       (uint8_t *)old.data + suffix_start,
                                       suffix_len);
                }
            }
        }

    } else {
        /* ----- Failure path ----- */

        /* On error the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* Ensure the previously saved byte (if any) is still the same */
        if (old.data != NULL && old.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    /* Clean up */
    free(val);
}
