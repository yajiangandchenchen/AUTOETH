# Shared whole-root Array and Union boundary contract

These rules refine the existing payload codec without adding a node key, public field, enum or API. Keep the original source matrix's X, blank, N/A, direction and guards. A blank does not require every concrete value to reject.

## Whole-root fixed Array without a length prefix

This rule applies only when the entire bounded ByteView passed to decode_payload is the actual extent of that root Array, and its elements have a known positive fixed wire width. Check complete element boundaries against that explicit extent first.

Interpret the expected complete elements. Skip complete extras. If complete required positions are absent and no complete legal missingElementDefault is available, return MissingRequired10 at that missing element node, parentEnd and DataID zero. Existing complete defaults remain available under their actual conditions. A partial element must never be filled by splicing default bytes; it follows the existing parent-bound failure rule.

Successful consumed includes the entire bounded root extent, including legitimate complete extras. Do not apply this to an embedded prefix-free Array followed by other fields without an independent array extent. It must not consume the enclosing parent's remaining bytes as though no following field existed. This clarification does not extend unknown variable-width element boundary policy.

## Union extent and selector priority

First prove the outer parent and complete length prefix can be located safely. When a prefix exists, prove the complete current Union extent lies in that parent before interpreting the selector.

For an ordinary Union:

`totalExtent = lengthWidth + selectorWidth + declaredDataLength`

The length covers data and padding, excluding selector and its own prefix. A tagged Union has only its one outer length; that declared extent includes the selector. Do not add a second prefix.

Next prove the selector is complete. Then classify it as zero, configured or unknown. Only then interpret the selected variant. A complete unknown nonzero selector returns UnknownUnionSelector11 at its selector start, the Union node and the actual current DataID. Do not assume it was a legal variant first and report its hypothetical missing payload.

A prefix-free whole-root Union uses its explicit root ByteView extent. Do not force nonemptyPayloadExtentBytes before reading the selector, because selector zero permits truly zero data. An embedded prefix-free Union still follows its declared selector/fixed-variant layout and cannot treat subsequent fields as NULL data.

## NULL with data

When a complete selector zero has a nonzero current exact Union data extent, use existing ValueRange4. Locate the first forbidden data byte, immediately after the selector, with that Union node and actual current DataID, or zero where no tag applies.

This is a NULL-data constraint, not UnknownUnionSelector. It does not replace the general configured-maximum or prefix-bounds rules. Establish a prefixed Union's child range first; perform its own value checks before the enclosing layer's later leftover-tail judgement. Do not steal outer bytes into the Union.

All coordinate arithmetic remains checked. Failure preserves Value, consumed, buffers and the existing side-effect sentinels. Success retains the no-error-location 0/0/0 sentinel. This shared policy contains no private case identities, test bytes or golden outputs.
