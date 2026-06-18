#include <proof_helpers/make_common_data_structures.h>

/* Nondeterministic helpers */
static size_t nondet_size_t(void) {
    return __CPROVER_nondet_ulong();
}
static bool nondet_bool(void) {
    return __CPROVER_nondet_bool();
}

void aws_array_list_front_harness(void) {
    struct aws_array_list list;
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 8);               /* bound for CBMC */

    if (nondet_bool()) {
        /* ---------- Dynamic list ---------- */
        size_t initial_capacity = nondet_size_t();
        __CPROVER_assume(initial_capacity > 0);
        __CPROVER_assume(initial_capacity <= 8);   /* bound for CBMC */

        struct aws_allocator *alloc = aws_default_allocator();

        if (aws_array_list_init_dynamic(&list, alloc, initial_capacity, item_size) != AWS_OP_SUCCESS) {
            return;
        }

        size_t length = nondet_size_t();
        __CPROVER_assume(length > 0);
        __CPROVER_assume(length <= initial_capacity);
        list.length = length;
    } else {
        /* ---------- Static list ---------- */
        size_t capacity = nondet_size_t();
        __CPROVER_assume(capacity > 0);
        __CPROVER_assume(capacity <= 8);            /* bound for CBMC */

        uint8_t *raw = malloc(capacity * item_size);
        __CPROVER_assume(raw != NULL);

        aws_array_list_init_static(&list, raw, capacity, item_size);

        size_t length = nondet_size_t();
        __CPROVER_assume(length > 0);
        __CPROVER_assume(length <= capacity);
        list.length = length;
    }

    /* Ensure the list satisfies its invariant before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Snapshot the list's data buffer (frame condition) */
    uint8_t *pre_data = NULL;
    if (list.current_size > 0) {
        pre_data = malloc(list.current_size);
        __CPROVER_assume(pre_data != NULL);
        memcpy(pre_data, list.data, list.current_size);
    }

    /* Allocate output buffer */
    uint8_t *out = malloc(item_size);
    __CPROVER_assume(out != NULL);

    /* Call the function under test */
    int ret = aws_array_list_front(&list, out);

    /* ---------- Post‑conditions ---------- */

    if (ret == AWS_OP_SUCCESS) {
        /* Success implies the list was non‑empty */
        assert(list.length > 0);
        /* The output buffer must contain the first element */
        assert(memcmp(out, list.data, item_size) == 0);
    } else {
        /* Failure must be due to an empty list */
        assert(list.length == 0);
    }

    /* Frame condition – the list must not be modified */
    if (list.current_size > 0) {
        assert(memcmp(pre_data, list.data, list.current_size) == 0);
    }

    /* Clean‑up */
    free(pre_data);
    free(out);
    if (list.alloc && list.data) {
        aws_mem_release(list.alloc, list.data);
    } else {
        free(list.data);
    }
}
