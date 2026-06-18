#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    size_t index;
    uint8_t val_buf[MAX_ITEM_SIZE];
    const void *val = val_buf;

    /* Initialize list with non-deterministic fields, bound and allocator */
    list.alloc = aws_default_allocator();
    list.item_size = nondet_size_t();
    list.current_size = nondet_size_t();
    list.length = nondet_size_t();
    list.data = NULL;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state for immutability checks on failure */
    struct aws_array_list old = list
