#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Choose nondeterministic indices within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t len = aws_array_list_length(&list);
    __CPROVER_assume(a < len);
    __CPROVER_assume(b < len);

    /* 4. Save the elements at the chosen indices */
    uint8_t old_a[MAX_ITEM_SIZE];
    uint8_t old_b[MAX_ITEM_SIZE];
    memcpy(old_a,
           (uint8_t *)list.data + (a * list.item_size),
           list.item_size);
    memcpy(old_b,
           (uint8_t *)list.data + (b * list.item_size),
           list.item_size);

    /* 5. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Assert fields that must not change */
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);

    /* 7. Assert the swap semantics */
    assert_bytes_match((uint8_t *)list.data + (a * list.item_size),
                       old_b,
                       list.item_size);
    assert_bytes_match((uint8_t *)list.data + (b * list.item_size),
                       old_a,
                       list.item_size);

    /* 8. Assert all other elements are unchanged */
    for (size_t i = 0; i < len; ++i) {
        if (i != a && i != b) {
            assert_bytes_match((uint8_t *)list.data + (i * list.item_size),
                               (uint8_t *)old.data + (i * old.item_size),
                               list.item_size);
        }
    }

    /* 9. Assert the validity invariant holds */
    assert(aws_array_list_is_valid(&list));
}
