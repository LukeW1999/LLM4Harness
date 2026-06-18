#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
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
    /* Initialise the common library so that the default allocator is valid */
    struct aws_allocator *default_alloc = aws_default_allocator();
    __CPROVER_assume(default_alloc != NULL);
    aws_common_library_init(default_alloc);

    /* Declare the array list */
    struct aws_array_list list;

    /* Choose nondeterministic but bounded item size and initial capacity */
    size_t item_size = nondet_uint();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_capacity = nondet_uint();
    __CPROVER_assume(initial_capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Initialise the list dynamically using the default allocator */
    if (aws_array_list_init_dynamic(&list, default_alloc, item_size, initial_capacity) != AWS_OP_SUCCESS) {
        return;
    }

    /* Call the function under test */
    aws_array_list_clean_up(&list);

    /* Post‑conditions: all fields must be cleared */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* The list must remain valid after clean‑up (zeroed list is considered valid) */
    assert(aws_array_list_is_valid(&list));
}
