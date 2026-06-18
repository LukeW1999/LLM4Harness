#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
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
    struct store_byte_from_buffer old_storage;
    if (old.data) {
        save_byte_from_array(old.data, old.current_size, &old_storage);
    }

    /* 3. Prepare nondeterministic inputs */
    /* val must be readable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    size_t index = nondet_size_t();

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        /* memory at the index matches the input value */
        assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                            val,
                            list.item_size);

        /* length updates only when index is beyond the previous length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* on failure the list must be unchanged */
        assert(result == AWS_OP_ERR);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* memory must be unchanged */
        if (old.data) {
            assert_byte_from_buffer_matches(old.data, &old_storage);
        }

        /* validity invariant */
        assert(aws_array_list_is_valid(&list));
    }

    free(val);
}
