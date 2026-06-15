#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    uint8_t *old_data = NULL;
    if (list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    /* 3. Choose indices respecting preconditions */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 5. Postconditions */
    if (a == b) {
        /* No change when indices are equal */
        if (list.current_size > 0) {
            assert(memcmp(list.data, old_data, list.current_size) == 0);
        }
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    } else {
        /* Elements at a and b are swapped, others unchanged */
        for (size_t i = 0; i < list.length; ++i) {
            uint8_t *cur = (uint8_t *)list.data + i * list.item_size;
            if (i == a) {
                assert_bytes_match(cur,
                                   old_data + b * list.item_size,
                                   list.item_size);
            } else if (i == b) {
                assert_bytes_match(cur,
                                   old_data + a * list.item_size,
                                   list.item_size);
            } else {
                assert_bytes_match(cur,
                                   old_data + i * list.item_size,
                                   list.item_size);
            }
        }
        /* Structural fields unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    }

    /* 6. Invariant: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(old_data);
}
