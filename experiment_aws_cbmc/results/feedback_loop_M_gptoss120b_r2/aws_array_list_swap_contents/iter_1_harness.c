#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_contents_harness(void) {
    /* 1. Declare two array lists */
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    /* 2. Bound the structures */
    __CPROVER_assume(aws_array_list_is_bounded(&list_a,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* 3. Allocate internal data buffers */
    ensure_array_list_has_allocated_data_member(&list_a);
    ensure_array_list_has_allocated_data_member(&list_b);

    /* 4. Set common allocator */
    list_a.alloc = aws_default_allocator();
    list_b.alloc = list_a.alloc;

    /* 5. Choose a common item size within bounds */
    size_t item_sz = nondet_size_t();
    __CPROVER_assume(item_sz > 0 && item_sz <= MAX_ITEM_SIZE);
    list_a.item_size = item_sz;
    list_b.item_size = item_sz;

    /* 6. Assume both lists are initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* 7. Ensure the two list objects are distinct */
    __CPROVER_assume(&list_a != &list_b);

    /* 8. Save old state for later comparison */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* 9. Call the function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* 10. Post‑condition: fields are swapped */
    assert(list_a.alloc == old_b.alloc);
    assert(list_b.alloc == old_a.alloc);

    assert(list_a.item_size == old_b.item_size);
    assert(list_b.item_size == old_a.item_size);

    assert(list_a.current_size == old_b.current_size);
    assert(list_b.current_size == old_a.current_size);

    assert(list_a.length == old_b.length);
    assert(list_b.length == old_a.length);

    assert(list_a.data == old_b.data);
    assert(list_b.data == old_a.data);

    /* 11. Unchanged fields (allocators are the same, item_size equal) */
    assert(list_a.alloc == old_a.alloc);
    assert(list_b.alloc == old_b.alloc);
    assert(list_a.item_size == old_a.item_size);
    assert(list_b.item_size == old_b.item_size);

    /* 12. Validity invariants must still hold */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
