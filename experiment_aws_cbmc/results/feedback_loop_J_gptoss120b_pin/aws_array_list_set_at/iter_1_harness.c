#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    /* bound the list (max initial allocation and max item size) */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    /* allocate internal data buffer */
    ensure_array_list_has_allocated_data_member(&list);
    /* set allocator (default) */
    list.alloc = aws_default_allocator();

    /* assume a non‑zero, bounded item size */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* allocate a readable source buffer for val */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* make the memory readable (nondet contents are fine) */
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* nondet index */
    size_t index = nondet_size_t();

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old = list;
    size_t old_length = list.length;
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;

    /* 3. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* length updates if index is beyond current length */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }

        /* the written element must match the source */
        assert_bytes_match((const uint8_t *)list.data + (list.item_size * index),
                           (const uint8_t *)val,
                           list.item_size);
    } else {
        /* on failure the list must remain unchanged */
        assert(list.alloc == old_alloc);
        assert(list.item_size == old_item_size);
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);
    /* current_size may change only in dynamic mode via ensure_capacity;
       however the specification does not guarantee it stays the same,
       so we only assert it unchanged on failure (already done above). */

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* free allocated buffers */
    free(val);
}
