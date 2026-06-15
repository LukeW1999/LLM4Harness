#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_init_static_harness() {
    /* 1. Declare and bound the list structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* 2. Nondeterministic inputs respecting preconditions */
    size_t item_count = nondet_size_t();
    size_t item_size  = nondet_size_t();
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size  > 0);

    /* Ensure multiplication does not overflow */
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    /* Allocate a raw array that satisfies the precondition raw_array != NULL */
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(raw_array, current_size));

    /* 3. Save old state (no fields are required to stay unchanged) */
    struct aws_array_list old = list;

    /* 4. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 5. Assert postconditions (all fields are defined by the function) */
    assert(list.alloc == NULL);                     /* allocator is set to NULL */
    assert(list.item_size == item_size);            /* item size stored */
    assert(list.length == 0);                       /* length reset to zero */
    assert(list.data == raw_array);                 /* data pointer set */
    assert(list.current_size == current_size);      /* total byte size stored */

    /* 6. No other fields exist; nothing else to assert unchanged */

    /* 7. Validity invariant must hold after initialization */
    assert(aws_array_list_is_valid(&list));
}
