/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string.h>
#include "primitives/transaction.h"
#include "pubkey.h"
#include "script/interpreter.h"
#include "script/script_error.h"
#include "version.h"

// Initialize libsecp256k1 context.
static auto secp256k1_context = ECCVerifyHandle();

// Helper class, not published. This is tested internal to verify_script.
class transaction_istream
{
public:
    template<typename Type>
    transaction_istream& operator>>(Type& instance)
    {
        ::Unserialize(*this, instance);
        return *this;
    }

    transaction_istream(const uint8_t* transaction, size_t size)
      : source_(transaction), remaining_(size)
    {
    }

    void read(char* destination, size_t size)
    {
        if (size > remaining_)
            throw std::ios_base::failure("end of data");

        memcpy(destination, source_, size);
        remaining_ -= size;
        source_ += size;
    }

    int GetType() const
    {
        return SER_NETWORK;
    }

    int GetVersion() const
    {
        return PROTOCOL_VERSION;
    }

private:
    size_t remaining_;
    const uint8_t* source_;
};

static const uint32_t MANDATORY_SCRIPT_VERIFY_FLAGS =
    SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_STRICTENC |
    SCRIPT_ENABLE_SIGHASH_FORKID | SCRIPT_VERIFY_LOW_S | SCRIPT_VERIFY_NULLFAIL;

static const uint32_t STANDARD_SCRIPT_VERIFY_FLAGS =
    MANDATORY_SCRIPT_VERIFY_FLAGS | SCRIPT_VERIFY_DERSIG | SCRIPT_VERIFY_LOW_S |
    SCRIPT_VERIFY_NULLDUMMY | SCRIPT_VERIFY_SIGPUSHONLY |
    SCRIPT_VERIFY_MINIMALDATA | SCRIPT_VERIFY_DISCOURAGE_UPGRADABLE_NOPS |
    SCRIPT_VERIFY_CLEANSTACK | SCRIPT_VERIFY_CHECKLOCKTIMEVERIFY |
    SCRIPT_VERIFY_CHECKSEQUENCEVERIFY | SCRIPT_VERIFY_NULLFAIL;


extern "C" {
    int verify_script(const unsigned char* transaction,
        size_t transaction_size, const unsigned char* prevout_script,
        size_t prevout_script_size, unsigned int tx_input_index,
        unsigned int flags, int64_t amount = 0)
    {
        if (amount > INT64_MAX)
            throw std::invalid_argument("value");

        if (transaction_size > 0 && transaction == NULL)
            throw std::invalid_argument("transaction");

        if (prevout_script_size > 0 && prevout_script == NULL)
            throw std::invalid_argument("prevout_script");

        std::shared_ptr<CTransaction> tx;

        try
        {
            transaction_istream stream(transaction, transaction_size);
            tx = std::make_shared<CTransaction>(deserialize, stream);
        }
        catch (const std::exception&)
        {
            return 1001; //verify_result_tx_invalid;
        }

        if (tx_input_index >= tx->vin.size())
            return 1002; // verify_result_tx_input_invalid;

        if (GetSerializeSize(*tx, SER_NETWORK, PROTOCOL_VERSION) != transaction_size)
            return 1003; //verify_result_tx_size_invalid;

        ScriptError_t error;
        const auto& tx_ref = *tx;
        Amount am(amount);
        TransactionSignatureChecker checker(&tx_ref, tx_input_index, am);
        // const unsigned int script_flags = verify_flags_to_script_flags(flags);
        const unsigned int script_flags = flags;

        CScript output_script(prevout_script, prevout_script + prevout_script_size);
        const auto& input_script = tx->vin[tx_input_index].scriptSig;

        VerifyScript(input_script, output_script, script_flags, checker, &error);

        return error;
    }

    int verify_script_with_standard_script_verify_flags(const unsigned char* transaction,
        size_t transaction_size, const unsigned char* prevout_script,
        size_t prevout_script_size, unsigned int tx_input_index, int64_t amount = 0)
    {
        return verify_script(transaction, transaction_size, prevout_script, prevout_script_size, tx_input_index, STANDARD_SCRIPT_VERIFY_FLAGS, amount);
    }
}