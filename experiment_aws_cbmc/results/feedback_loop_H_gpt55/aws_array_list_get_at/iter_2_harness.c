#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_get_at_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    list.alloc = nondet_bool() ? allocator : NULL;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    uint8_t val[MAX_ITEM_SIZE];
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_byte);

    struct aws_array_list old = list;

    int result = aws_array_list_get_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(list.alloc == old.alloc);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert_array_list_equivalence(&list, &old, &old_byte);

    if (index < old.length) {
        assert(result == AWS_OP_SUCCESS);

        size_t offset = 0;
        int offset_result = aws_mul_size_checked(index, old.item_size, &offset);
        assert(offset_result == AWS_OP_SUCCESS);

        if (old.item_size > 0) {
            assert_bytes_match(val, (const uint8_t *)old.data + offset, old.item_size);
        }
    } else {
        assert(result == AWS_OP_ERR);
    }

    assert(aws_array_list_is_valid(&list));
}
