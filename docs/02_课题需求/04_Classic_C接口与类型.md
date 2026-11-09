# SUPPLIED CUSTOMER INTERFACE: original C types andfinite host mapping

Originals control;theexactadaptationsandhostimportsareexplicit.Thistext isnotcompiledABIevidence.

## customer contract 头文件与接口隔离

C兼容头分别为Platform_Types.h、Std_Types.h、ComStack_Cfg.h、ComStack_Types.h及具体模块头。Std_Types.h包含Platform_Types.h；ComStack_Types.h通过Std_Types.h取得基础类型，并包含本项目明确的ComStack_Cfg.h。标准类型头不增加项目扩展；主机BuildInfo、错误域、长度/容量span和拥有型对象放在独立autoeth公共头中。

模块自己的实现包含自己的公共头；只引入实际需要的其他模块头。公开头有唯一include guard，不使用仅大小写不同的文件名。用户必须能够从类型/API表确定头文件、单位、合法值和调用条件。C函数声明提供C++下的C linkage，但不把C++模板、vector或异常类型暴露为C参数。

## customer contract 固定基础类型与布尔

| 源类型 | 主机合同 | 线数据/合法性边界 |
|---|---|---|
| uint8/16/32/64 | 恰好8/16/32/64位无符号，取自相应固定宽度实现类型 | 运算及窄化前先检查逻辑边界；不依赖宿主long宽度。 |
| sint8/16/32/64 | 对应固定宽度二补码有符号类型 | 最小/最大值、字节序及非法窄化单独验收；不能用原生结构体内存布局做线序列化。 |
| uint8/16/32_least、sint8/16/32_least | 至少指定宽度；只允许模块内部使用 | 不进入公共API参数或线字段，不能把least类型的额外位宽当作源计数器模域。 |
| float32/float64 | IEEE 754 binary32/binary64；实际主机表示在B0检查 | 按位序列化使用安全字节复制和显式端序；不经别名不相容指针读取。 |
| boolean | C99为_Bool；C++14为bool，使用TRUE/FALSE语言布尔常量 | 不假定AUTOSAR规定所有平台sizeof(boolean)=1；实际GCC布局及跨语言调用是未来证据。 |
| VoidPtr/ConstVoidPtr | void* / const void* | 只有调用方保证有效对象、容量与生命周期才可访问。 |
| NULL_PTR | approved customer contract：C99保持((void*)0)，C++14公共头条件分支为nullptr | 明示语言适配；不宣称表达式/任意ECU ABI相同，独立跨语言消费者须验证。 |

PlatformTypes第17页限制布尔赋值/运算形式。先在uint8上取得线字节最低位，再用比较产生布尔值；不对bool对象做位运算，不把任意0x02/0x03原始表示memcpy进bool。SOME/IP线0x02最低位0，解码FALSE；0x03最低位1，解码TRUE。C语言将整数实参转换为_Bool的规则不替代这条线格式规则。

浮点源FLOAT*_MIN是最小正正规值这一合同，不能与负最大值或最小次正规数混淆。应用是否限制NaN/Inf/物理范围由具体服务schema声明，不能在基础线编解码层暗加全局限制。

## customer contract E2E枚举种类与明确主机表示

源的“Kind Enumeration”没有为以下类型规定底层对象大小。本项目按Q009为它们选uint8 typedef及源命名常量：

E2E_P01CheckStatusType、E2E_P02CheckStatusType、E2E_P04CheckStatusType、E2E_P04mCheckStatusType、E2E_P05CheckStatusType、E2E_P06CheckStatusType、E2E_P07CheckStatusType、E2E_P07mCheckStatusType、E2E_P08CheckStatusType、E2E_P08mCheckStatusType、E2E_P11CheckStatusType、E2E_P22CheckStatusType、E2E_P44CheckStatusType、E2E_P44mCheckStatusType、E2E_PCheckStatusType、E2E_SMStateType。

字段顺序、精确源类型名、固定整数宽度、形参位置和数值保持原合同；这不是对其他枚举的一般ABI改写。Native强类型在边界显式转换，未知0xFF可作为确定的主机输入按Q007处理，不依赖宿主C enum的int大小或C++非固定底层枚举的非法值行为。

