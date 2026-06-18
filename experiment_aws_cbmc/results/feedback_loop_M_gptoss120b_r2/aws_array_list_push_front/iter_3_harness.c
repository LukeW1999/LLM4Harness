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
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.length <= list.capacity);
    __CPROVER_assume(list.alloc == aws_default_allocator());

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a nondeterministic value to push */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data_byte;
    if (old.data != NULL && old.length > 0) {
        save_byte_from_array((uint8_t *)old.data,
                             old.length * old.item_size,
                             &old_data_byte);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length must increase by one */
        assert(list.length == old.length + 1);
        /* item size and allocator must stay the same */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        /* the new first element must equal the value we pushed */
        assert_bytes_match((uint8_t *)list.data,
                           (uint8_t *)val,
                           list.item_size);
        /* previously existing elements must be shifted right */
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
        assert(list.data == old.data);
        if (old.data != NULL && old.length > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data,
                                            &old_data_byte);
        }
    }

    /* 6. Invariant: the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
