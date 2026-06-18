#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
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

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data = {0};
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_data);
    }

    /* 3. Prepare nondeterministic inputs */
    /* val must point to readable memory of list->item_size bytes */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    size_t index = nondet_size_t();
    /* bound index to avoid overflow in capacity calculations */
    if (list.item_size > 0) {
        size_t max_index = MAX_BUFFER_SIZE / list.item_size;
        __CPROVER_assume(index <= max_index);
    } else {
        __CPROVER_assume(index == 0);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* validity must hold */
        assert(aws_array_list_is_valid(&list));

        /* length is updated if needed */
        size_t expected_len = (old.length > index) ? old.length : index + 1;
        assert(list.length == expected_len);

        /* the stored element matches the input value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);

        /* fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* current_size never decreases (may increase in dynamic mode) */
        assert(list.current_size >= old.current_size);
    } else {
        /* on failure the list must be unchanged */
        assert(aws_array_list_is_valid(&list));
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        if (list.data) {
            assert_byte_from_buffer_matches(list.data, &old_data);
        }
    }
}
