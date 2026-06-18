#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

static void assume_valid_stuffer(const struct s2n_stuffer *s)
{
    __CPROVER_assume(s->read_cursor <= s->write_cursor);
    __CPROVER_assume(s->write_cursor <= s->blob.size);
    __CPROVER_assume(s->high_water_mark <= s->blob.size);
    __CPROVER_assume(s->blob.allocated == 0 || s->blob.allocated >= s->blob.size);
}

void s2n_stuffer_write_bytes_harness(void)
{
    struct s2n_stuffer stuffer;

    uint8_t blob_buf[MAX_BUFFER_SIZE];
    uint32_t blob_buf_len = nondet_uint32_t();
    __CPROVER_assume(blob_buf_len <= MAX_BUFFER_SIZE);
    stuffer.blob.data = blob_buf;
    stuffer.blob.size = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.size <= blob_buf_len);
    stuffer.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.allocated == 0 || stuffer.blob.allocated >= stuffer.blob.size);
    stuffer.blob.growable = nondet_bool();

    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced = nondet_bool();
    stuffer.growable = nondet_bool();
    stuffer.tainted = nondet_bool();

    assume_valid_stuffer(&stuffer);

    struct s2n_stuffer old = stuffer;

    uint8_t in_buf[MAX_BUFFER_SIZE];
    uint32_t in_len = nondet_uint32_t();
    __CPROVER_assume(in_len <= MAX_BUFFER_SIZE);
    uint8_t *in = in_buf;

    int result = s2n_stuffer_write_bytes(&stuffer, in, in_len);

    if (result == 0) {
        __CPROVER_assert(stuffer.write_cursor == old.write_cursor + in_len, "write_cursor advanced");
        uint32_t expected_hwm = old.high_water_mark;
        if (stuffer.write_cursor > expected_hwm) {
            expected_hwm = stuffer.write_cursor;
        }
        __CPROVER_assert(stuffer.high_water_mark == expected_hwm, "high_water_mark updated");
    } else {
        __CPROVER_assert(stuffer.write_cursor == old.write_cursor, "write_cursor unchanged");
        __CPROVER_assert(stuffer.high_water_mark == old.high_water_mark, "high_water_mark unchanged");
    }

    __CPROVER_assert(stuffer.blob.data == old.blob.data, "blob data unchanged");
    __CPROVER_assert(stuffer.blob.size == old.blob.size, "blob size unchanged");
    __CPROVER_assert(stuffer.blob.allocated == old.blob.allocated, "blob allocated unchanged");
    __CPROVER_assert(stuffer.blob.growable == old.blob.growable, "blob growable unchanged");
    __CPROVER_assert(stuffer.read_cursor == old.read_cursor, "read_cursor unchanged");
    __CPROVER_assert(stuffer.alloced == old.alloced, "alloced unchanged");
    __CPROVER_assert(stuffer.growable == old.growable, "growable unchanged");
    __CPROVER_assert(stuffer.tainted == old.tainted, "tainted unchanged");

    assume_valid_stuffer(&stuffer);
}
