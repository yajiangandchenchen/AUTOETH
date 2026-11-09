# Supplied public CodecResult andschema field contract

The direct purecodec APIs returnthe following complete fixed-fieldvalue. Itisnotanopaque resultwhosefields mustbe guessed.

| Field | Native type | Meaning |
|---|---|---|
| code | CodecCode withuint16_trepresentation |exactlocalenumerationbelow |
| byteOffset | uint64_t |absoluteoffsetundertheprovidedLayoutContext;notanunqualifiednativepointer |
| nodeId | uint32_t |localNodeId oftherelevant compiledschema node;0onlywhen noapplicablenodecanbeidentified |
| dataId | uint16_t |relevant12-bitTLVDataIDwhenapplicable;qualifywithcode/node/context,notanE2EDataID |

CodecCode:OK0,NullRange1,InvalidSchema2,ValueType3,ValueRange4,InvalidText5,LengthOverflow6,ParentBounds7,DuplicateKnownDataId8,KnownWireType9,MissingRequired10,UnknownUnionSelector11,OutputCapacity12,Resource13. Thesenumbersarelocalcodecvalues,notSOME/IPwireReturnCodeortransformerbytes.

RuntimeStatus isdeliberatelysmaller:moduleCodec andthedeclaredlocalPayloadMalformed/InvalidConfigclass,detail=CodecCodeonly,byte_offset=CodecResult.byteOffset. NodeId/DataIDdonotfitbyimplicationintodetail32. Adirectcodec returnunconditionallycarriesthefixedCodecResult;aruntime diagnosticrecordmaycarrythefullresultonlyifitwasadmitted/stillretained. Ringloss/overwritecanremovethatdetailandimmediatefailurecorrelationisnotguaranteed. RuntimeSnapshotisanas-ofimmutablepublishedviewwithsequence/time,notahardfreshnessguarantee. NochangeinfullerrorreportingismisrepresentedbyasmallStatus.

## Public SchemaSpec and NodeSpec

ThebaseconfigurationdocumentalreadygivestheexactexternalJSONnode-referencegrammar. Thissectioncollectsthefixedfield/rangeconstraintssothereisnodependencyonprivate designIDs.

| Object/kind | Fixedpublicfields | Ranges/constraints |
|---|---|---|
| SchemaSpec | name,root,role,argumentSet,payloadByteOrder,tlvStaticLengthBytes,dynamicTaggedLength,defaultAlignmentBits,nodes |root/nonzeronodeIDsuint32,uniqueperSchemaSpec;roleevent/field/methodRequest/methodResponse;Big/Little;staticlength0/1/2/4;alignment8/16/32/64/128/256;acyclicclosedreferences |
| NodeSpeccommon | id,kind |idnonzerouint32;kindBool/Unsigned/Signed/Float32Bits/Float64Bits/Struct/Array/ByteArray/String/Union;onlykind-applicablekeys |
| Primitive | widthBits |Bool8;integer8/16/32/64;floatbits32/64matchingkind |
| Struct | members,lengthBytes,tagged |orderedmembers;length0/1/2/4;directmemberDataIDsallpresentorallabsent,0..FFFanduniqueatthatdirectscope |
| MemberSpec | name,node,optional,optionaldataId,optionalreceptionDefault |nonemptyuniqueexactname/reference;NodeIduint32;optionalmembersrequiretagging;complete typeddefaultonlyforwholemissingdata |
| Array | element,fixedCountormaxReceiveCount,lengthBytes,optional missingElementDefaults |exactlyfixedordynamic,notboth;dimensionsarenestednodes;dynamiclength1/2/4;fixedmissingdefaultsmustbecompleteandtyped |
| ByteArray | fixedCountor maxReceiveCount,lengthBytes |arbitrarybinary;thedeclaredarraycount/lengthdomain,notterminatedtext |
| String | encoding,lengthBytes,maxWireBytes,optionalfixedWireBytes/maxUnicodeScalars |UTF8/UTF16;length0/1/2/4;max/fixedcountBOM+content+terminator+paddingbytesexcludingprefix;fixed<=max;UTF16fixedsendeven;emptywireminimum4B;strictscalars/contentNULrulesinthebasecontract |
| Union | selectorWidthBytes,variants,lengthBytes,optionalnonemptyPayloadExtentBytes |selectorwidth1/2/4;uniqueconfigurednonzeroselectors,maptochildnodes;0empty;length0/1/2/4;equalnonemptylengthsandunambiguousNULLlocationwhenprefixabsent |
| MethodrootargumentSet | argumentSet=true,rootStruct,lengthBytes0 |methodrolesonly;actualorderedargumentset,noaddedrootprefix;onestructargumentisachildnodeandkeepsitsownlengthrules |

Member/defaultnumbersareexacttypedvalues:unsignedandsignedwidthrangeschecked,IEEEbithexwidthexact,TextstrictUnicode,ByteArrayevenhex,Unionselector/valuepairconsistent. A schema references source C API/configfield names onlythroughitsdeclaredprojection;itdoesnotaddmembers tooriginalAUTOSARtypes. TLVcomplexstaticwidthinheritance,wiretype5/6/7receivewidths,namespace,doubles/unknowns,requiredmissing/default,parentboundsandoddUTF16extentsemanticsremainasinthebasecustomercontract. Node/length/countresourcesapplybeforeallocation/recursion.

C++facade/compiler/containerconstructionnotexplicitlylistedasanaggregatefield remainsimplementationfreedom;allfixedfields,codevalues,externalgrammar,units,returnsandobservableatomicityarefixed. Noadditionalgenericcompatibilitylayer,allocatorfeature,workfloworprivatecase/oracleisinthisaddendum.


## Exact result location and Struct interpretation

For a successful direct CodecResult, byteOffset/nodeId/dataId are 0/0/0 as a no-error-location sentinel. The absolute-coordinate and identifiable-node descriptions apply to errors with locations. Use the full fourteen-code table and all eight signature scopes in 14_校验错误定位规则.md. No field/code width or value changes.

Struct vectors follow MemberSpec declaration order and member_ids[i]=MemberSpec.node, including repeated nodes at independent positions. A complete allowed required receptionDefault is a present logical value; an absent optional remains absent. The exact contract is 17_结构体成员身份与默认值.md. Signed values use the current explicit two's-complement rule without changing widths or host conversion safety.
