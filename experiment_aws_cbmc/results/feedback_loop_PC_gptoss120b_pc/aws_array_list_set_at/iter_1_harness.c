#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
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
    /* Use the default allocator */
    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 3. Allocate a readable buffer for val */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    uint8_t *val_buf = malloc(list.item_size);
    __CPROVER_assume(val_buf != NULL);
    /* make the buffer readable (nondet contents) */
    for (size_t i = 0; i < list.item_size; ++i) {
        val_buf[i] = nondet_uint8_t();
    }

    /* 4. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data_storage;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array((uint8_t *)old.data, old.current_size, &old_data_storage);
    }

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val_buf, index);

    /* 6. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* The element at the given index must now match the supplied value */
        assert(list.data != NULL);
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index),
                           val_buf,
                           list.item_size);

        /* Length update semantics */
        if (index >= old.length) {
            /* On success the addition must have succeeded, so length == index + 1 */
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        /* On failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        if (old.data != NULL && old.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)old.data, &old_data_storage);
        }
    }

    /* 7. Fields that never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val_buf);
}
