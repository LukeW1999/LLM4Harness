#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_nospec_mask_harness() {
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    size_t result = aws_nospec_mask(index, bound);

    size_t half_size_max = SIZE_MAX / 2;
    
    if (index >= bound || bound > half_size_max || index > half_size_max) {
        assert(result == 0);
    } else {
        assert(result == (size_t)UINTPTR_MAX);
    }
}
