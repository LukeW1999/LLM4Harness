/*=== Contract ===
Preconditions:
  - list != NULL
  - list->alloc != NULL
  - list->item_size > 0
  - list->current_size == list->item_size * capacity   (capacity is the number of items that can be stored)
  - list->length <= capacity
  - if list->length > 0 then list->data != NULL
  - n is any size_t value
Postconditions (validity):
  - list remains a valid aws_array_list (aws_array_list_is_valid returns true)
Postconditions (length):
  - let old_len = __CPROVER_old(list->length);
    if (n >= old_len) then list->length == 0;
    else list->length == old_len - n;
Postconditions (frame):
  - list->alloc is unchanged
  - list->item_size is unchanged
  - list->current_size is unchanged
  - list->data pointer value is unchanged
  - memory outside the range [list->data, list->data + list->current_size) is unchanged
=== End Contract ===*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_pop_front_n_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size, must be > 0 */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    /* nondet capacity */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= SIZE_MAX / item_size); /* avoid overflow */

    /* set up list fields */
    list.alloc = alloc;
    list.item_size = item_size;
    list.current_size = item_size * capacity;

    /* allocate backing buffer */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* nondet length, must be <= capacity */
    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /* ensure list is valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet n */
    size_t n = nondet_size_t();

    /* Save old state for postcondition checks */
    size_t old_len = list.length;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data_ptr = list.data;

    /* Call the function under test */
    aws_array_list_pop_front_n(&list, n);

    /* Postcondition: validity */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition: length */
    if (n >= old_len) {
        assert(list.length == 0);
    } else {
        assert(list.length == old_len - n);
    }

    /* Frame conditions */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);
    assert(list.current_size == old_current_size);
    assert(list.data == old_data_ptr);

    return 0;
}
