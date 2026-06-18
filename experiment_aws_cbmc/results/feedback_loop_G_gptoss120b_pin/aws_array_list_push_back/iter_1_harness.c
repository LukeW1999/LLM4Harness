#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

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
    struct store_byte_from_buffer old_data_storage;
    if (old.data) {
        save_byte_from_array((uint8_t *)old.data, old.current_size, &old_data_storage);
    }

    /* 3. Nondeterministic index and value */
    size_t index = nondet_size_t();
    /* bound index to keep allocations reasonable */
    __CPROVER_assume(index < (MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE));

    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* make the input readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
        /* nondet contents */
        uint8_t *val_bytes = (uint8_t *)val;
        for (size_t i = 0; i < list.item_size; ++i) {
            val_bytes[i] = nondet_uint8_t();
        }
    }

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Postconditions */

    /* validity always holds */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* The value at the given index must now equal the input */
        assert_bytes_match((uint8_t *)val,
                           (uint8_t *)list.data + (list.item_size * index),
                           list.item_size);

        /* Length may increase but never decrease */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        assert(list.length >= old.length);

        /* current_size may increase but never decrease */
        assert(list.current_size >= old.current_size);

        /* allocator and item_size never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* data pointer may change; if unchanged, its contents (outside the written index) must be unchanged */
        if (list.data == old.data) {
            /* bytes other than the written region must be unchanged */
            size_t before = index * list.item_size;
            size_t after = (old.length > index ? old.length : index + 1) * list.item_size;
            if (before > 0) {
                assert_bytes_match((uint8_t *)old.data,
                                   (uint8_t *)list.data,
                                   before);
            }
            if (after < old.current_size) {
                assert_bytes_match((uint8_t *)old.data + after,
                                   (uint8_t *)list.data + after,
                                   old.current_size - after);
            }
        } else {
            /* if data changed, we cannot assert its previous contents */
        }
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data) {
            assert_byte_from_buffer_matches((uint8_t *)old.data, &old_data_storage);
        }
    }

    /* clean up */
    free(val);
}
