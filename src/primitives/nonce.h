// Copyright (c) 2018 Michael Toutonghi
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PRIMITIVES_NONCE_H
#define BITCOIN_PRIMITIVES_NONCE_H

#include "serialize.h"
#include "uint256.h"
#include "arith_uint256.h"


/** For POS blocks, the nNonce of a block header holds the entropy source for the POS contest
 * in the latest VerusHash protocol, implemented at block below
 * 
 * */
class CPOSNonce : public uint256
{
public:
    static bool NewPOSActive(int32_t height);
    static bool NewNonceActive(int32_t height);

    CPOSNonce() {}
    CPOSNonce(const base_blob<256> &b) : uint256(b) {}
    CPOSNonce(const std::vector<unsigned char> &vch) : uint256(vch) {}

    int32_t GetPOSTarget() const
    {
        uint32_t nBits = 0;

        for (const unsigned char *p = begin() + 3; p >= begin(); p--)
        {
            nBits <<= 8;
            nBits += *p;
        }
        return nBits;
    }

    bool IsPOSNonce() const
    {
        arith_uint256 arNonce = UintToArith256(*this);
        arith_uint256 tmpNonce = ((arNonce << 128) >> 128);
        CVerusHashWriter hashWriter = CVerusHashWriter(SER_GETHASH, PROTOCOL_VERSION);
        hashWriter << ArithToUint256(tmpNonce);
        return (*this == ArithToUint256(UintToArith256(hashWriter.GetHash()) << 128 | tmpNonce));
    }

    void SetPOSTarget(int32_t nBits)
    {
        CVerusHashWriter hashWriter = CVerusHashWriter(SER_GETHASH, PROTOCOL_VERSION);
        arith_uint256 tmpNonce;

        arith_uint256 arNonce = UintToArith256(*this);
        arNonce = ((arNonce >> 32) << 32) | nBits;

        tmpNonce = ((arNonce << 128) >> 128);
        hashWriter << ArithToUint256(tmpNonce);

        (uint256 &)(*this) = ArithToUint256(UintToArith256(hashWriter.GetHash()) << 128 | tmpNonce);
    }

    void SetPOSEntropy(const uint256 &pastHash, uint256 txid, int32_t voutNum);
    bool CheckPOSEntropy(const uint256 &pastHash, uint256 txid, int32_t voutNum);
};

#endif // BITCOIN_PRIMITIVES_NONCE_H
