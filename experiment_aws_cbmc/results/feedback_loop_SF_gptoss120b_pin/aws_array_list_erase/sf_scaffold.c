#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <aws/common/assert.h>
#include "make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic item size and capacity */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    size_t max_items = nondet_size_t();
    __CPROVER_assume(max_items > 0);

    /* initialize list */
    struct aws_array_list list;
    list.alloc = alloc;
    list.item_size = item_size;
    list.current_size = max_items * item_size;
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= max_items);

    list.data = malloc(list.current_size);
    __CPROVER_assume(list.data != NULL);

    /* ensure the list is in a valid state */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondeterministic index */
    size_t index = nondet_size_t();

    /* pre‑call snapshot */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;

    /* call the function under verification */
    int result = aws_array_list_erase(&list, index);

    /* ASSERT_POSTCONDITIONS_HERE */
}
