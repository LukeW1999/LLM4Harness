#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Non‑deterministic n */
    size_t n = nondet_size_t();

    /* 4. Call function under test */
    aws_array_list_pop_front_n(&list, n);

    /* 5. Post‑condition: length */
    size_t expected_length = (n >= old.length) ? 0 : old.length - n;
    assert(list.length == expected_length);

    /* 6. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 7. Data content preservation for the remaining items */
    if (expected_length > 0) {
        size_t copy_bytes = expected_length * old.item_size;
        assert_bytes_match((uint8_t *)list.data,
                           (uint8_t *)old.data + n * old.item_size,
                           copy_bytes);
    }

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
