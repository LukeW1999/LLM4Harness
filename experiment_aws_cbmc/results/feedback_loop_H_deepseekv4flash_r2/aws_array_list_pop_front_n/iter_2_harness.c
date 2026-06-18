#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness() {
    /* nondet inputs */
    struct aws_array_list list;
    size_t n;

    /* bound the list to keep the proof tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* ensure the data member is allocated and list is valid */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save the old state */
    struct aws_array_list old = list;
    size_t old_length = old.length;
    size_t max_current_size = MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE;

    /* make a byte-level copy of the old data region */
    uint8_t old_data[max_current_size];
    for (size_t i = 0; i < old.current_size; i++) {
        old_data[i] = ((uint8_t *)old.data)[i];
    }

    /* call the function under test */
    aws_array_list_pop_front_n(&list, n);

    /* postconditions: fields that must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* postcondition: length change */
    if (old_length == 0 || n >= old_length) {
        assert(list.length == 0);
    } else if (n == 0) {
        assert(list.length == old_length);
    } else {
        assert(list.length == old_length - n);
    }

    /* postcondition: data content is shifted correctly */
    if (n > 0 && n < old_length) {
        size_t remaining = old_length - n;
        size_t item_sz = old.item_size;
        for (size_t i = 0; i < remaining; i++) {
            /* element at new index i should equal old element at index i + n */
            const uint8_t *new_ptr = (const uint8_t *)list.data + i * item_sz;
            const uint8_t *old_ptr = old_data + (i + n) * item_sz;
            assert_bytes_match(new_ptr, old_ptr, item_sz);
        }
    } else if (n == 0) {
        /* no removal: the entire data region must be unchanged */
        assert_bytes_match((const uint8_t *)list.data, old_data, old.current_size);
    } else { /* n >= old_length : list cleared but data content unchanged */
        assert_bytes_match((const uint8_t *)list.data, old_data, old.current_size);
    }

    /* postcondition: list remains valid */
    assert(aws_array_list_is_valid(&list));
}
