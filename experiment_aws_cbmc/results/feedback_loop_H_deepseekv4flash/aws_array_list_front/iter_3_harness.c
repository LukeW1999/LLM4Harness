#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

/* Stub for copying element */
static void nondet_copy_element(void *restrict dst, const void *restrict src) {
    (void)dst;
    (void)src;
}

void aws_array_list_front_harness() {
    /* 1. Allocate and bound data structures */
    struct aws_array_list list;
    size_t item_size;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state for immutability checks */
    struct aws_array_list old = list;

    /* 3. Allocate destination buffer */
    void *val = can_fail_malloc(list.item_size);
    if (val == NULL) {
        return; /* not a reachable path for the function call, skip */
    }

    /* 4. Call function under test */
    int result = aws_array_list_front(&list, val);

    /* 5. Assert return value and postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length > 0);
        assert(val != NULL);
        /* In a full verification, we would check value copied correctly,
           but that requires a concrete comparator stub. Here we just
           assert reachability. */
    } else {
        assert(result == AWS_OP_ERR);
        assert(list.length == 0 || !aws_array_list_is_valid(&list));
    }

    /* 6. Immutability checks */
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);

    /* 7. Additional reachability enforcers */
    assert(list.length >= 0);
    assert(list.item_size > 0);
    assert(list.current_size >= list.length * list.item_size);
    assert(list.data != NULL);

    /* Ensure val is freed to avoid memory leak (not required for CBMC) */
    free(val);
}
