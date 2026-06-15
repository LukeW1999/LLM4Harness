#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

void aws_array_list_init_static_harness(void) {
    /* 1. Declare the list structure */
    struct aws_array_list list;

    /* 2. Non‑deterministic inputs for the static array */
    size_t item_count = nondet_size_t();
    size_t item_size  = nondet_size_t();

    /* 3. Preconditions required by the implementation */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    /* Prevent size_t overflow in the multiplication */
    __CPROVER_assume(item_count <= SIZE_MAX / item_size);
    size_t current_size = item_count * item_size;

    /* 4. Allocate the raw backing array */
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* 5. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 6. Post‑conditions that must hold after successful initialization */
    assert(list.alloc == NULL);                     /* allocator is cleared */
    assert(list.item_size == item_size);            /* item size is stored */
    assert(list.length == 0);                       /* list starts empty */
    assert(list.data == raw_array);                 /* data points to provided array */
    assert(list.current_size == current_size);      /* total byte size matches allocation */

    /* 7. The list must satisfy its validity predicate */
    assert(aws_array_list_is_valid(&list));
}
