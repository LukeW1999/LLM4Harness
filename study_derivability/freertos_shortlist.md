# FreeRTOS-Plus-TCP Shortlist — 40 Functions (Tier A / B / C)

**Status**: DRAFT — needs PI confirmation.  
Generated from enumeration of `test/cbmc/proofs/` (79 unique harnesses after deduplication).  
All paths relative to `study_derivability/corpora/FreeRTOS-Plus-TCP/`.

Tier classification rationale:
- **Tier A** (CVE-critical): network-protocol parsing functions linked to known buffer-overflow
  / out-of-bounds CVEs (ARP, DNS, DHCP processing); based on Amusuo et al. 2024 taxonomy.
- **Tier B** (mid-complexity): protocol state-machine and checksum functions; moderate LoC,
  non-trivial data-structure manipulation.
- **Tier C** (data-structure-like): cache lookup / entry management; simpler control flow,
  comparable in style to aws-c-common list / buffer functions.

---

## Tier A — CVE-critical (14 functions)

| Function | Module | LoC | Harness path |
|---|---|---|---|
| ARPProcessPacket | ARP | 116 | `test/cbmc/proofs/ARP/ARPProcessPacket/ARPProcessPacket_harness.c` |
| ARPAgeCache | ARP | 73 | `test/cbmc/proofs/ARP/ARPAgeCache/ARPAgeCache_harness.c` |
| xCheckRequiresARPResolution | ARP | 118 | `test/cbmc/proofs/ARP/xCheckRequiresARPResolution/xCheckRequiresARPResolution_harness.c` |
| OutputARPRequest | ARP | 143 | `test/cbmc/proofs/ARP/ARP_FreeRTOS_OutputARPRequest/OutputARPRequest_harness.c` |
| DNS_ParseDNSReply | DNS_ParseDNSReply | 230 | `test/cbmc/proofs/DNS_ParseDNSReply/DNS_ParseDNSReply_harness.c` |
| DNSgetHostByName | DNS | 303 | `test/cbmc/proofs/DNS/DNSgetHostByName/DNSgetHostByName_harness.c` |
| DNSgetHostByName_a | DNS | 299 | `test/cbmc/proofs/DNS/DNSgetHostByName_a/DNSgetHostByName_a_harness.c` |
| DHCPProcess | DHCP | 221 | `test/cbmc/proofs/DHCP/DHCPProcess/DHCPProcess_harness.c` |
| DHCPProcessEndPoint | DHCP | 241 | `test/cbmc/proofs/DHCP/DHCPProcessEndPoint/DHCPProcessEndPoint_harness.c` |
| ProcessIPPacket | parsing | 196 | `test/cbmc/proofs/parsing/ProcessIPPacket/ProcessIPPacket_harness.c` |
| ProcessReceivedTCPPacket | parsing | 192 | `test/cbmc/proofs/parsing/ProcessReceivedTCPPacket/ProcessReceivedTCPPacket_harness.c` |
| ProcessReceivedUDPPacket | parsing | 107 | `test/cbmc/proofs/parsing/ProcessReceivedUDPPacket/ProcessReceivedUDPPacket_harness.c` |
| ProcessICMPPacket | ICMP | 70 | `test/cbmc/proofs/ICMP/ProcessICMPPacket/ProcessICMPPacket_harness.c` |
| HandleEthernetPacket | IP | 88 | `test/cbmc/proofs/IP/HandleEthernetPacket/HandleEthernetPacket_harness.c` |

## Tier B — Mid-complexity (14 functions)

| Function | Module | LoC | Harness path |
|---|---|---|---|
| TCPHandleState | TCP | 289 | `test/cbmc/proofs/TCP/prvTCPHandleState/TCPHandleState_harness.c` |
| TCPReturnPacket | TCP | 260 | `test/cbmc/proofs/TCP/prvTCPReturnPacket/TCPReturnPacket_harness.c` |
| TCPPrepareSend | TCP | 166 | `test/cbmc/proofs/TCP/prvTCPPrepareSend/TCPPrepareSend_harness.c` |
| prvHandleListen | TCP | 140 | `test/cbmc/proofs/TCP/prvHandleListen/prvHandleListen_harness.c` |
| SendData | TCP | 144 | `test/cbmc/proofs/TCP/prvSendData/SendData_harness.c` |
| ProcessICMPEchoRequest | ICMP | 84 | `test/cbmc/proofs/ICMP/ProcessICMPEchoRequest/ProcessICMPEchoRequest_harness.c` |
| vProcessGeneratedUDPPacket | UDP | 142 | `test/cbmc/proofs/UDP/vProcessGeneratedUDPPacket/vProcessGeneratedUDPPacket_harness.c` |
| ConsiderFrameForProcessing | IP | 66 | `test/cbmc/proofs/IP/ConsiderFrameForProcessing/ConsiderFrameForProcessing_harness.c` |
| ProcessEthernetPacket | IP | 96 | `test/cbmc/proofs/IP/ProcessEthernetPacket/ProcessEthernetPacket_harness.c` |
| usGenerateProtocolChecksum | IPUtils | 79 | `test/cbmc/proofs/IPUtils/usGenerateProtocolChecksum/usGenerateProtocolChecksum_harness.c` |
| pxDuplicateNetworkBufferWithDescriptor | IPUtils | 89 | `test/cbmc/proofs/IPUtils/pxDuplicateNetworkBufferWithDescriptor/pxDuplicateNetworkBufferWithDescriptor_harness.c` |
| vSocketClose | Socket | 125 | `test/cbmc/proofs/Socket/vSocketClose/vSocketClose_harness.c` |
| vSocketWakeUpUser | Socket | 172 | `test/cbmc/proofs/Socket/vSocketWakeUpUser/vSocketWakeUpUser_harness.c` |
| MatchingEndpoint | Routing | 86 | `test/cbmc/proofs/Routing/MatchingEndpoint/MatchingEndpoint_harness.c` |

