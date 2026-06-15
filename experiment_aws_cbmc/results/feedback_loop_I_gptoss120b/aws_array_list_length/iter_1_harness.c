#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes = {0};
    if (list.data) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_bytes);
    }

    /* 3. Prepare nondeterministic inputs */
    size_t index = nondet_size_t();

    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* make the memory readable – contents are nondet */
    }

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length may increase to index+1 if index was beyond the old length */
        size_t expected_length = old.length;
        if (index >= old.length) {
            expected_length = index + 1;
        }
        assert(list.length == expected_length);

        /* the element at `index` must now equal `val` */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           (uint8_t *)val,
                           list.item_size);

        /* all other existing elements must remain unchanged */
        for (size_t i = 0; i < old.length; ++i) {
            if (i != index && i < list.length) {
                assert_bytes_match((uint8_t *)list.data + (i * list.item_size),
                                   (uint8_t *)old.data + (i * old.item_size),
                                   list.item_size);
            }
        }
    } else {
        /* on failure the whole structure must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        if (list.data) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_bytes);
        }
    }

    /* 6. Fields that never change regardless of outcome */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 7. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
