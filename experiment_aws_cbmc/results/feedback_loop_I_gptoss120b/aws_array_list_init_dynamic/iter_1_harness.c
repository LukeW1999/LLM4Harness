#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_dynamic_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* allocator must be non‑NULL */
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic inputs respecting preconditions */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list,
                                             alloc,
                                             initial_item_allocation,
                                             item_size);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* fields that must change on success */
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        /* postcondition from implementation */
        assert(list.current_size == 0 || list.data != NULL);
    } else {
        /* on failure the structure must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.length == old.length);          /* length is never modified */
    /* note: list.data may change on success, so no assert here */

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
