#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound data structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    size_t old_data_bytes = list.length * list.item_size;
    uint8_t *old_bytes = NULL;
    if (old_data_bytes > 0) {
        old_bytes = malloc(old_data_bytes);
        __CPROVER_assume(old_bytes != NULL);
        memcpy(old_bytes, list.data, old_data_bytes);
    }

    /* 3. Nondeterministic indices respecting preconditions */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 5. Postconditions */

    /* Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* Content changes */
    if (a == b) {
        /* No change at all */
        if (old_data_bytes > 0) {
            assert(memcmp(list.data, old_bytes, old_data_bytes) == 0);
        }
    } else {
        uint8_t *data = (uint8_t *)list.data;
        /* Verify swapped elements */
        assert(memcmp(data + a * list.item_size,
                      old_bytes + b * list.item_size,
                      list.item_size) == 0);
        assert(memcmp(data + b * list.item_size,
                      old_bytes + a * list.item_size,
                      list.item_size) == 0);
        /* Verify other elements unchanged */
        for (size_t i = 0; i < list.length; ++i) {
            if (i != a && i != b) {
                assert(memcmp(data + i * list.item_size,
                              old_bytes + i * list.item_size,
                              list.item_size) == 0);
            }
        }
    }

    /* 6. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(old_bytes);
}
