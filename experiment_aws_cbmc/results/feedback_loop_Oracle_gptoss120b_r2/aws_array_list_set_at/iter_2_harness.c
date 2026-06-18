#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_set_at_harness(void) {
    /*--------------------------------------------------------------------
     * Setup: allocate and nondeterministically initialize a list and a
     *        value buffer.
     *--------------------------------------------------------------------*/
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t init_count;
    __CPROVER_assume(init_count <= MAX_INITIAL_ITEM_ALLOCATION);
    size_t item_sz;
    __CPROVER_assume(item_sz > 0 && item_sz <= MAX_ITEM_SIZE);

    aws_array_list_init_dynamic(&list, alloc, init_count, item_sz);
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.data != NULL);

    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_sz));

    /*--------------------------------------------------------------------
     * Choose an index that may cause overflow (and thus failure) or be
     * safe (and thus succeed).
     *--------------------------------------------------------------------*/
    size_t index;
    bool cause_overflow = __CPROVER_nondet_bool();
    size_t max_safe = (SIZE_MAX / item_sz) - 1;
    if (cause_overflow) {
        __CPROVER_assume(index > max_safe);
    } else {
        __CPROVER_assume(index <= max_safe);
    }

    /*--------------------------------------------------------------------
     * Snapshot old state for post‑condition checks.
     *--------------------------------------------------------------------*/
    struct aws_array_list old_list = list;
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data_ptr = list.data;
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t item_size = list.item_size;

    uint8_t *old_contents = NULL;
    if (old_data_ptr && old_length > 0 && item_size > 0) {
        old_contents = malloc(old_length * item_size);
        __CPROVER_assume(old_contents);
        memcpy(old_contents, old_data_ptr, old_length * item_size);
    }

    uint8_t *val_snapshot = malloc(item_size);
    __CPROVER_assume(val_snapshot);
    memcpy(val_snapshot, val, item_size);

    /*--------------------------------------------------------------------
     * Call the function under verification.
     *--------------------------------------------------------------------*/
    int ret = aws_array_list_set_at(&list, val, index);

    /*--------------------------------------------------------------------
     * Post‑condition checks.
     *--------------------------------------------------------------------*/
    assert(aws_array_list_is_valid(&list));
    assert(aws_array_list_capacity(&list) >= old_capacity);

    if (ret == AWS_OP_SUCCESS) {
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }

        assert(memcmp((uint8_t *)list.data + index * item_size,
                      val_snapshot,
                      item_size) == 0);

        for (size_t i = 0; i < old_length; ++i) {
            if (i == index) {
                continue;
            }
            assert(memcmp((uint8_t *)list.data + i * item_size,
                          old_contents + i * item_size,
                          item_size) == 0);
        }
    } else {
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data_ptr);
        if (old_contents) {
            assert(memcmp(list.data,
                          old_contents,
                          old_length * item_size) == 0);
        }
    }

    assert(list.alloc == alloc);

    /*--------------------------------------------------------------------
     * Clean up.
     *--------------------------------------------------------------------*/
    free(val);
    free(old_contents);
    free(val_snapshot);
    aws_array_list_clean_up(&list);
}
