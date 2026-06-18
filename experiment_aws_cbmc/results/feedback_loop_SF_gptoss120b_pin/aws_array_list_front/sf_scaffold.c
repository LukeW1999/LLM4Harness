#include <aws/common/array_list.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic item size and capacity */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    size_t max_items = nondet_size_t();
    __CPROVER_assume(max_items > 0);

    /* allocate and initialize the list */
    ensure_array_list_is_allocated(&list, alloc, item_size, max_items);

    /* allocate output buffer */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, item_size));

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

    /* ASSERT_POSTCONDITIONS_HERE */
}
