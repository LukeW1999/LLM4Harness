#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_clean_up_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();
    struct aws_array_list list;

    size_t item_size = nondet_size_t();
    size_t initial_capacity = nondet_size_t();

    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Initialize the list */
    aws_array_list_init(&list, alloc, initial_capacity, item_size);

    /* Ensure the list is in a valid state before cleanup */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    int rc = aws_array_list_clean_up(&list);
    assert(rc == AWS_OP_SUCCESS);

    /* Post‑conditions of clean_up */
    assert(list.data == NULL);
    assert(list.length == 0);
    assert(list.current_size == 0);
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
}
