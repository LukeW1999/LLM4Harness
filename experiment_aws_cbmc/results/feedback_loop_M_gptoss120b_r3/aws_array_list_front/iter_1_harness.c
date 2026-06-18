#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
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

    /* 2. Allocate a readable source buffer for the value to set */
    uint8_t val_buf[MAX_ITEM_SIZE];
    /* nondet contents are fine – they just need to be readable */
    const void *val = (const void *)val_buf;

    /* 3. Nondeterministic index, bounded to keep state space finite */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + MAX_ITEM_SIZE);

    /* 4. Save old state for later comparison */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data) {
        save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* item_size and allocator never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* length is updated only when index is beyond the previous length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* the bytes at the target index must now equal the source value */
        assert_bytes_match((const uint8_t *)list.data + (index * list.item_size),
                           val_buf,
                           list.item_size);
    } else {
        /* On failure the list must be unchanged (including its contents) */
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    /* 7. The list must always remain valid */
    assert(aws_array_list_is_valid(&list));
}