| 域 | 主要源符号及值 | 使用规则 |
|---|---|---|
| 调用返回Std_ReturnType | E2E_E_OK=0x00；E2E_E_INPUTERR_NULL=0x13；E2E_E_INPUTERR_WRONG=0x17；其他源错误保持 | 不用raw Status替代；非OK调用映到generic ERROR。 |
| 普通raw Profile Status | OK=0x00；NONEWDATA=0x01；ERROR=0x07；REPEATED=0x08；OKSOMELOST=0x20；WRONGSEQUENCE=0x40 | 只对实际源中定义的该Profile常量适用；源OKSOMELOST映generic OK。 |
| 旧01/02额外raw Status | WRONGCRC=0x02；SYNC=0x03；INITIAL=0x04 | R4_2/PRE_R4_2映射有差异，保留真实首次检查规则。 |
| generic Status | E2E_P_OK=0x00；REPEATED=0x01；WRONGSEQUENCE=0x02；ERROR=0x03；NOTAVAILABLE=0x04；NONEWDATA=0x05 | NOTAVAILABLE用于窗口初始化；非法/保留SM输入按the public customer contract拒绝；不是把raw值数值直转。 |
| SMState | VALID=0x00；DEINIT=0x01；NODATA=0x02；INIT=0x03；INVALID=0x04 | 历史窗口状态，不能替代当前数据检查。 |
| 标准方法类型/结果 | REQUEST=0x00，RESPONSE=0x01；OK=0x00，ERROR=0x01 | 均为源Std类型的uint8域；与线RESPONSE0x80/ERROR0x81分开。 |

未知raw Map输入得到generic E2E_P_ERROR=0x03；未知/保留generic SM输入返回E2E_E_INPUTERR_WRONG=0x17且状态不变，精确优先级见the public customer contract。原始CRC的有效输入/无DET前提不因这些主机检查而改变。

SoAd、Sd、ComStack等其他枚举在各自接口表中分别明确；本项不默认为它们选择同一uint8表示，也不自动承诺任意0xFF输入。它们的真实调用方对象与源前提仍需检查。

## customer contract ComStack和版本结构

本项目ComStack_Cfg.h固定PduIdType为uint16、PduLengthType为uint32，作为明确的主机配置，不声称存在ARXML生成器。PDU长度单位始终是字节。窄源接口（例如SoAd_RxIndication的uint16 Length、04/44系列的运行时Length）在其边界检查，不能因全局长度类型较宽而省略。

PduInfoType字段顺序为SduDataPtr:uint8*、MetaDataPtr:uint8*、SduLength:PduLengthType。元数据布局、长度和已知项按配置；多字节元数据使用平台端序，不能误当成网络端序。生产方分配完整所需元数据，只初始化已知项；转发时未知项保持，不将借用指针留到回调之后。

BufReq_ReturnType源常量为BUFREQ_OK=0x00、BUFREQ_E_NOT_OK=0x01、BUFREQ_E_BUSY=0x02、BUFREQ_E_OVFL=0x03。它与Std_ReturnType是不同语义域。TP状态/RetryInfoType、重试或零长度查询的具体合同在网络适配表里逐项说明，不从通用NULL规则推断允许情况。

Std_VersionInfoType字段顺序：vendorID:uint16、moduleID:uint16、sw_major_version:uint8、sw_minor_version:uint8、sw_patch_version:uint8。GetVersionInfo同步且可重入，所选版本信息接口在未初始化时的允许调用与空输出处理按具体模块合同及源条件执行。

## customer contract 发布身份、模块ID与未分配厂商

ReleaseOverview R24-11第12页将平台R24-11映到内部R4.10.0。BSW版本宏按major/minor/revision为0x04/0x0A/0x00；项目软件版本独立为0.1.0，即0x00/0x01/0x00。模块间检查比较相应标准发布主/次版本，不拿项目版本或文档ID替代。

| 模块 | 源模块ID（十进制） | 明确十六进制值 |
|---|---:|---:|
| Crc | 201 | 0x00C9 |
| E2E | 207 | 0x00CF |
| E2EXf | 176 | 0x00B0 |
| SomeIpXf | 174 | 0x00AE |
| SomeIpTp | 177 | 0x00B1 |
| Sd | 171 | 0x00AB |
| SoAd | 56 | 0x0038 |
| PduR | 51 | 0x0033 |

