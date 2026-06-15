#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness(void) {
    /* 1. Allocate and bound the array list */
    struct aws_array_list list;
    list.allocator = aws_default_allocator();
    __CPROVER_assume(list.allocator != NULL);

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Preserve old state */
    struct aws_array_list old = list;

    if (old.item_size != 0) {
        __CPROVER_assume(old.length <= SIZE_MAX / old.item_size);
    }

    uint8_t *old_data_copy = NULL;
    if (list.data != NULL && list.item_size > 0 && list.length > 0) {
        size_t old_bytes = list.item_size * list.length;
        old_data_copy = malloc(old_bytes);
        if (old_data_copy != NULL) {
            memcpy(old_data_copy, list.data, old_bytes);
        }
    }

    /* 3. Nondeterministic n (pop count) */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= old.length);
    if (old.item_size != 0) {
        __CPROVER_assume(n <= SIZE_MAX / old.item_size);
    }

    /* 4. Call the function under test */
    int rv = aws_array_list_pop_front_n(&list, n);
    assert(rv == 0);

    /* 5. Post‑condition checks */
    if (n >= old.length) {
        /* All elements removed */
        assert(list.length == 0);
        assert(list.data == old.data);
    } else {
        /* Some elements remain */
        assert(list.length == old.length - n);
        assert(list.data == old.data);
        if (list.length > 0 && list.item_size > 0 && old_data_copy != NULL) {
            size_t moved_bytes = list.length * list.item_size;
            assert_bytes_match((uint8_t *)list.data,
                               old_data_copy + n * old.item_size,
                               moved_bytes);
        }
    }

    /* 6. Frame condition: unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.allocator == old.allocator);

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    free(old_data_copy);
}
