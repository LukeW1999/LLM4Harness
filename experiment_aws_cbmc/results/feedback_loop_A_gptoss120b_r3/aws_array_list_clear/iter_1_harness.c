#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* 2. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 3. Allocate a readable source buffer of size item_size */
    size_t item_sz = list.item_size;
    uint8_t *src = malloc(item_sz);
    __CPROVER_assume(src != NULL);
    /* make the source readable – malloc already provides readable memory */
    /* (optional) fill with nondet data */
    for (size_t i = 0; i < item_sz; ++i) {
        src[i] = nondet_uint8_t();
    }

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, src, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The element at the given index must now equal the source bytes */
        assert_bytes_match(
            (const uint8_t *)list.data + (list.item_size * index),
            src,
            list.item_size);

        /* Length handling */
        if (index >= old.length) {
            /* Length should become index+1 */
            assert(list.length == index + 1);
        } else {
            /* Length unchanged */
            assert(list.length == old.length);
        }

        /* Capacity (current_size) never shrinks */
        assert(list.current_size >= old.current_size);

        /* Fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);

        /* Data buffer content unchanged */
        if (list.data != NULL && old.current_size > 0) {
            assert_bytes_match(
                (const uint8_t *)list.data,
                (const uint8_t *)old.data,
                old.current_size);
        }

        /* Result must be error */
        assert(result == AWS_OP_ERR);
    }

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
