#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>

size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
int nondet_int(void);

void aws_array_list_get_at_harness(void) {
    /* allocator (not used for static list) */
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size, bounded to avoid huge allocations */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size < 256);

    /* nondet list length, bounded */
    size_t length = nondet_size_t();
    __CPROVER_assume(length <= 10);

    /* allocate raw storage for static list */
    uint8_t *raw = malloc(item_size * length);
    __CPROVER_assume(raw != NULL);
    for (size_t i = 0; i < item_size * length; ++i) {
        raw[i] = nondet_uint8_t();
    }

    /* initialise static array list */
    struct aws_array_list list;
    aws_array_list_init_static(&list, raw, length, item_size);

    /* snapshot list state before the call */
    uint8_t *data_before = malloc(item_size * length);
    __CPROVER_assume(data_before != NULL);
    memcpy(data_before, raw, item_size * length);
    size_t length_before = list.length;
    size_t current_before = list.current_size;

    /* allocate output buffer */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    uint8_t *val_before = malloc(item_size);
    __CPROVER_assume(val_before != NULL);
    for (size_t i = 0; i < item_size; ++i) {
        val[i] = nondet_uint8_t();
        val_before[i] = val[i];
    }

    /* nondet index */
    size_t index = nondet_size_t();

    /* call the function under test */
    int ret = aws_array_list_get_at(&list, val, index);

    /* ---- postconditions ---- */

    /* 1. list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* 2. length and capacity must be unchanged */
    assert(list.length == length_before);
    assert(list.current_size == current_before);

    /* 3. underlying data must be unchanged */
    assert(memcmp(list.data, data_before, item_size * length_before) == 0);

    /* 4. return value / error code correctness and output buffer invariants */
    if (index < length_before) {
        /* successful read */
        assert(ret == AWS_OP_SUCCESS);
        assert(memcmp(val,
                      (uint8_t *)list.data + (index * item_size),
                      item_size) == 0);
    } else {
        /* out‑of‑bounds read */
        assert(ret != AWS_OP_SUCCESS);
        /* error code should be AWS_ERROR_INVALID_INDEX */
        assert(aws_last_error() == AWS_ERROR_INVALID_INDEX);
        /* output buffer must be unchanged */
        assert(memcmp(val, val_before, item_size) == 0);
    }

    /* clean up */
    free(data_before);
    free(val_before);
    free(val);
    free(raw);

    return 0;
}
