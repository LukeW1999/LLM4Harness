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

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Use the default allocator */
    list.alloc = aws_default_allocator();

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable input value */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Save old state */
    struct aws_array_list old = list;

    struct store_byte_from_buffer storage;
    if (old.length > 0) {
        save_byte_from_array(old.data,
                             old.length * old.item_size,
                             &storage);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length increased by one */
        assert(list.length == old.length + 1);

        /* fields that must stay the same */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* first element now equals the input value */
        assert_bytes_match((uint8_t *)list.data,
                           (const uint8_t *)val,
                           list.item_size);

        /* existing elements shifted right by one slot */
        if (old.length > 0) {
            assert_byte_from_buffer_matches(
                (uint8_t *)list.data + list.item_size,
                &storage);
        }
    } else {
        /* on failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 6. The list must remain valid in all cases */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
