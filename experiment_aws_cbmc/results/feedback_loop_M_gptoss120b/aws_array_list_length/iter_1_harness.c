#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    /* 3. Prepare nondeterministic inputs */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + MAX_ITEM_SIZE);

    size_t item_sz = list.item_size;               /* item_size is guaranteed > 0 by validity */
    uint8_t val_buf[MAX_ITEM_SIZE];
    for (size_t i = 0; i < item_sz; ++i) {
        val_buf[i] = nondet_uint8_t();
    }

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, val_buf, index);

    /* 5. Global post‑condition: the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* 6. Post‑conditions depending on the return value */
    if (result == AWS_OP_SUCCESS) {
        /* The element at the given index must now equal the supplied value */
        uint8_t read_back[MAX_ITEM_SIZE];
        memcpy(read_back,
               (uint8_t *)list.data + (index * list.item_size),
               list.item_size);
        assert_bytes_match(read_back, val_buf, list.item_size);

        /* Length handling */
        if (index >= old.length) {
            /* Length should have grown to index+1 */
            assert(list.length == index + 1);
        } else {
            /* Length must be unchanged */
            assert(list.length == old.length);
        }
    } else {
        /* On failure the list must be unchanged */
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    /* 7. Fields that never change regardless of success or failure */
    assert(list.alloc == old.alloc);               /* allocator is never altered */
    assert(list.item_size == old.item_size);       /* item size is immutable */

    /* For static lists (alloc == NULL) the data pointer and current size must stay the same */
    if (list.alloc == NULL) {
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }
}
