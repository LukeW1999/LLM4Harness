#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
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

    /* Save old state */
    struct aws_array_list old = list;

    /* Prepare a readable input value */
    size_t item_sz = list.item_size;
    /* item_sz is guaranteed > 0 by validity predicate */
    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* Choose a nondeterministic index, bounded to avoid overflow */
    size_t index = nondet_size_t();
    __CPROVER_assume(item_sz == 0 || index <= SIZE_MAX / item_sz);

    /* 3. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Post‑condition checks for both success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* The element at the given index must now contain the input bytes */
        assert_bytes_match((uint8_t *)list.data + (index * item_sz), val, item_sz);

        /* Length updates */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        /* On failure the list must remain unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
    }

    /* 5. Fields that never change regardless of the result */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    /* 6. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
}
