#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Zero‑initialize the list */
    memset(&list, 0, sizeof(list));
    list.alloc = allocator; /* dynamic list */

    /* Nondeterministic item size, bounded */
    size_t item_size = __CPROVER_nondet_ulong();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 64);
    list.item_size = item_size;

    /* Nondeterministic length, bounded */
    size_t length = __CPROVER_nondet_ulong();
    __CPROVER_assume(length <= 8);
    list.length = length;

    /* Compute required size = length * item_size, avoid overflow */
    size_t required_size;
    __CPROVER_assume(!aws_mul_size_checked(length, item_size, &required_size));

    /* Choose current_size >= required_size, also bounded */
    size_t extra = __CPROVER_nondet_ulong();
    __CPROVER_assume(extra <= 32);
    size_t current_size = required_size + extra;
    list.current_size = current_size;

    /* Allocate data buffer if needed */
    if (current_size > 0) {
        list.data = malloc(current_size);
        __CPROVER_assume(list.data != NULL);
        for (size_t i = 0; i < current_size; ++i) {
            ((unsigned char *)list.data)[i] = __CPROVER_nondet_uchar();
        }
    } else {
        list.data = NULL;
    }

    /* Allocate val buffer (must be writable) */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Ensure preconditions */
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* Capture pre‑state for later comparison */
    struct aws_array_list list_pre = list;
    unsigned char *data_pre = NULL;
    if (list.data) {
        data_pre = malloc(list.current_size);
        __CPROVER_assume(data_pre != NULL);
        memcpy(data_pre, list.data, list.current_size);
    }

    /* Call the function under test */
    int ret = aws_array_list_front(&list, val);

    /* Postcondition: return value must be either success or empty error */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_ERROR_LIST_EMPTY);

    if (ret == AWS_OP_SUCCESS) {
        assert(list_pre.length > 0);
        assert(memcmp(val, list_pre.data, list_pre.item_size) == 0);
    } else {
        assert(list_pre.length == 0);
    }

    /* Frame conditions: list structure fields unchanged */
    assert(list.alloc == list_pre.alloc);
    assert(list.item_size == list_pre.item_size);
    assert(list.current_size == list_pre.current_size);
    assert(list.length == list_pre.length);
    assert(list.data == list_pre.data);

    /* Frame condition: data buffer unchanged */
    if (list.data && data_pre) {
        assert(memcmp(list.data, data_pre, list.current_size) == 0);
    }

    /* Clean up */
    free(list.data);
    free(val);
    free(data_pre);
}
