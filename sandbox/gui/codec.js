// Copyright (c) 2026, Neil Aldea <njaldea@gmail.com>
// SPDX-License-Identifier: BSL-1.0
// See the repository LICENSE and https://www.boost.org/LICENSE_1_0.txt.

import { encode, decode } from "@msgpack/msgpack";

/**
 * @type {import("@nil-/xit").CoDec<any>}
 */
export const msgpack_codec = {
    encode: o => encode(o),
    decode: o => decode(o)
};
