#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;

    /* Nondeterministic choice: dynamic list or static list */
    bool is_dynamic;

    if (is_dynamic) {
        /* Dynamic list: alloc is set, data may or may not be allocated */
        struct aws_allocator *alloc = aws_default_allocator();

        size_t item_size;
        size_t initial_item_allocation;

        /* Bound sizes to keep verification tractable */
        __CPROVER_assume(item_size > 0 && item_size <= 128);
        __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= 16);

        /* Check for overflow: initial_item_allocation * item_size must not overflow */
        size_t total_size;
        __CPROVER_assume(!__builtin_mul_overflow(initial_item_allocation, item_size, &total_size));

        int rc = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

        if (rc == AWS_OP_SUCCESS) {
            /* List is valid after successful init */
            assert(aws_array_list_is_valid(&list));
            assert(list.alloc == alloc);
            assert(list.item_size == item_size);
        } else {
            /* Init failed: data may be NULL, set up a minimal valid state */
            list.alloc = alloc;
            list.data = NULL;
            list.current_size = 0;
            list.length = 0;
            list.item_size = item_size;
        }
    } else {
        /* Static list: alloc is NULL, data points to a raw array */
        size_t item_size;
        size_t item_count;

        __CPROVER_assume(item_size > 0 && item_size <= 128);
        __CPROVER_assume(item_count > 0 && item_count <= 16);

        /* Check for overflow */
        size_t total_size;
        __CPROVER_assume(!__builtin_mul_overflow(item_count, item_size, &total_size));

        void *raw_array = malloc(total_size);
        __CPROVER_assume(raw_array != NULL);

        aws_array_list_init_static(&list, raw_array, item_count, item_size);

        assert(aws_array_list_is_valid(&list));
        assert(list.alloc == NULL);
    }

    /* Precondition: list must be valid before clean_up */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Call the function under verification */
    aws_array_list_clean_up(&list);

    /* Postconditions: all fields zeroed */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* Postconditions via API */
    assert(aws_array_list_length(&list) == 0);
    assert(aws_array_list_capacity(&list) == 0);
}
