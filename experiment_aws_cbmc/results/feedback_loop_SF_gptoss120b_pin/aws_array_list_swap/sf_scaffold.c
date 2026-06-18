#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;
    /* allocator */
    list.alloc = aws_default_allocator();

    /* item size */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    /* capacity (current_size) */
    list.current_size = nondet_size_t();
    __CPROVER_assume(list.current_size <= SIZE_MAX / list.item_size);

    /* allocate backing storage */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size * list.item_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* length */
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= list.current_size);

    /* indices to swap */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* snapshot of relevant state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;

    /* call the function under verification */
    aws_array_list_swap(&list, a, b);

    /* ASSERT_POSTCONDITIONS_HERE */

    /* clean up */
    free(list.data);
}
