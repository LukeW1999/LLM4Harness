#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

void aws_array_list_init_static_harness(void) {
    /* 1. Declare the list and nondeterministic parameters */
    struct aws_array_list list;

    size_t item_count = nondet_size_t();
    size_t item_size  = nondet_size_t();

    /* 2. Assume preconditions from the Doxygen comment and implementation */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    /* prevent size_t overflow in multiplication */
    __CPROVER_assume(item_count <= SIZE_MAX / item_size);
    size_t current_size = item_count * item_size;

    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* 3. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 4. Assert post‑conditions that must hold after a successful call */
    assert(list.alloc == NULL);                     /* allocator is set to NULL */
    assert(list.item_size == item_size);            /* item size stored */
    assert(list.length == 0);                       /* list starts empty */
    assert(list.data == raw_array);                 /* data points to provided array */
    assert(list.current_size == current_size);      /* capacity equals item_count * item_size */

    /* 5. No fields are required to stay unchanged because the function zero‑initialises the struct */

    /* 6. The list must satisfy its validity predicate */
    assert(aws_array_list_is_valid(&list));
}
