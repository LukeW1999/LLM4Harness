#include <proof_helpers/make_common_data_structures.h>

int s2n_stuffer_wipe(struct s2n_stuffer *stuffer);

void s2n_stuffer_wipe_harness(void) {
    struct s2n_stuffer stuffer;

    uint32_t blob_size;
    __CPROVER_assume(blob_size <= 64);

    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = blob_size;
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    stuffer.read_cursor = nondet_uint32();
    __CPROVER_assume(stuffer.read_cursor <= blob_size);

    stuffer.write_cursor = nondet_uint32();
    __CPROVER_assume(stuffer.write_cursor <= blob_size);
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);

    stuffer.high_water_mark = nondet_uint32();
    __CPROVER_assume(stuffer.high_water_mark <= blob_size);

    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    struct s2n_stuffer old = stuffer;

    int result = s2n_stuffer_wipe(&stuffer);

    if (result == 0) {
        assert(stuffer.read_cursor == 0);
        assert(stuffer.write_cursor == 0);
        assert(stuffer.high_water_mark == 0);

        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);

        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
    } else {
        assert(stuffer.read_cursor == 0);
        assert(stuffer.write_cursor == 0);
        assert(stuffer.high_water_mark == 0);
    }

    return 0;
}
