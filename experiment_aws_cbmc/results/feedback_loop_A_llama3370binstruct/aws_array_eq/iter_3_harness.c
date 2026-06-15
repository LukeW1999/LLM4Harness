#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_eq_harness() {
    struct aws_array_list *list1 = ensure_array_list_is_bounded_and_allocated(MAX_BUFFER_SIZE, aws_default_allocator());
    struct aws_array_list *list2 = ensure_array_list_is_bounded_and_allocated(MAX_BUFFER_SIZE, aws_default_allocator());

    __CPROVER_assume(aws_array_list_is_valid(list1));
    __CPROVER_assume(aws_array_list_is_valid(list2));

    bool result = aws_array_eq(list1, list2);

    assert(result == (list1->length == list2->length));
    if (result) {
        for (size_t i = 0; i < list1->length; i++) {
            assert(((uint8_t *)list1->data)[i] == ((uint8_t *)list2->data)[i]);
        }
    } else {
        bool found_difference = false;
        for (size_t i = 0; i < list1->length && i < list2->length; i++) {
            if (((uint8_t *)list1->data)[i] != ((uint8_t *)list2->data)[i]) {
                found_difference = true;
                break;
            }
        }
        if (!found_difference) {
            assert(list1->length != list2->length);
        }
    }

    assert(aws_array_list_is_valid(list1));
    assert(aws_array_list_is_valid(list2));
}
