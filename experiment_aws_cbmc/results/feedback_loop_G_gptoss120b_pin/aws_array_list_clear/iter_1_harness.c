#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                                MAX_INITIAL_ITEM_ALLOCATION,
                                                MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for the value to set */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* make the source buffer readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state before the call */
    struct aws_array_list old = list;
    /* Save a byte from the old data buffer for immutability checks on failure */
    struct store_byte_from_buffer old_byte;
    if (old.data) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The data at the given index must now equal the source value */
        assert(list.data != NULL);
        assert(index < list.current_size / list.item_size);
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);

        /* Length is updated if we wrote past the previous length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* current_size and data pointer may change in dynamic mode, so no assert on them */
    } else {
        /* On failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (list.data) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    /* 7. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    if (val) {
        free(val);
    }
}
