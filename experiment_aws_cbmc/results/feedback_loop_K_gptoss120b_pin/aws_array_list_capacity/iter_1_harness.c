/*=== Contract for aws_array_list_capacity ===
Preconditions:
  - __CPROVER_assume(list != NULL);
  - __CPROVER_assume(list->item_size != 0);
  - __CPROVER_assume(aws_array_list_is_valid(list));

Postconditions (validity):
  - The function returns a size_t value equal to list->current_size / list->item_size.
  - After the call, aws_array_list_is_valid(list) must still hold.

Postconditions (length):
  - Returned capacity >= list->length.
  - Returned capacity * list->item_size == list->current_size.

Postconditions (frame):
  - No memory outside the aws_array_list structure and its data buffer is modified.
  - The contents of the data buffer remain unchanged.
===*/

#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_capacity_harness(void) {
    /* Allocate and initialize a list */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size, must be > 0 */
    size_t item_size;
    __CPROVER_assume(item_size > 0);

    /* nondet initial allocation count (could be zero) */
    size_t init_count;
    __CPROVER_assume(init_count <= 10); /* keep it small for proof */

    /* Initialize the list dynamically */
    int init_res = aws_array_list_init_dynamic(&list, alloc, init_count, item_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);

    /* Ensure length is within capacity */
    size_t capacity = list.current_size / list.item_size;
    __CPROVER_assume(list.length <= capacity);

    /* Make a copy of the list structure and its data buffer for later comparison */
    struct aws_array_list old_list = list;
    uint8_t *old_data = NULL;
    if (list.data) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }

    /* Call the function under test */
    size_t ret_capacity = aws_array_list_capacity(&list);

    /* ---- Postcondition checks ---- */

    /* Return value matches definition */
    assert(ret_capacity == list.current_size / list.item_size);

    /* Capacity is at least the current length */
    assert(ret_capacity >= list.length);

    /* Capacity * item_size equals current_size */
    assert(ret_capacity * list.item_size == list.current_size);

    /* List remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Frame condition: list structure fields unchanged */
    assert(list.item_size == old_list.item_size);
    assert(list.current_size == old_list.current_size);
    assert(list.alloc == old_list.alloc);
    assert(list.length == old_list.length);
    assert(list.data == old_list.data);

    /* Frame condition: data buffer unchanged */
    if (list.data) {
        assert(memcmp(list.data, old_data, list.current_size) == 0);
        free(old_data);
    }

    /* Clean up */
    aws_array_list_clean_up(&list);
    return 0;
}