依据为BSWGeneral第95–98页原表。库原件文档ID（例如428/416）不是模块ID。

当前没有项目获分配AUTOSAR Vendor ID的事实。C主机版本接口使用0x0000作为明确的LOCAL_UNASSIGNED标记，Native BuildInfo同时报告vendor_id_assigned=false；这不是注册厂商标识或厂商一致性PASS。后续正式分配时在约定配置点替换，不借用第三方实现编号。原始规范和项目元信息保持各自身份。

## customer contract Imported types and configuration storage

The base C99/C++14 types, PduInfo order, Std_VersionInfoType and 16 narrow E2E status types are customer contract and ../04_接口数据表/标准C接口/类型成员.tsv. PduIdType is uint16 and PduLengthType uint32 in this host configuration. Configured PDU IDs are zero-based dense within their own entrypoint namespace; TX, RX, lower-confirmation, upper-destination and SoCon IDs are not interchangeable.

SoAd_SoConIdType,SoAd_RoutingGroupIdType,PduR_RoutingPathGroupIdType,PduR_PBConfigIdType and Sd_ServiceGroupIdType are uint16 in this host. Source configurable8/16 alternatives are preserved as source variation, not forced into a common byte representation. Source unnumbered SoAd_SoConModeType is a C enum with LOCAL assignments ONLINE0,RECONNECT1,OFFLINE2; C++ uses the measured-compatible fixed unsigned-int representation from customer contract. Numbered Sd requested/current values retain the source tables in customer contract.

The following TcpIp names are explicitly host imports, not a declaration of the unread/full TcpIp binary ABI: TcpIp_SocketIdType:uint32 logical adapter ID (never a naked reusable fd);TcpIp_LocalAddrIdType:uint16;TcpIp_DomainType:uint16 with host symbols TCPIP_AF_INET2/TCPIP_AF_INET6=28;TcpIp_ReturnType enum TCPIP_E_OK0/TCPIP_E_NOT_OK1/TCPIP_E_PENDING2;TcpIp_IpAddrStateType enum ASSIGNED0/ONHOLD1/UNASSIGNED2. TcpIp_EventType has host values TCP_RESET0,TCP_CLOSED1,TCP_FIN_RECEIVED2,UDP_CLOSED3; unselected TLS events are not accepted as a configured feature. This numeric imported mapping is LOCAL, not inferred from POSIX constants.

Host TcpIp_SockAddrType stores, in order,domain:uint16,port:uint16,addr:uint8[16],scope_id:uint32. Domain is set by callers; IPv4 occupies the first4 address bytes with the rest0. Port is a host numeric value; wire conversion is explicit. The actual struct capacity must fit this host type; no derived-original-address ABI is claimed. Address assignment/link/remote readiness is observed from this runtime's real adapter/provider.

The source SoAd_ConfigType,SomeIpTp_ConfigType,Sd_ConfigType and PduR_PBConfigType permit implementation-specific contents. This host uses these exact aggregates, whose pointer fields and referenced callback arrays remain valid/read-only until module uninitialization:

| C aggregate | Ordered host fields |
|---|---|
| SoAd_ConfigType | schemaVersion:uint16=1;soConCount:uint32;const AeSoConConfig* soCons;txRouteCount:uint32;const AeSoAdTxRoute* txRoutes;rxRouteCount:uint32;const AeSoAdRxRoute* rxRoutes;routingGroupCount:uint32;const AeRoutingGroup* routingGroups |
| PduR_PBConfigType | configurationId:PduR_PBConfigIdType;pathCount:uint32;const AePduRoute* paths;groupCount:uint32;const AeRoutingGroup* groups |
| SomeIpTp_ConfigType | txPduCount:uint32;const AeTpTxPdu* txPdus;rxPduCount:uint32;const AeTpRxPdu* rxPdus;mainPeriodNs:uint64 |
| Sd_ConfigType | serverCount:uint32;const AeSdServerRef* servers;clientCount:uint32;const AeSdClientRef* clients;consumedGroupCount:uint32;const AeSdConsumedGroupRef* consumedGroups;serviceGroupCount:uint32;const AeSdServiceGroup* serviceGroups;AeSdCurrentCallbacks currentCallbacks |
| SomeIpXf_ConfigType / E2EXf_ConfigType | bindingCount:uint32;const AeTransformerBinding* bindings; references the same immutable configured service/schema/channel descriptions, not generated alternate algorithms |

