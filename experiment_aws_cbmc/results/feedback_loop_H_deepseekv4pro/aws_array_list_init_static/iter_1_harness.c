#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_static_harness() {
    struct aws_array_list list;
    size_t item_count;
    size_t item_size;
    void *raw_array;

    /* assign nondeterministic values */
    item_count = nondet_size_t();
    item_size  = nondet_size_t();

    /* enforce all FATAL precondition conditions */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= SIZE_MAX / item_size); /* multiplication must not overflow */

    /* allocate a raw buffer of the required size */
    raw_array = malloc(item_count * item_size);
    __CPROVER_assume(raw_array != NULL); /* allocation cannot fail */

    /* call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* assert all explicit field assignments */
    assert(list.alloc       == NULL);
    assert(list.data        == raw_array);
    assert(list.item_size   == item_size);
    assert(list.length      == 0UL);
    assert(list.current_size == item_count * item_size);

    /* assert the overall validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* release the allocated buffer */
    free(raw_array);
}
