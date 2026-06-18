#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
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

    /* 2. Allocate a readable source value */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* fill with nondet data */
    for (size_t i = 0; i < list.item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 3. Choose a nondeterministic index (bounded to keep state space reasonable) */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= list.length + 5); /* reasonable bound */

    /* 4. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_storage;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_storage);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Global validity invariant */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* Unchanged fields */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* Length change semantics */
        if (index >= old.length) {
            assert(list.length == old.length + 1);
        } else {
            assert(list.length == old.length);
        }

        /* The stored value must be copied to the target location */
        assert_bytes_match((const uint8_t *)list.data + (index * list.item_size),
                           (const uint8_t *)val,
                           list.item_size);

        /* All other bytes remain unchanged */
        for (size_t i = 0; i < list.length; ++i) {
            if (i == index) continue;
            size_t offset = i * list.item_size;
            if (old.current_size > offset) {
                assert_bytes_match((const uint8_t *)list.data + offset,
                                   (const uint8_t *)old.data + offset,
                                   list.item_size);
            }
        }
    } else {
        /* On failure the list must be unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert_array_list_equivalence(&list, &old, &old_storage);
    }

    /* Clean up */
    free(val);
}
