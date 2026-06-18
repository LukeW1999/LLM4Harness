#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                                MAX_INITIAL_ITEM_ALLOCATION,
                                                MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* make the buffer readable for CBMC */
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes = {0};
    if (old.data) {
        save_byte_from_array(old.data, old.current_size, &old_bytes);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Global validity invariant must hold */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* ----- Success path ----- */

        /* length may increase to index+1 if index was beyond the old length */
        size_t expected_len = old.length;
        if (index >= old.length) {
            expected_len = index + 1;
        }
        assert(list.length == expected_len);

        /* the data at the target index must now equal the source buffer */
        if (list.data) {
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                               val,
                               list.item_size);
        }

        /* fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* current_size may change (capacity growth), so we do not assert it */
    } else {
        /* ----- Failure path ----- */

        /* the whole structure must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);

        /* memory backing the list must be unchanged */
        if (old.data) {
            assert_byte_from_buffer_matches(list.data, &old_bytes);
        }
    }
}
