#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "make_common_data_structures.h"

void aws_array_list_push_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic initialization of list fields */
    list.alloc = alloc;
    list.item_size = __CPROVER_nondet_size_t();
    list.length = __CPROVER_nondet_size_t();
    list.current_size = __CPROVER_nondet_size_t();
    list.data = __CPROVER_nondet_pointer();

    /* ensure list is in a valid state */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondeterministic value to push */
    const void *val = __CPROVER_nondet_pointer();
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* snapshot pre‑call state */
    size_t orig_len = list.length;
    void *orig_data = list.data;
    size_t orig_item_size = list.item_size;
    size_t orig_current_size = list.current_size;
    uint8_t *orig_bytes = NULL;
    if (orig_data != NULL && orig_current_size > 0) {
        orig_bytes = malloc(orig_current_size);
        __CPROVER_assume(orig_bytes != NULL);
        memcpy(orig_bytes, orig_data, orig_current_size);
    }

    /* call the function under verification */
    int result = aws_array_list_push_front(&list, val);

    /* post‑condition assertions */
    /* result must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* allocator and item size must remain unchanged */
    assert(list.alloc == alloc);
    assert(list.item_size == orig_item_size);

    if (result == AWS_OP_ERR) {
        /* on error the list state must be unchanged */
        assert(list.length == orig_len);
        assert(list.current_size == orig_current_size);
        assert(list.data == orig_data);
        if (orig_data != NULL && orig_current_size > 0) {
            assert(memcmp(list.data, orig_bytes, orig_current_size) == 0);
        }
    } else {
        /* on success the length is increased by one */
        assert(list.length == orig_len + 1);

        /* capacity must be sufficient for the new length */
        assert(list.current_size >= list.length * list.item_size);

        /* data pointer must be non‑null and readable */
        assert(list.data != NULL);
        assert(AWS_MEM_IS_READABLE(list.data, list.length * list.item_size));

        /* the first element must equal the pushed value */
        assert(memcmp(list.data, val, list.item_size) == 0);

        /* the original elements must be shifted right by one slot */
        if (orig_len > 0) {
            assert(memcmp((uint8_t *)list.data + list.item_size,
                          (uint8_t *)orig_data,
                          orig_len * list.item_size) == 0);
        }
    }
}