AeSoConConfig ordered fields: id:SoAd_SoConIdType,group:uint16,nativeEndpointIndex:uint32,automaticSetup:boolean,acceptanceFilter:boolean,pduHeaderEnabled:boolean,tcpImmediateTpConfirmation:boolean,configuredRemote:TcpIp_SockAddrType,remoteIsSet:boolean,remoteIpWildcard:boolean,remotePortWildcard:boolean. Selected C routes require pduHeaderEnabled=TRUE and TCP TP immediate=TRUE; TCP IF,header-disabled stream,ACK-dependent confirmation and general nPdu collection modes reject configuration. Native ordinary endpoint framing remains full16B SOME/IP, with its real FIFO packing.

AeSoAdTxRoute fields: sourcePdu:PduIdType,destinationSoCon:SoAd_SoConIdType,headerId:uint32,upperConfirmationPdu:PduIdType,upperType:{IF,TP},triggerTransmit:boolean,metadataBytes:uint16,const AeUpperCallbacks* callbacks,groupRefCount:uint16,const uint16* groupRefs. AeSoAdRxRoute fields: sourceSoCon,headerId:uint32,destinationPdu:PduIdType,upperType,metadataBytes,callbacks,groupRefCount,groupRefs. Group reference values are indexes in their own group table. All counts/nonnull count-pointer pairs/references/types are checked as a whole before Init publishes initialized state; existing state is not partially replaced on invalid configuration.

AeRoutingGroup fields: id:uint16,initialEnabled:boolean,txTriggerable:boolean. AePduRoute fields: sourceApi:AePduApi enum,sourcePdu:PduIdType,destinationApi:AePduApi,destinationPdu:PduIdType,confirmationPdu:PduIdType,metadataBytes:uint16,groupRefCount:uint16,const uint16* groupRefs,const AeUpperCallbacks* upper. AePduApi is a host enum AutoEth0,SomeIpTp1,SoAdIf2,SoAdTp3. Each path has exactly one destination. A route is enabled if its ungrouped default is enabled or at least one referenced routing group is enabled, as the source applicable group rules prescribe; request reference counts are distinct from duplicate incoming packets.

AeTpTxPdu fields: sourcePdu:PduIdType,lowerPdu:PduIdType,confirmationPdu:PduIdType,maxSegmentPayloadBytes:uint32,burstCount:uint16,minimumSeparationNs:uint64. AeTpRxPdu fields: lowerPdu:PduIdType,upperPdu:PduIdType,maxOriginalBytes:uint32,reassemblyTimeoutNs:uint64. Source ECUC duration values are seconds; these implementation-specific host fields are checked integer nanoseconds, with exact conversion at import, no floating intermediate. The C SomeIpTp module is CP_STREAMING; FO_CONTIGUOUS is a separate explicitly named Native strategy.

AeSdServerRef fields: handleId:uint16,nativeServiceIndex:uint32,initialRequested:Sd_ServerServiceSetStateType. AeSdClientRef:handleId:uint16,nativeQueryIndex:uint32,initialRequested:Sd_ClientServiceSetStateType. AeSdConsumedGroupRef:handleId:uint16,nativeClientHandle:uint16,nativeEventGroupId:uint16,initialRequested:Sd_ConsumedEventGroupSetStateType. AeSdServiceGroup:handleId:uint16,serverRefCount:uint16,const uint16* serverRefs,clientRefCount:uint16,const uint16* clientRefs. C arrays bind actual Native immutable descriptions/timers, not a second SD engine; their full values/defaults are in customer contract and configuration examples. Sd00121 and SoAd00211 require retaining access to ConfigPtr; Native ownership does not erase that caller lifetime.


