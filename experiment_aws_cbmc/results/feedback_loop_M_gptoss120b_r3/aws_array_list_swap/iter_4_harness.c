#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Ensure there is at least one element and a reasonable item size */
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= 64);               /* bound item size */
    __CPROVER_assume(list.current_size >= list.length * list.item_size);

    /* 2. Choose two indices (they may be equal) */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 3. Save copies of the two elements that will be swapped */
    uint8_t *old_a = malloc(list.item_size);
    uint8_t *old_b = malloc(list.item_size);
    __CPROVER_assume(old_a != NULL);
    __CPROVER_assume(old_b != NULL);
    memcpy(old_a,
           (uint8_t *)list.data + a * list.item_size,
           list.item_size);
    memcpy(old_b,
           (uint8_t *)list.data + b * list.item_size,
           list.item_size);

    /* 4. Save the whole list structure for later field checks */
    struct aws_array_list old = list;

    /* 5. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Fields that must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 7. Verify the two swapped elements (if a != b) */
    if (a != b) {
        assert(memcmp((uint8_t *)list.data + a * list.item_size,
                      old_b,
                      list.item_size) == 0);
        assert(memcmp((uint8_t *)list.data + b * list.item_size,
                      old_a,
                      list.item_size) == 0);
    } else {
        /* When a == b the element should be unchanged */
        assert(memcmp((uint8_t *)list.data + a * list.item_size,
                      old_a,
                      list.item_size) == 0);
    }

    /* 8. List must still be valid */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(old_a);
    free(old_b);
}
