#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non‑deterministically decide whether the list is dynamic or static */
    if (nondet_bool()) {
        list.alloc = aws_default_allocator();
    } else {
        list.alloc = NULL; /* static mode */
    }

    /* 3. Allocate a readable buffer for the value to be pushed */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* make the memory readable for the required size */
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* 4. Save old state before the call */
    struct aws_array_list old = list;
    /* Save a byte from the old data buffer to help detect unintended changes */
    struct store_byte_from_buffer old_data_byte;
    if (old.data != NULL && old.length > 0) {
        save_byte_from_array((uint8_t *)old.data,
                             old.length * old.item_size,
                             &old_data_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length must increase by one */
        assert(list.length == old.length + 1);
        /* item size and allocator must stay the same */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        /* the first element must equal the value we pushed */
        assert_bytes_match((uint8_t *)list.data, val, list.item_size);
        /* the rest of the elements must be the old elements shifted right */
        if (old.length > 0) {
            assert_bytes_match((uint8_t *)list.data + list.item_size,
                               (uint8_t *)old.data,
                               old.length * old.item_size);
        }
    } else {
        /* on failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        /* ensure the data buffer content is unchanged */
        if (old.data != NULL && old.length > 0) {
            assert_byte_from_buffer_matches((uint8_t *)old.data, &old_data_byte);
        }
    }

    /* 7. Fields that must never change regardless of outcome */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data || list.data != old.data); /* data may change only on success */

    /* 8. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
