#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static uint8_t s_expected_cat_byte(
    size_t offset,
    const uint8_t *src1,
    size_t src1_len,
    const uint8_t *src2,
    size_t src2_len,
    const uint8_t *src3,
    size_t src3_len,
    size_t number_of_args) {

    if (offset < src1_len) {
        return src1[offset];
    }

    offset -= src1_len;

    if (offset < src2_len) {
        return src2[offset];
    }

    offset -= src2_len;

    if (number_of_args == 3 && offset < src3_len) {
        return src3[offset];
    }

    return 0;
}
