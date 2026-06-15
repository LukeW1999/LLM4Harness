/*=====================================================================
  Contract for aws_array_list_back
  ----------------------------------------------------------------------
  Preconditions:
    - list != NULL
    - list->item_size > 0
    - (list->current_size == 0) || (list->data != NULL)
    - list->length * list->item_size <= list->current_size
    - val != NULL
    - val points to writable memory of size list->item_size
  Postconditions (validity):
    - aws_array_list_is_valid(list) holds after the call
  Postconditions (return):
    - returns AWS_OP_SUCCESS (0) when list->length > 0
    - returns AWS_ERROR_LIST_EMPTY when list->length == 0
  Postconditions (length):
    - list->length is unchanged
  Postconditions (frame):
    - list->alloc, list->current_size, list->item_size, list->data
      are unchanged
    - the contents of list->data are unchanged
    - when list->length > 0, the memory pointed to by val contains a
      copy of the last element in the list
=====================================================================*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

int aws_array_list_back_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic item size > 0, bounded for tractability */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 8);
    list.item_size = item_size;

    /* nondeterministic capacity (number of items that can be stored), bounded */
    size_t capacity;
    __CPROVER_assume(capacity <= 8);
    __CPROVER_assume(capacity <= SIZE_MAX / item_size);
    list.current_size = item_size * capacity;

    /* allocate backing storage if capacity > 0 */
    if (list.current_size > 0) {
        list.data = aws_mem_acquire(alloc, list.current_size);
        __CPROVER_assume(list.data != NULL);
        /* initialize data with nondeterministic bytes */
        uint8_t *bytes = (uint8_t *)list.data;
        for (size_t i = 0; i < list.current_size; ++i) {
            bytes[i] = __CPROVER_nondet_uchar();
        }
    } else {
        list.data = NULL;
    }

    /* nondeterministic length satisfying length * item_size <= current_size */
    size_t length;
    __CPROVER_assume(length <= capacity);
    list.length = length;

    list.alloc = alloc;

    /* allocate writable buffer for output */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Preserve a copy of the original list state for frame checks */
    struct aws_array_list list_before = list;
    void *data_before = NULL;
    if (list.data != NULL) {
        data_before = malloc(list.current_size);
        __CPROVER_assume(data_before != NULL);
        memcpy(data_before, list.data, list.current_size);
    }

    int ret = aws_array_list_back(&list, val);

    /* Postcondition: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition: length unchanged */
    assert(list.length == list_before.length);

    /* Postcondition: allocator, item_size, current_size, data pointer unchanged */
    assert(list.alloc == list_before.alloc);
    assert(list.item_size == list_before.item_size);
    assert(list.current_size == list_before.current_size);
    assert(list.data == list_before.data);

    /* Postcondition: data buffer unchanged */
    if (data_before != NULL) {
        assert(memcmp(data_before, list.data, list.current_size) == 0);
        free(data_before);
    }

    if (list_before.length > 0) {
        /* Should have succeeded */
        assert(ret == AWS_OP_SUCCESS);
        /* Verify that val contains the last element */
        size_t last_offset = list_before.item_size * (list_before.length - 1);
        assert(memcmp(val,
                      (uint8_t *)list_before.data + last_offset,
                      list_before.item_size) == 0);
    } else {
        /* Should have failed with list empty error */
        assert(ret == AWS_ERROR_LIST_EMPTY);
    }

    free(val);
    if (list.data != NULL) {
        aws_mem_release(alloc, list.data);
    }
    return 0;
}
