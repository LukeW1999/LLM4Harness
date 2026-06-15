#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < aws_array_list_length(&list));
    int result = aws_array_list_erase(&list, index);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_length(&list) == aws_array_list_length(&old) - 1);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    } else {
        assert(aws_array_list_length(&list) == aws_array_list_length(&old));
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* 7. Test edge cases */
    aws_array_list_init(&list, aws_default_allocator(), sizeof(int), 10);
    assert(aws_array_list_is_valid(&list));
    assert(aws_array_list_length(&list) == 0);
    int value = 5;
    aws_array_list_push_back(&list, &value);
    assert(aws_array_list_length(&list) == 1);
    aws_array_list_erase(&list, 0);
    assert(aws_array_list_length(&list) == 0);
    assert(aws_array_list_is_valid(&list));
}

int main() {
    aws_array_list_erase_harness();
    return 0;
}
