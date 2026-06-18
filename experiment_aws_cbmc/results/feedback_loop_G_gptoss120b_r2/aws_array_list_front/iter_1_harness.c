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

    /* 2. Non‑deterministic index and value */
    size_t index = nondet_size_t();
    /* item_size is guaranteed non‑zero by the validity predicate */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 3. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (old.data != NULL && index < old.length) {
        save_byte_from_array((uint8_t *)old.data + index * old.item_size,
                             old.item_size,
                             &old_bytes);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* validity must hold */
        assert(aws_array_list_is_valid(&list));

        /* the element at the given index must now equal the supplied value */
        assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                           val,
                           list.item_size);

        /* length updates */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* fields that never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* if the underlying buffer was not reallocated, all untouched elements stay the same */
        if (list.data == old.data) {
            for (size_t i = 0; i < old.length; ++i) {
                if (i != index) {
                    assert_bytes_match((uint8_t *)list.data + i * list.item_size,
                                       (uint8_t *)old.data + i * old.item_size,
                                       list.item_size);
                }
            }
        }
    } else {
        /* on failure the list must be unchanged */
        assert(aws_array_list_is_valid(&list));
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 6. Clean up */
    free(val);
}
