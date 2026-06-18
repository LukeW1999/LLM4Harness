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

    /* 2. Allocate a readable input value */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* make the memory readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_bytes);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length is updated to at least index+1 */
        size_t expected_len = old.length;
        if (index >= old.length) {
            expected_len = index + 1;
        }
        assert(list.length == expected_len);

        /* The stored element matches the input value */
        if (list.data != NULL && list.item_size > 0) {
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                               (uint8_t *)val,
                               list.item_size);
        }

        /* Fields that must not change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* Unchanged memory regions (except the region we wrote) */
        assert_array_list_equivalence(&list, &old, &old_bytes);
    } else {
        /* On failure the list must remain unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert_array_list_equivalence(&list, &old, &old_bytes);
    }

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
}
