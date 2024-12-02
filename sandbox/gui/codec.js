import { encode, decode } from "@msgpack/msgpack";

/**
 * @template T
 * @type {{ encode: <T>(o: T) => Uint8Array; decode: <T>(o: Uint8Array) => T; }}
 */
export const msgpack_codec = {
    encode: o => encode(o),
    decode: o => decode(o)
};