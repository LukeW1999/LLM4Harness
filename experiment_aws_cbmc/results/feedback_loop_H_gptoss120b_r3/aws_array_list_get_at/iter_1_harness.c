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

    /* Save a copy of the original state */
    struct aws_array_list old = list;
    size_t old_len = list.length;

    /* 2. Prepare a readable input buffer */
    size_t item_sz = list.item_size;
    /* item_sz is guaranteed > 0 by the validity predicate */
    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 3. Choose a nondeterministic index, bounded to avoid overflow */
    size_t index = nondet_size_t();
    __CPROVER_assume(item_sz == 0 || index <= SIZE_MAX / item_sz);

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length may increase if we wrote past the previous end */
        if (index >= old_len) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_len);
        }

        /* Fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* The element at the given index must now contain the bytes from val */
        assert_bytes_match((uint8_t *)list.data + index * item_sz,
                           val,
                           item_sz);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (list.data && old.data) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data,
                               list.current_size);
        }
    }

    /* 6. The list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
