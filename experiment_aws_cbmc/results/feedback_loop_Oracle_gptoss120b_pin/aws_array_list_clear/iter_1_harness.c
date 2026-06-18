#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministically decide whether to start with a zeroed list or a valid one */
    bool make_zero = __CPROVER_nondet_bool();
    if (make_zero) {
        memset(&list, 0, sizeof(list));
    } else {
        size_t item_size;
        size_t init_items;

        /* reasonable bounds to keep the model tractable */
        __CPROVER_assume(item_size > 0 && item_size <= 64);
        __CPROVER_assume(init_items <= 8);

        /* ensure multiplication does not overflow */
        size_t mul;
        __CPROVER_assume(!aws_mul_size_checked(init_items, item_size, &mul));

        int rc = aws_array_list_init_dynamic(&list, alloc, init_items, item_size);
        __CPROVER_assume(rc == AWS_OP_SUCCESS);
    }

    /* capture a snapshot of the data buffer before the call */
    uint8_t *old_data = NULL;
    size_t old_current_size = 0;
    if (list.data) {
        old_current_size = list.current_size;
        old_data = malloc(old_current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_current_size);
    }
    size_t old_length = list.length;

    /* invoke the function under test */
    aws_array_list_clear(&list);

    /* postcondition: either the list is zeroed or it remains a valid list */
    bool is_zeroed = (list.alloc == NULL &&
                      list.data == NULL &&
                      list.item_size == 0 &&
                      list.length == 0 &&
                      list.current_size == 0);
    assert(is_zeroed || aws_array_list_is_valid(&list));

    /* length must be zero after clear */
    assert(list.length == 0);

    /* if a data buffer exists, its contents must be unchanged (aside from possible debug fills) */
    if (list.data) {
        assert(memcmp(old_data, list.data, old_current_size) == 0);
    }

    /* clean up any allocated resources */
    if (list.data) {
        aws_array_list_clean_up(&list);
    }
    free(old_data);

    return 0;
}
