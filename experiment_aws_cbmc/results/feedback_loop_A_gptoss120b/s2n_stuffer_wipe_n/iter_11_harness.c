#include <proof_helpers/make_common_data_structures.h>
#include <s2n.h>
#include <string.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

#ifndef S2N_SUCCESS
#define S2N_SUCCESS 0
#endif

#ifndef S2N_WIPE_PATTERN
#define S2N_WIPE_PATTERN ((uint8_t)0x00)
#endif

#ifndef S2N_MIN
#define S2N_MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef POSIX_PRECONDITION
#define POSIX_PRECONDITION(x) ((void)0)
#endif
#ifndef POSIX_POSTCONDITION
#define POSIX_POSTCONDITION(x) ((void)0)
#endif
#ifndef POSIX_CHECKED_MEMSET
#define POSIX_CHECKED_MEMSET(p,c,n) memset((p),(c),(n))
#endif

void s2n_stuffer_wipe_n_harness(void) {
    struct s2n_stuffer stuffer;
    static uint8_t buffer[MAX_BUFFER_SIZE];

    uint32_t max_blob_size = nondet_uint32_t();
    __CPROVER_assume(max_blob_size <= MAX_BUFFER_SIZE);

    stuffer.blob.data = (max_blob_size == 0) ? NULL : buffer;
    __CPROVER_assume(stuffer.blob.data != NULL || max_blob_size == 0);

    stuffer.blob.size = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.size <= max_blob_size);
    stuffer.blob.allocated = max_blob_size;
    stuffer.blob.growable = nondet_bool();

    stuffer.write_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    stuffer.read_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    stuffer.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    stuffer.alloced = nondet_bool();
    stuffer.growable = nondet_bool();
    stuffer.tainted = nondet_bool();

    __CPROVER_assume(s2n_stuffer_validate(&stuffer));

    struct s2n_stuffer old = stuffer;

    uint32_t n = nondet_uint32_t();

    int result = s2n_stuffer_wipe_n(&stuffer, n);
    assert(result == S2N_SUCCESS);

    uint32_t expected_wipe = S2N_MIN(n, old.write_cursor);
    assert(stuffer.write_cursor == old.write_cursor - expected_wipe);

    uint32_t expected_read = S2N_MIN(old.read_cursor, stuffer.write_cursor);
    assert(stuffer.read_cursor == expected_read);

    for (uint32_t i = 0; i < expected_wipe; ++i) {
        assert(stuffer.blob.data[stuffer.write_cursor + i] == S2N_WIPE_PATTERN);
    }

    assert(stuffer.blob.size == old.blob.size);
    assert(stuffer.blob.allocated == old.blob.allocated);
    assert(stuffer.blob.growable == old.blob.growable);
    assert(stuffer.blob.data == old.blob.data);

    assert(stuffer.high_water_mark == old.high_water_mark);
    assert(stuffer.alloced == old.alloced);
    assert(stuffer.growable == old.growable);
    assert(stuffer.tainted == old.tainted);

    assert(s2n_stuffer_validate(&stuffer));
}
