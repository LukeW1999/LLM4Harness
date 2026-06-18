#include <aws/common/array_list.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* 2. Ensure the list has allocated data and a valid allocator */
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = aws_default_allocator();               /* use default allocator */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 3. Assume non‑empty list so that indices can be chosen */
    __CPROVER_assume(list.length > 0);

    /* 4. Choose non‑deterministic indices within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 5. Save old state */
    struct aws_array_list old = list;

    /* 6. Save a copy of the whole data buffer for immutability checks */
    size_t data_bytes = list.length * list.item_size;
    unsigned char *old_data = NULL;
    if (data_bytes > 0) {
        old_data = malloc(data_bytes);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, (const unsigned char *)list.data, data_bytes);
    }

    /* 7. Save copies of the two elements that will be swapped */
    unsigned char *elem_a = NULL;
    unsigned char *elem_b = NULL;
    if (list.item_size > 0) {
        elem_a = malloc(list.item_size);
        elem_b = malloc(list.item_size);
        __CPROVER_assume(elem_a != NULL);
        __CPROVER_assume(elem_b != NULL);
        memcpy(elem_a,
               (const unsigned char *)list.data + a * list.item_size,
               list.item_size);
        memcpy(elem_b,
               (const unsigned char *)list.data + b * list.item_size,
               list.item_size);
    }

    /* 8. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 9. Post‑condition: swapped elements */
    if (list.item_size > 0) {
        if (a != b) {
            assert(memcmp((const unsigned char *)list.data + a * list.item_size,
                          elem_b,
                          list.item_size) == 0);
            assert(memcmp((const unsigned char *)list.data + b * list.item_size,
                          elem_a,
                          list.item_size) == 0);
        } else {
            /* a == b ⇒ element unchanged */
            assert(memcmp((const unsigned char *)list.data + a * list.item_size,
                          elem_a,
                          list.item_size) == 0);
        }
    }

    /* 10. All other elements unchanged */
    if (data_bytes > 0) {
        for (size_t i = 0; i < list.length; ++i) {
            if (i == a || i == b) {
                continue;
            }
            assert(memcmp((const unsigned char *)list.data + i * list.item_size,
                          old_data + i * list.item_size,
                          list.item_size) == 0);
        }
    }

    /* 11. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 12. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* 13. Free temporary buffers */
    free(old_data);
    free(elem_a);
    free(elem_b);
}
