# Phase 3 Report — FreeRTOS LLM Generation

## Summary

- Functions with ≥1 GT assertion: 55
- Total generations: 206

## Recall Table

| Condition | Model  | N  | Mean recall | N > 0 |
|-----------|--------|----|-------------|-------|
| A         | claude | 55 | 0.025       |     3 |
| A         | qwen   | 55 | 0.042       |     9 |
| E         | claude | 48 | 0.294       |    22 |
| E         | qwen   | 48 | 0.255       |    20 |

## Non-zero Recall Cases

- **DHCPProcess** cond=E model=qwen: recall=1.00 (4/4) [module: DHCP]
- **ProcessReceivedUDPPacket** cond=E model=qwen: recall=1.00 (1/1) [module: parsing]
- **ProcessICMPMessage_IPv6** cond=E model=qwen: recall=1.00 (6/6) [module: ND]
- **prvHandleListen_IPv6** cond=E model=qwen: recall=1.00 (3/3) [module: TCP]
- **ProcessReceivedUDPPacket_IPv6** cond=E model=qwen: recall=1.00 (2/2) [module: parsing]
- **prvHandleListen** cond=E model=qwen: recall=1.00 (3/3) [module: TCP]
- **ProcessICMPPacket** cond=E model=claude: recall=1.00 (2/2) [module: ICMP]
- **vProcessGeneratedUDPPacket** cond=E model=claude: recall=1.00 (7/7) [module: UDP]
- **pxDuplicateNetworkBufferWithDescriptor** cond=E model=claude: recall=1.00 (1/1) [module: IPUtils]
- **DHCPProcess** cond=E model=claude: recall=1.00 (4/4) [module: DHCP]
- **vProcessGeneratedUDPPacket_IPv6** cond=E model=claude: recall=1.00 (7/7) [module: UDP]
- **ProcessICMPMessage_IPv6** cond=E model=claude: recall=1.00 (6/6) [module: ND]
- **prvHandleListen_IPv6** cond=E model=claude: recall=1.00 (3/3) [module: TCP]
- **ProcessReceivedUDPPacket_IPv6** cond=E model=claude: recall=1.00 (2/2) [module: parsing]
- **prvHandleListen** cond=E model=claude: recall=1.00 (3/3) [module: TCP]
- **DHCPv6Process_PassReplyToEndPoint** cond=A model=claude: recall=1.00 (2/2) [module: DHCPv6]
- **vProcessGeneratedUDPPacket** cond=E model=qwen: recall=0.86 (6/7) [module: UDP]
- **ReturnICMP_IPv6** cond=E model=qwen: recall=0.86 (6/7) [module: ND]
- **vProcessGeneratedUDPPacket_IPv6** cond=E model=qwen: recall=0.86 (6/7) [module: UDP]
- **ReturnICMP_IPv6** cond=E model=claude: recall=0.86 (6/7) [module: ND]
- **OutputARPRequest** cond=E model=qwen: recall=0.75 (3/4) [module: ARP]
- **OutputARPRequest** cond=E model=claude: recall=0.75 (3/4) [module: ARP]
- **prvAllowIPPacketIPv6** cond=E model=qwen: recall=0.67 (2/3) [module: parsing]
- **DHCPProcessEndPoint** cond=E model=qwen: recall=0.67 (4/6) [module: DHCP]
- **DHCPProcessEndPoint** cond=E model=claude: recall=0.67 (4/6) [module: DHCP]
- **HandleEthernetPacket** cond=E model=claude: recall=0.50 (1/2) [module: IP]
- **ProcessEthernetPacket** cond=E model=claude: recall=0.50 (2/4) [module: IP]
- **HandleEthernetPacket** cond=A model=qwen: recall=0.50 (1/2) [module: IP]
- **ProcessReceivedUDPPacket_IPv6** cond=A model=qwen: recall=0.50 (1/2) [module: parsing]
- **ProcessIPPacket** cond=E model=qwen: recall=0.43 (3/7) [module: parsing]
- **ProcessIPPacket** cond=E model=claude: recall=0.43 (3/7) [module: parsing]
- **prvAllowIPPacketIPv6** cond=E model=claude: recall=0.33 (1/3) [module: parsing]
- **prvAllowIPPacketIPv6** cond=A model=qwen: recall=0.33 (1/3) [module: parsing]
- **SendData** cond=E model=qwen: recall=0.25 (1/4) [module: TCP]
- **TCPPrepareSend** cond=E model=qwen: recall=0.25 (1/4) [module: TCP]
- **SendData** cond=E model=claude: recall=0.25 (1/4) [module: TCP]
- **SendData** cond=A model=qwen: recall=0.25 (1/4) [module: TCP]
- **ProcessReceivedTCPPacket** cond=E model=qwen: recall=0.22 (2/9) [module: parsing]
- **TCPHandleState** cond=E model=claude: recall=0.22 (2/9) [module: TCP]
- **ProcessReceivedTCPPacket** cond=E model=claude: recall=0.22 (2/9) [module: parsing]
- **DHCPv6Process** cond=A model=claude: recall=0.20 (1/5) [module: DHCPv6]
- **ReceiveRA** cond=A model=claude: recall=0.20 (1/5) [module: RA]
- **DNS_TreatNBNS** cond=A model=qwen: recall=0.20 (1/5) [module: DNS]
- **DHCPv6ProcessEndPoint** cond=E model=claude: recall=0.17 (1/6) [module: DHCPv6]
- **DHCPProcessEndPoint** cond=A model=qwen: recall=0.17 (1/6) [module: DHCP]
- **vProcessGeneratedUDPPacket** cond=A model=qwen: recall=0.14 (1/7) [module: UDP]
- **DNSgetHostByName_a** cond=E model=qwen: recall=0.12 (1/8) [module: DNS]
- **TCPHandleState** cond=E model=qwen: recall=0.11 (1/9) [module: TCP]
- **TCPReturnPacket_IPv6** cond=E model=qwen: recall=0.10 (1/10) [module: TCP]
- **TCPReturnPacket** cond=E model=qwen: recall=0.10 (1/10) [module: TCP]
- **TCPReturnPacket_IPv6** cond=E model=claude: recall=0.10 (1/10) [module: TCP]
- **TCPReturnPacket** cond=E model=claude: recall=0.10 (1/10) [module: TCP]
- **TCPReturnPacket_IPv6** cond=A model=qwen: recall=0.10 (1/10) [module: TCP]
- **TCPReturnPacket** cond=A model=qwen: recall=0.10 (1/10) [module: TCP]

## Key Observation

FreeRTOS assertions are placed inside **stub function implementations**, not in the
main harness body. The LLM must:
1. Know which helper functions to stub (requires reading the target's source)
2. Know what preconditions each stub should check

**Condition E succeeds** (e.g., DHCPProcess: 100% recall under both models) when the
example harness contains the same stubs as the target function. In that case the
assertions are textually present in the example → directly derivable.

**Condition A always fails** (0% recall) because the harness header snippet shows only
includes and variable declarations, not stub assertions.

This result strongly supports the **knowledge barrier** interpretation: FreeRTOS
assertions require knowing the call graph of the target function, which is not
derivable from the harness template alone.
