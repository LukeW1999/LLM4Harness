# Vendored source snapshots

## aws-c-common-snapshot.tar.gz
The exact `aws-c-common` source tree the mutants (`../mutants/`) were generated
from. On the experiment server this lived at `/root/aws-c-common` as a
**tarball-extracted tree, not a git checkout**, so there is no upstream commit
hash pinning it. This archive is the canonical record of that source and is
preserved here for reproducibility (Zenodo artifact) before the server was
decommissioned. License: Apache-2.0 (redistributable; LICENSE/NOTICE included in
the archive).

To regenerate mutants, extract to `/root/aws-c-common` (the path hard-coded in
`scripts/cbmc_runner.py` / `run_mutation_oracle_cbmc.py`; see `../../REBUILD.md`).

## s2n-tls
Not snapshotted here: it was a git checkout pinned at commit `d4805fd`
(`git clone https://github.com/aws/s2n-tls`), recoverable from upstream, and its
mutants are already in `../mutants_s2n/`. See `../../REBUILD.md` §3.
