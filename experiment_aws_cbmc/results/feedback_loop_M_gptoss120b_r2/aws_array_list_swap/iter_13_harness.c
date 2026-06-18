#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

extern uint64_t nondet_uint64_t(void);

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);
    list.alloc = allocator;

    /* Choose reasonable bounds for the list */
    size_t max_items = (size_t)nondet_uint64_t();
    __CPROVER_assume(max_items > 0);
    __CPROVER_assume(max_items <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_sz = (size_t)nondet_uint64_t();
    __CPROVER_assume(item_sz > 0);
    __CPROVER_assume(item_sz <= MAX_ITEM_SIZE);

    /* Initialize the list with the chosen parameters */
    aws_array_list_init(&list, allocator, max_items, item_sz);

    /* Set a nondeterministic non‑zero length that fits within the allocated capacity */
    list.length = (size_t)nondet_uint64_t();
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(list.length <= max_items);

    /* Preserve the original state */
    struct aws_array_list old = list;
    size_t total_bytes = list.length * list.item_size;
    uint8_t old_data[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    if (total_bytes > 0) {
        uint8_t *data_ptr = (uint8_t *)list.data;
        for (size_t i = 0; i < total_bytes; ++i) {
            old_data[i] = data_ptr[i];
        }
    }

    /* Choose two distinct valid indices */
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    int rv = aws_array_list_swap(&list, a, b);
    __CPROVER_assume(rv == AWS_OP_SUCCESS);

    /* Verify structural fields are unchanged */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* Verify the swap effect on the underlying buffer */
    if (total_bytes > 0) {
        uint8_t *data_ptr = (uint8_t *)list.data;
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

    assert(aws_array_list_is_valid(&list));

    aws_array_list_clean_up(&list);
}
