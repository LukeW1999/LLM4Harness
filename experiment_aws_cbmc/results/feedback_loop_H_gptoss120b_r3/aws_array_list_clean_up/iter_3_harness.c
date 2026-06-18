#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

/* Maximum bounds used for the verification */
#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 64
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 64
#endif

void aws_array_list_clean_up_harness(void) {
    /* 1. Obtain the default allocator (must be non‑NULL and have a valid release) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Declare the array list */
    struct aws_array_list list;

    /* 3. Choose nondeterministic but bounded item size and initial capacity */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_capacity = nondet_size_t();
    __CPROVER_assume(initial_capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 4. Initialise the list dynamically using the default allocator */
    if (aws_array_list_init_dynamic(&list, allocator, item_size, initial_capacity) != AWS_OP_SUCCESS) {
        /* If initialisation fails, there is nothing to clean up – exit the harness */
        return;
    }

    /* 5. Assume the list satisfies the boundedness predicate required by the proof */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* 6. Preserve the old state (optional, not used in this harness) */
    struct aws_array_list old = list;

    /* 7. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 8. Post‑conditions: all fields must be cleared */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* 9. The list must remain valid after clean‑up */
    assert(aws_array_list_is_valid(&list));
}