AeTransformerBinding ordered fields are bindingId:uint16,nativeServiceIndex:uint32,elementId:uint16,role:AeTransformerRole,schemaIndex:uint32,e2eChannelIndex:uint32,inPlace:boolean,disableCheck:boolean,disableStateMachine:boolean,executeDespiteNoData:boolean,upperHeaderBits:uint16. AeTransformerRole host values are Sender0,Receiver1,ClientRequest2,ServerRequest3,ServerResponse4,ClientResponse5. Indices reference the bound immutable Runtime descriptors;the configured finite IDs/roles/in-place form must exactly match the exported symbol,source profile/header length andschema role. Original source config remains implementation-specific;no ARXML generator is implied.

ClassicHostConfig in autoeth/classic_host.hpp has ordered const pointers pdur:PduR_PBConfigType*,soad:SoAd_ConfigType*,someip_tp:SomeIpTp_ConfigType*,sd:Sd_ConfigType*,someip_xf:SomeIpXf_ConfigType*,e2e_xf:E2EXf_ConfigType*,then DiagnosticObserver observer. All point to live read-only caller-owned C aggregates;the binding owns internal validated identity maps butdoesnotreplace the original ConfigPtr addresses. A NULL module config is allowed only when its entire declared compatibility route is unselected;the selected full C acceptance configuration supplies allsix. DiagnosticObserver receives an immutable owned diagnostic after internal commit,without core CRC/E2E callbacks.

## customer contract Callback signatures, borrow and error results

AeUpperCallbacks contains function pointers in the following order; each optional entry is NULL only when its configured route does not use it. Required route callbacks are checked before Init:

- void (*IfRxIndication)(PduIdType,const PduInfoType*);
- void (*IfTxConfirmation)(PduIdType,Std_ReturnType);
- Std_ReturnType (*TriggerTransmit)(PduIdType,PduInfoType*);
- BufReq_ReturnType (*StartOfReception)(PduIdType,const PduInfoType*,PduLengthType,PduLengthType*);
- BufReq_ReturnType (*CopyRxData)(PduIdType,const PduInfoType*,PduLengthType*);
- void (*TpRxIndication)(PduIdType,Std_ReturnType);
- BufReq_ReturnType (*CopyTxData)(PduIdType,const PduInfoType*,const RetryInfoType*,PduLengthType*);
- void (*TpTxConfirmation)(PduIdType,Std_ReturnType);
- void (*SoConModeChg)(SoAd_SoConIdType,SoAd_SoConModeType);
- void (*LocalIpAddrAssignmentChg)(SoAd_SoConIdType,TcpIp_IpAddrStateType).

AeSdCurrentCallbacks has ClientServiceCurrentState(uint16,Sd_ClientServiceCurrentStateType),ConsumedEventGroupCurrentState(uint16,Sd_ConsumedEventGroupCurrentStateType),EventHandlerCurrentState(uint16,Sd_EventHandlerCurrentStateType),all returning void and called only after a real current-state change. Requested SetState success is not a current AVAILABLE callback.

RetryInfoType retains ComStack table order TpDataState:TpDataStateType,TxTpDataCnt:PduLengthType. Host ordinary enum TP_DATACONF0/TP_DATARETRY1/TP_CONFPENDING2 is separately checked against the imported source table; it is not an E2E status. CopyTxData retry NULL means ordinary forward copying; TP_DATARETRY repositions only within actually retained unconfirmed bytes and validates TxTpDataCnt before copying. A0-byte Copy query permits NULL SduDataPtr and returns available capacity without advancing the copy cursor. A present data copy requires the real requested range. CopyRxData0 similarly queries free receive storage. StartOfReception may have NULL info when its source conditions permit;required bufferSizePtr remains live, TpSduLength and any initial info bytes are independently bounded. BUFREQ_OK0,NOT_OK1,BUSY2,OVFL3 remain distinct and are forwarded without casting to Std_ReturnType.

Successful IF/TP data transfer callbacks consume only call-lifetime PduInfo and metadata. Synchronous1:1 forwarding converts the configured PDU ID and passes byte content/known and unknown metadata unchanged; asynchronous Native consumption copies before returning. A callback cannot retain an upper pointer after its borrow or free another owner's memory. Failure/terminal notifications occur once for that accepted transfer. No callbacks are made for a rejected unadmitted request.

