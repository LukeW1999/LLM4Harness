/* Contract:
   Preconditions:
     - list != NULL
     - list->alloc != NULL
     - list->item_size > 0
     - list->current_size == capacity * list->item_size, where capacity > 0
     - list->length <= capacity
     - list->data points to a buffer of size list->current_size
   Postconditions (validity):
     - aws_array_list_is_valid(list) holds after the call
   Postconditions (length):
     - if n >= old_length then list->length == 0
     - else list->length == old_length - n
   Postconditions (frame):
     - Memory outside the list's data buffer is unchanged
     - For the remaining elements, the bytes are exactly the original bytes shifted forward by n * item_size
     - Bytes beyond the new length (up to old length) may be overwritten with DEBUG_FILL in debug builds, but their values are not required to be preserved
*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet setup for item size, capacity, and length */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size < 1024);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity < 256);

    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);

    /* allocate the backing buffer */
    list.alloc = alloc;
    list.item_size = item_size;
    list.length = length;
    list.current_size = capacity * item_size;
    list.data = malloc(list.current_size);
    __CPROVER_assume(list.data != NULL);

    /* initialize the buffer with nondet data */
    uint8_t *buf = (uint8_t *)list.data;
    for (size_t i = 0; i < list.current_size; ++i) {
        buf[i] = nondet_uint8_t();
    }

    /* remember the original state for later comparison */
    uint8_t *old_buf = malloc(list.current_size);
    __CPROVER_assume(old_buf != NULL);
    memcpy(old_buf, buf, list.current_size);

    /* assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet n */
    size_t n = nondet_size_t();

    /* restrict n to avoid overflow and out‑of‑bounds accesses */
    __CPROVER_assume(n <= length);
    __CPROVER_assume(n * item_size <= list.current_size);
    __CPROVER_assume(n <= list.current_size / item_size); /* prevent overflow */

    /* call the function under test */
    aws_array_list_pop_front_n(&list, n);

    /* refresh pointer in case the implementation reallocates */
    buf = (uint8_t *)list.data;

    /* postcondition: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* postcondition: length updates correctly */
    if (n >= length) {
        assert(list.length == 0);
    } else {
        size_t expected_len = length - n;
        assert(list.length == expected_len);

        size_t remaining_bytes = expected_len * item_size;
        /* the remaining bytes must be the original bytes shifted forward */
        assert(memcmp(buf,
                      old_buf + n * item_size,
                      remaining_bytes) == 0);
    }

    free(old_buf);
    free(list.data);
}
