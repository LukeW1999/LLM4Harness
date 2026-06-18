#include <proof_helpers/make_common_data_structures.h>

void harness() {
    size_t a;
    size_t b;
    size_t r = aws_add_size_saturating(a, b);
    if (a <= (size_t)-1 - b) {
        assert(r == a + b);
    } else {
        assert(r == (size_t)-1);
    }
}
