#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_array_list_swap_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    /* 2. Use a concrete allocator to guarantee successful temporary allocation */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 3. Save a copy of the whole list state before the call */
    struct aws_array_list old = list;

    /* Save a copy of the raw data buffer for later comparison */
    size_t total_bytes = list.length * list.item_size;
    uint8_t *old_data = NULL;
    if (total_bytes > 0) {
        old_data = malloc(total_bytes);
        __CPROVER_assume(old_data != NULL);
        uint8_t *data_ptr = (uint8_t *)list.data;
        for (size_t i = 0; i < total_bytes; ++i) {
            old_data[i] = data_ptr[i];
        }
    }

    /* 4. Non‑deterministic indices, bounded by list length */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 5. Call the function under test and assume it succeeds */
    int rv = aws_array_list_swap(&list, a, b);
    __CPROVER_assume(rv == AWS_OP_SUCCESS);

    /* 6. Post‑condition: fields that must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* 7. Post‑condition: data buffer contents */
    if (total_bytes > 0) {
        uint8_t *data_ptr = (uint8_t *)list.data;
        size_t item_sz = list.item_size;

        for (size_t idx = 0; idx < list.length; ++idx) {
            for (size_t i = 0; i < item_sz; ++i) {
                size_t offset = idx * item_sz + i;
                if (idx == a) {
                    assert(data_ptr[offset] == old_data[b * item_sz + i]);
                } else if (idx == b) {
                    assert(data_ptr[offset] == old_data[a * item_sz + i]);
                } else {
                    assert(data_ptr[offset] == old_data[offset]);
                }
            }
        }
    }

    /* 8. Validity invariant must still hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(old_data);
}