Host diagnostics preserve source module/API/error identifiers and runtime/development category in the owned diagnostic ring and an optional real configured observer. No empty Det_ReportError/ReportRuntimeError success stub or fabricated ECU DET/DEM service is installed. The pure CRC/E2E core keeps its no-DET contract. Full external error-stack conformance is outside this limited host mapping.

## customer contract Socket, address, route and transfer state transitions

| Current/event/guard | Action and output |
|---|---|
| Init valid | store ConfigPtr,initialize SoCons OFFLINE,route groups/config state; invalid data leaves module uninitialized with diagnostic |
| Open request | checked requested-open reference count; E_OK is accepted request; Main and actual bind/connect readiness produce RECONNECT/ONLINE |
| Close abortFALSE | count matching Open/Close references; source00604 closes when no requester holds it; Main terminates active TP and disables routes |
| Close abortTRUE/link/socket failure | invalidate old write/read capability, Main closes affected socket/generation and completes accepted work once; customer contract handles Native partial frames/siblings |
| SetRemote locked | E_NOT_OK,remote/state unchanged; no hidden delayed successful assignment |
| SetRemote valid/unlocked | set actual address; address-family mismatch rejects; setting ONLINE address with wildcards causes RECONNECT (00687) |
| SetUniqueRemote | existing exact address returns its SoCon; otherwise choose an unused same-group wildcard entry by00680 best match; none/domain error leaves assigned output unchanged |
| ReleaseRemote locked | record pending release; Main executes after lock is gone; no immediate address mutation |
| ReleaseRemote unlocked | restore configured remote setting,not blindly zero; mode transitions follow resulting wildcard/readiness |
| GetRemote unset/domain mismatch | E_NOT_OK and all outputs unchanged; valid success reports actual address,not merely configured template |
| GetLocal missing actual prefix/router facts | E_NOT_OK,all outputs unchanged; complete real facts/domain/capacity produce one successful output commit |
| Routing Enable/Disable/Specific | change only addressed group/SoCon reference; OR relation means another enabled reference can retain the route; invalid IDs change nothing |
| GroupTransmit accepted | store bounded trigger for Main; retrieve newest PDU using TriggerTransmit and actual capacity then transmit each selected enabled IF route; callback failures remain per-route diagnostics,not a false global delivery result |
| IF same-socket overlap |00653 returns E_NOT_OK while a transfer is ongoing; IF ongoing lasts to IfTransmit return, TP to its confirmation |
| TP same-socket overlap or zero SduLength |00650/00551 reject E_NOT_OK,no TxConfirmation; accepted TP enters pending query/copy/send state |
| Accepted TP Main | query CopyTxData with0; obtain real available bytes; UDP copies through its whole TP buffer; selected TCP full provider retrieves/owns all immutable bytes through the real adapter before its early acceptance |
| UDP IF/TP backend accepts whole owned datagram | source00544/00557 successful host stack acceptance,then one E_OK confirmation in prescribed Main; actual POSIX kernel progress remains an independent Native observation |
| TCP TP ImmediateTRUE backend accepts owned full PDU | source00667 one E_OK confirmation in Main; it does not manufacture TCP ACK or imply peer execution |
| TCP adapter rejects accepted TP |00670 one E_NOT_OK in Main; pre-admission TpTransmit rejection produces no confirmation |
| UDP CopyTxData NOT_OK |00652 terminate session,one E_NOT_OK confirmation,keep UDP socket |
| TCP CopyTxData NOT_OK |00651 disable sends/Rx,close next Main; accepted pending results terminal once |
| Active TpCancelTx/Rx |00575/00577/00581 store cancel and close in Main; no active session00576 rejects; distinct from Native WAITING business cancel |
| Actual Rx/TcpAccepted/Connected/IP/Event | validate logical socket/SoCon generation,scope/address/domain/route and required pointers,own ingress before return; no recursive transaction dispatch inside send/copy |
| Source TxConfirmation callback | records only its actual adapter event; selected ImmediateTRUE cannot produce a second opposite upper confirmation after prior success |

