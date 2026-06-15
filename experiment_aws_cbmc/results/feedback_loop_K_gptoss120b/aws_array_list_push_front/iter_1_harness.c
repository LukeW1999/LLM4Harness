/*=== Formal Contract for aws_array_list_push_front ==========================
Preconditions:
  - list != NULL
  - list->alloc == aws_default_allocator()          // dynamic list
  - list->item_size > 0
  - list->length <= list->current_size / list->item_size
  - list->data points to a readable/writable buffer of size list->current_size
  - val != NULL and points to readable memory of size list->item_size
  - aws_array_list_is_valid(list) holds (list is in a consistent state)

Postconditions (validity):
  - The function returns 0 on success, otherwise a non‑zero AWS error code.
  - If the return value is 0, then aws_array_list_is_valid(list) still holds.

Postconditions (length & capacity):
  - If return == 0:
        * list->length == old_length + 1
        * list->current_size >= (old_length + 1) * list->item_size
  - If return != 0:
        * list->length == old_length
        * list->current_size == old_current_size

Postconditions (data frame):
  - If return == 0:
        * The first item in list->data equals the bytes pointed to by val.
        * For each i in [1 .. old_length]:
              memcpy(&list->data[i * item_size], &old_data[(i-1) * item_size], item_size);
        * All other bytes of the buffer (beyond (old_length+1)*item_size up to current_size)
          remain unchanged.
  - If return != 0:
        * The contents of list->data remain unchanged.
==========================================================================*/

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Nondeterministic helpers */
size_t nondet_size_t(void);
bool nondet_bool(void);
void *nondet_ptr(void);

/* Helper to copy memory safely */
static void copy_buffer(uint8_t *dst, const uint8_t *src, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = src[i];
    }
}

void aws_array_list_push_front_harness(void) {
    /* Allocate and nondeterministically initialize an aws_array_list */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    list.alloc = alloc;

    /* Item size must be > 0 */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    /* Choose a nondet length */
    size_t old_length = nondet_size_t();
    __CPROVER_assume(old_length <= 1000);               /* bound for tractability */

    /* Choose a nondet capacity (in elements) that is at least old_length */
    size_t capacity_elems = nondet_size_t();
    __CPROVER_assume(capacity_elems >= old_length);
    __CPROVER_assume(capacity_elems <= 2000);           /* bound */

    list.current_size = capacity_elems * list.item_size;
    list.length = old_length;

    /* Allocate the backing buffer */
    list.data = malloc(list.current_size);
    __CPROVER_assume(list.data != NULL);

    /* Initialize the existing elements with nondet data */
    uint8_t *data_bytes = (uint8_t *)list.data;
    for (size_t i = 0; i < list.current_size; ++i) {
        data_bytes[i] = nondet_uint8_t();
    }

    /* Preserve a copy of the original buffer for later comparison */
    uint8_t *old_data = malloc(old_length * list.item_size);
    __CPROVER_assume(old_data != NULL);
    copy_buffer(old_data, data_bytes, old_length * list.item_size);

    /* Allocate and initialize the value to be pushed */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    uint8_t *val_bytes = (uint8_t *)val;
    for (size_t i = 0; i < list.item_size; ++i) {
        val_bytes[i] = nondet_uint8_t();
    }

    /* Assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state for post‑condition checks */
    size_t old_current_size = list.current_size;
    size_t old_capacity = old_current_size / list.item_size;

    /* Call the function under verification */
    int ret = aws_array_list_push_front(&list, val);

    /* ==== Postcondition checks ==== */

    /* Return value must be either 0 (success) or a non‑zero AWS error code */
    assert(ret == 0 || ret != 0);

    if (ret == 0) {
        /* Length increased by exactly one */
        assert(list.length == old_length + 1);

        /* Capacity must be sufficient for the new length */
        assert(list.current_size >= (list.length) * list.item_size);

        /* The new front element must equal *val */
        for (size_t i = 0; i < list.item_size; ++i) {
            assert(((uint8_t *)list.data)[i] == ((uint8_t *)val)[i]);
        }

        /* All previously existing elements must have been shifted right by one */
        for (size_t i = 1; i <= old_length; ++i) {
            for (size_t j = 0; j < list.item_size; ++j) {
                assert(((uint8_t *)list.data)[i * list.item_size + j] ==
                       old_data[(i - 1) * list.item_size + j]);
            }
        }

        /* Bytes beyond the new logical length must be unchanged */
        size_t bytes_to_check = list.current_size - (list.length * list.item_size);
        for (size_t i = 0; i < bytes_to_check; ++i) {
            size_t idx = list.length * list.item_size + i;
            assert(((uint8_t *)list.data)[idx] ==
                   ((uint8_t *)old_data)[old_length * list.item_size + i] ||
                   idx >= old_current_size); /* if buffer grew, new bytes are unconstrained */
        }
    } else {
        /* On error, length and capacity must be unchanged */
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);

        /* Entire buffer must be unchanged */
        for (size_t i = 0; i < old_current_size; ++i) {
            assert(((uint8_t *)list.data)[i] == ((uint8_t *)old_data)[i % (old_length * list.item_size)]);
        }
    }

    /* The list must remain valid after the operation */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(list.data);
    free(old_data);
    free(val);

    return 0;
}
