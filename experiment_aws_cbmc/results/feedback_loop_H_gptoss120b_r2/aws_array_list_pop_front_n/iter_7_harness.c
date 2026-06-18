#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_pop_front_n_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.allocator = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    size_t old_len = old.length;
    size_t item_sz = old.item_size;

    uint8_t *old_bytes = NULL;
    if (old_len > 0 && item_sz > 0) {
        old_bytes = malloc(old_len * item_sz);
        __CPROVER_assume(old_bytes != NULL);
        memcpy(old_bytes, old.data, old_len * item_sz);
    }

    size_t n = (size_t)nondet_uint64_t();
    __CPROVER_assume(n <= old_len);

    aws_array_list_pop_front_n(&list, n);

    if (n >= old_len) {
        assert(list.length == 0);
    } else {
        assert(list.length == old_len - n);
        if (list.length > 0 && item_sz > 0) {
            assert_bytes_match((uint8_t *)list.data,
                old_bytes + n * item_sz,
                list.length * item_sz);
        }
    }

    assert(list.allocator == old.allocator);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);

    assert(aws_array_list_is_valid(&list));

    free(old_bytes);
}
