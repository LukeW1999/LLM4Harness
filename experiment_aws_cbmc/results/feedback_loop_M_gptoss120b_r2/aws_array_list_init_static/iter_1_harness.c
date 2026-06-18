#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_static_harness(void) {
    /* 1. Declare inputs */
    struct aws_array_list list;
    void *raw_array;
    size_t item_count;
    size_t item_size;

    /* 2. Bound nondeterministic inputs */
    __CPROVER_assume(list.alloc == NULL);               /* list is uninitialized stack object */
    __CPROVER_assume(raw_array != NULL);
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Ensure raw_array points to a readable/writable buffer of sufficient size */
    size_t raw_buf_size;
    __CPROVER_assume(raw_buf_size > 0);
    __CPROVER_assume(raw_buf_size <= MAX_BUFFER_SIZE);
    raw_array = malloc(raw_buf_size);
    __CPROVER_assume(raw_array != NULL);

    /* 3. Pre‑condition: multiplication must not overflow */
    size_t expected_current_size = 0;
    bool no_overflow = !aws_mul_size_checked(item_count, item_size, &expected_current_size);
    __CPROVER_assume(no_overflow);

    /* 4. Save old state for fields that should remain unchanged (none in this case) */
    struct aws_array_list old = list;

    /* 5. Call function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 6. Post‑condition assertions */

    /* Changed fields */
    assert(list.alloc == NULL);                         /* allocator is set to NULL for static mode */
    assert(list.data == raw_array);                     /* data points to the provided raw array */
    assert(list.item_size == item_size);                /* item size is stored */
    assert(list.length == 0);                           /* length is reset to zero */
    assert(list.current_size == expected_current_size);/* current_size equals item_count * item_size */

    /* Unchanged fields – none other than those explicitly set above */

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