SoAd00680 selection order is exact IP+port,IP+port-wildcard,port+IP-wildcard,both wildcards,no match. An entry with no configured specific-or-wildcard remote is ignored. Equal-specificity remaining candidates use stable configured SoCon order as a LOCAL deterministic tie rule. AcceptanceFilterTRUE additionally requires the received source to match both specific-or-wildcard address components; unset remote accepts nothing. Source00524/525 are not replaced by a generic “open means accept all” condition.

GetSoConMode is a void getter, ReleaseRemoteAddr is void, IsConnectionReady returns TcpIp_ReturnType with the requested RemoteAddrPtr. Readiness uses real selected adapter/provider conditions; a controlled test provider proves gating only,not cryptography/ARP. SoAd_CopyTxData's uint16 BufLength and RxIndication's uint16 Length are chunk boundaries; Native large frames are delivered in checked chunks and keep their full length elsewhere.

## customer contract Configured SOME/IP and E2E transformer bindings

No generator or full RTE is selected. Concrete project serializers are handwritten schema adapters calling the same production codec. Rte_Cs_TransactionHandleType is an explicitly imported host aggregate with source-mentioned fields clientId:uint16,sequenceCounter:uint16, not a full RTE ABI. E2EXf_CSTransactionHandleType remains the original two uint32 fields e2eCounter,e2eSourceId in that order.

The C service DTOs are own project types: AeVehicleSnapshot{revision:uint32,speedCenti:uint16,temperatureDeci:sint16,flags:uint8,label:uint8[72],labelUnits:uint32};AeCalibration{speedLimitCenti:uint16,gainMilli:uint16,label:uint8[72],labelUnits:uint32};AeCalibrationState{revision:uint32,value:AeCalibration}. Label capacity is72 uint8 units;valid labelUnits includes supplied BOM/terminator and follows customer contract normalization. Wire fields follow the declared schema,never these structs' memory padding or labelUnits metadata. A C label output is committed with its companion valid-unit count only after complete successful decode; capacity failure is local0x81,not bad wire.

Selected serializer IDs are VehicleEvent,CalibrationRequest,CalibrationResponse and their inverse. Event data has no RTE transaction argument; CS forms have it first. CalibrationResponse additionally has Std_ReturnType returnValue / Std_ReturnType* returnValue before its one data parameter because its configured operation declares possible errors. The exact concrete signatures are in the single API TSV. ExtractProtocolHeaderFields keeps its source76 signature and customer contract's narrow check/output policy. Init/DeInit/GetVersionInfo use their original forms. Source SomeIpXf return bytes00/01/81/87/88/89/8A remain separate from local Status/wire codes.

Selected E2EXf IDs cover these same three bindings,each with out-of-place and separate InPlace symbol suffix. ForwardedCode is absent because Forward is NOT_SELECTED. VehicleEvent uses original P04;Calibration request/response P04m. Method symbols include the source extraction function-pointer and csTransactionHandle parameters in source order; server metadata is required when the role consumes it,client-unused server-handle input is not treated as a new source identity. A valid server SinkCheck snapshot is separate from an old handle or published SourceID alone.

C transformer calls occur in the bound Runtime's actual configured invocation context,which owns the exact connection/service/method/peer and transaction snapshot. The8B tail supplies Client/Session; ordinary RPC request-counter echo/check uses that real registered transaction,not a shared “last request”. A standalone C call with no required bound RPC context returns hard runtime0xFF with output/length/handle unchanged. Event-channel operations require their configured initialized channel but no RPC context. This limited host mapping is explicit and cannot be advertised as standalone full RTE execution.

Raw C serialization buffers have the original caller's valid storage precondition: capacity at least the configured schema/binding maximum and the reserved E2E prefix in in-place mode. Their pointer/length signature cannot probe an arbitrary allocation. Native MutableByteView operations provide checked actual-capacity failures. No undersized raw C allocation is falsely promised safe. The current-message Check/SM side effects persist after a later type decode failure,while application output/length/handle remain atomic per 02_业务行为与状态约束.md/customer contract. DeInit invalidates only its configured channels after quiescence; neither it nor Init silently cancels unrelated Runtime operations.


The complete public ResourceConfig/SchedulingConfig/CorrelationConfig/CaptureConfig tables are in 06_配置字段与默认值.md. The fixed CodecResult/CodecCode andSchemaSpec/NodeSpec field/range contract is in 07_结果类型与Schema.md.
