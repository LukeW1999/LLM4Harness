#include <aws/common/array_list.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_array_list_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic item size and capacity, bounded to keep CBMC tractable */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 64);
    size_t max_items = nondet_size_t();
    __CPROVER_assume(max_items > 0);
    __CPROVER_assume(max_items <= 64);

    /* allocate and initialize the list */
    ensure_array_list_is_allocated(&list, alloc, item_size, max_items);
    __CPROVER_assume(list.length <= max_items);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    if (list.length > 0) {
        __CPROVER_assume(list.data != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(list.data, item_size));
    }

    /* allocate output buffer */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, item_size));
    __CPROVER_assume(val != list.data); /* avoid overlap */

    /* snapshot pre‑state */
    size_t old_length = list.length;
    void *old_data = list.data;
    uint8_t *old_front = NULL;
    if (old_length > 0) {
        old_front = malloc(item_size);
        __CPROVER_assume(old_front != NULL);
        memcpy(old_front, list.data, item_size);
    }

    /* call the function under verification */
    int result = aws_array_list_front(&list, val);

    /* post‑conditions */
    if (old_length > 0) {
        assert(result == AWS_OP_SUCCESS);
        for (size_t i = 0; i < item_size; ++i) {
            assert(((uint8_t *)val)[i] == old_front[i]);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(aws_last_error() == AWS_ERROR_LIST_EMPTY);
    }

    /* structural invariants */
    assert(list.length == old_length);
    assert(list.item_size == item_size);
    assert(list.data == old_data);
    assert(list.alloc == alloc);
}
