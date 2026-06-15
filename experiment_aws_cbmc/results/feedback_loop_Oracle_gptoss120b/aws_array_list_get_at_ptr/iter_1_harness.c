#include <aws/common/common.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_get_at_ptr_harness(void) {
    /*--------------------------------------------------------------------*/
    /* Setup: create a (potentially) dynamic array list with bounded size */
    /*--------------------------------------------------------------------*/
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet initial allocation and item size within the given bounds */
    size_t init_alloc = nondet_uint();
    __CPROVER_assume(init_alloc <= MAX_INITIAL_ITEM_ALLOCATION);
    size_t item_sz = nondet_uint();
    __CPROVER_assume(item_sz <= MAX_ITEM_SIZE);
    __CPROVER_assume(item_sz > 0);               /* item size must be non‑zero */

    /* initialise the list (dynamic mode) */
    int init_rc = aws_array_list_init_dynamic(&list, alloc, init_alloc, item_sz);
    __CPROVER_assume(init_rc == AWS_OP_SUCCESS);

    /*--------------------------------------------------------------------*/
    /* Additional structural assumptions required by the harness            */
    /*--------------------------------------------------------------------*/
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(aws_array_list_is_valid(&list));   /* duplicated as per spec */

    /*--------------------------------------------------------------------*/
    /* Prepare the output pointer argument                                 */
    /*--------------------------------------------------------------------*/
    void *out_ptr = NULL;
    void **val = &out_ptr;
    __CPROVER_assume(val);                     /* val must be non‑NULL */

    /*--------------------------------------------------------------------*/
    /* Choose a nondet index                                                */
    /*--------------------------------------------------------------------*/
    size_t index = nondet_uint();

    /*--------------------------------------------------------------------*/
    /* Preserve pre‑state for frame condition checks                        */
    /*--------------------------------------------------------------------*/
    struct aws_array_list old_list = list;
    void *old_out = out_ptr;

    /* copy the underlying data buffer (if any) for later comparison */
    uint8_t *old_data_copy = NULL;
    if (list.data != NULL && list.current_size > 0) {
        old_data_copy = malloc(list.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, list.data, list.current_size);
    }

    /*--------------------------------------------------------------------*/
    /* Call the function under verification                                 */
    /*--------------------------------------------------------------------*/
    int rc = aws_array_list_get_at_ptr(&list, val, index);

    /*--------------------------------------------------------------------*/
    /* Post‑condition 1: return value / error code correctness            */
    /*--------------------------------------------------------------------*/
    if (aws_array_list_length(&list) > index) {
        assert(rc == AWS_OP_SUCCESS);
    } else {
        assert(rc == aws_raise_error(AWS_ERROR_INVALID_INDEX));
    }

    /*--------------------------------------------------------------------*/
    /* Post‑condition 2: output pointer correctness                        */
    /*--------------------------------------------------------------------*/
    if (rc == AWS_OP_SUCCESS) {
        /* *val must point to the element at the given index */
        void *expected = (void *)((uint8_t *)list.data + (list.item_size * index));
        assert(*val == expected);
    } else {
        /* on error the output pointer must be unchanged */
        assert(*val == old_out);
    }

    /*--------------------------------------------------------------------*/
    /* Post‑condition 3: frame conditions (list unchanged)                  */
    /*--------------------------------------------------------------------*/
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /*--------------------------------------------------------------------*/
    /* Post‑condition 4: underlying data buffer not modified               */
    /*--------------------------------------------------------------------*/
    if (old_data_copy != NULL) {
        assert(memcmp(old_data_copy, list.data, list.current_size) == 0);
        free(old_data_copy);
    }

    /* clean‑up */
    aws_array_list_clean_up(&list);
    return 0;
}
