#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    list.allocator = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    size_t old_len = old.length;
    size_t item_sz = old.item_size;

    uint8_t *old_bytes = NULL;
    if (old_len > 0 && item_sz > 0) {
        old_bytes = malloc(old_len * item_sz);
        __CPROVER_assume(old_bytes != NULL);
        memcpy(old_bytes, old.data, old_len * item_sz);
    }

    /* 3. Nondeterministic n, bounded by old_len */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= old_len);

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Postconditions */
    if (n >= old_len) {
        /* List cleared */
        assert(list.length == 0);
    } else {
        /* Elements shifted forward */
        assert(list.length == old_len - n);
        if (list.length > 0 && item_sz > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               old_bytes + n * item_sz,
                               list.length * item_sz);
        }
    }

    /* 6. Unchanged fields (except data pointer may change) */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    /* list.data may be reallocated, so we do not assert equality */

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    free(old_bytes);
}