## Tier C — Data-structure-like (12 functions)

| Function | Module | LoC | Harness path |
|---|---|---|---|
| ARPGetCacheEntry | ARP | 43 | `test/cbmc/proofs/ARP/ARPGetCacheEntry/ARPGetCacheEntry_harness.c` |
| ARPGetCacheEntryByMac | ARP | 28 | `test/cbmc/proofs/ARP/ARPGetCacheEntryByMac/ARPGetCacheEntryByMac_harness.c` |
| ARPRefreshCacheEntry | ARP | 44 | `test/cbmc/proofs/ARP/ARPRefreshCacheEntry/ARPRefreshCacheEntry_harness.c` |
| ARPGenerateRequestPacket | ARP | 39 | `test/cbmc/proofs/ARP/ARPGenerateRequestPacket/ARPGenerateRequestPacket_harness.c` |
| ulARPRemoveCacheEntryByMac | ARP | 20 | `test/cbmc/proofs/ARP/ulARPRemoveCacheEntryByMac/ulARPRemoveCacheEntryByMac_harness.c` |
| ClearARP | ARP | 19 | `test/cbmc/proofs/ARP/ARP_FreeRTOS_ClearARP/ClearARP_harness.c` |
| IsDHCPSocket | DHCP | 49 | `test/cbmc/proofs/DHCP/IsDHCPSocket/IsDHCPSocket_harness.c` |
| DNSclear | DNS | 19 | `test/cbmc/proofs/DNS/DNSclear/DNSclear_harness.c` |
| NBNSHandlePacket | DNS | 41 | `test/cbmc/proofs/DNS/NBNSHandlePacket/NBNSHandlePacket_harness.c` |
| ReadNameField | ReadNameField | 98 | `test/cbmc/proofs/ReadNameField/ReadNameField_harness.c` |
| SkipNameField | SkipNameField | 55 | `test/cbmc/proofs/SkipNameField/SkipNameField_harness.c` |
| prvChecksumIPv6Checks | prvChecksumIPv6Checks | 46 | `test/cbmc/proofs/prvChecksumIPv6Checks/prvChecksumIPv6Checks_harness.c` |

---

## Excluded / Variant duplicates

The following are excluded because they are alternative configurations of an already-listed
function, or are infrastructure-only files:

| Excluded | Reason |
|---|---|
| `ARP_OutputARPRequest_buffer_alloc1/` | Variant of OutputARPRequest (different alloc config) |
| `ARP_OutputARPRequest_buffer_alloc2/` | Same |
| `Socket/vSocketBind/ALLOW_*` (3 variants) | Same function, different compile flags |
| `Socket/lTCPAddRxdata` | Borderline Tier B/C; can swap in for any Tier C if desired |
| `TCPWin/vTCPWindowDestroy` | Standalone utility; available as swap-in |
| `DNS/DNS_TreatNBNS`, `DNSgetHostByName_cancel` | Available as swap-ins |
| `DHCPv6/*` (6 functions) | IPv6 variant; omitted to keep corpus focused on IPv4 |
| `ND/*` (2 functions) | IPv6; same reason |
| `RA/*` (2 functions) | IPv6; same reason |
| `parsing/*_IPv6` (2 functions) | IPv6 |
| `TCP/*_IPv6` (2 functions) | IPv6 |
| `CBMCStubLibrary/tasksStubs` | Infrastructure stub, not a function-under-test |
| `utility/memory_assignments` | Infrastructure helper |
| `ProcessDHCPReplies` | Redundant with DHCPProcess |
| `SendEventToIPTask` | Thin wrapper, low assert count |
| `xRecv_Update_IPv4`, `xRecv_Update_IPv6` | Low-level recv helpers |

---

## Notes for PI

1. **IPv6 exclusion**: DHCPv6, ND, RA, and IPv6 parsing variants are excluded to keep the
   corpus focused. If IPv6 generalization is interesting for the paper, these can be swapped in.

2. **vSocketBind variants**: Three compile-flag variants exist. If included, should pick one
   (e.g., `DONT_ALLOW_SOCKET_SEND_WITHOUT_BIND`) to avoid triple-counting the same function.

3. **Total**: 14 + 14 + 12 = **40 functions** as specified.
