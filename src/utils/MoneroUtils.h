/**
 * Copyright (c) 2017-2019 woodser
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Parts of this file are originally copyright (c) 2014-2019, The Monero Project
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * All rights reserved.
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers
 */

#ifndef MoneroUtils_h
#define MoneroUtils_h

#include "wallet/MoneroWallet.h"
#include "cryptonote_basic/cryptonote_basic.h"
#include "serialization/keyvalue_serialization.h"	// TODO: consolidate with other binary deps?
#include "storages/portable_storage.h"

/**
 * Collection of utilities for working with Monero's binary portable storage format.
 */
namespace MoneroUtils
{
  using namespace std;
  using namespace cryptonote;

  void jsonToBinary(const std::string &json, std::string &bin);

  void binaryToJson(const std::string &bin, std::string &json);

  void binaryBlocksToJson(const std::string &bin, std::string &json);

  shared_ptr<MoneroTxRequest> deserializeTxRequest(const string& txRequestStr);

  shared_ptr<MoneroTransferRequest> deserializeTransferRequest(const string& transferRequestStr);

  shared_ptr<MoneroOutputRequest> deserializeOutputRequest(const string& outputRequestStr);

  shared_ptr<MoneroSendRequest> deserializeSendRequest(const string& sendRequestStr);

  vector<shared_ptr<MoneroKeyImage>> deserializeKeyImages(const string& keyImagesJson);

  string serialize(const boost::property_tree::ptree& node);

  /**
   * Convert a Monero Core cryptonote::block to a block in this library's native model.
   *
   * @param cnBlock is the Core block to convert
   * @return a block in this library's native model
   */
  shared_ptr<MoneroBlock> cnBlockToBlock(const cryptonote::block& cnBlock);


  /**
   * Convert a Monero Core crpytonote::transaction to a transaction in this library's
   * native model.
   *
   * @param cnTx is the Core transaction to convert
   * @param initAsTxWallet specifies if a MoneroTx xor MoneroTxWallet should be initialized
   */
  shared_ptr<MoneroTx> cnTxToTx(const cryptonote::transaction& cnTx, bool initAsTxWallet = false);

  /**
   * Modified from core_rpc_server.cpp to return a string.
   *
   * TODO: remove this duplicate, use core_rpc_server instead
   */
  static std::string get_pruned_tx_json(cryptonote::transaction &tx)
  {
    std::stringstream ss;
    json_archive<true> ar(ss);
    bool r = tx.serialize_base(ar);
    CHECK_AND_ASSERT_MES(r, std::string(), "Failed to serialize rct signatures base");
    return ss.str();
  }

  // ------------------------- VALUE RECONCILATION- ---------------------------

  // TODO: template implementation here, could move to MoneroUtils.hpp per https://stackoverflow.com/questions/3040480/c-template-function-compiles-in-header-but-not-implementation
  template <class T> boost::property_tree::ptree toPropertyTree(const vector<shared_ptr<T>> types) {
    boost::property_tree::ptree typeNodes;
    for (const auto& type : types)  {
      typeNodes.push_back(std::make_pair("", type->toPropertyTree()));
    }
    return typeNodes;
  }

  // TODO: template implementation here, could move to MoneroUtils.hpp per https://stackoverflow.com/questions/3040480/c-template-function-compiles-in-header-but-not-implementation
  template <class T> boost::property_tree::ptree toPropertyTree(const vector<T> types) {
    boost::property_tree::ptree typeNodes;
    for (const auto& type : types)  {
      typeNodes.push_back(std::make_pair("", type.toPropertyTree()));
    }
    return typeNodes;
  }

  boost::property_tree::ptree toPropertyTree(const vector<string> strs);
  boost::property_tree::ptree toPropertyTree(const vector<uint32_t> nums);


  // TODO: refactor common template code
  template <class T, typename std::enable_if<std::is_same<T, string>::value, T>::type* = nullptr>
  boost::optional<T> reconcile(const boost::optional<T>& val1, const boost::optional<T>& val2, boost::optional<bool> resolveDefined, boost::optional<bool> resolveTrue, boost::optional<bool> resolveMax, const string& errMsg = "") {

    // check for equality
    if (val1 == val2) return val1;

    // resolve one value none
    if (val1 == boost::none || val2 == boost::none) {
      if (resolveDefined != boost::none && *resolveDefined == false) return boost::none;
      else return val1 == boost::none ? val2 : val1;
    }

    throw runtime_error(string("Cannot reconcile strings: ") + to_string(val1) + string(" vs ") + to_string(val2) + (!errMsg.empty() ? string(". ") + errMsg : string("")));
  }
  template <class T, typename std::enable_if<std::is_same<T, string>::value, T>::type* = nullptr>
  boost::optional<T> reconcile(const boost::optional<T>& val1, const boost::optional<T>& val2, const string& errMsg = "") {
    return reconcile(val1, val2, boost::none, boost::none, boost::none, errMsg);
  }

  template <class T, typename std::enable_if<std::is_integral<T>::value, T>::type* = nullptr>
  boost::optional<T> reconcile(const boost::optional<T>& val1, const boost::optional<T>& val2, boost::optional<bool> resolveDefined, boost::optional<bool> resolveTrue, boost::optional<bool> resolveMax, const string& errMsg = "") {

    // check for equality
    if (val1 == val2) return val1;

    // resolve one value none
    if (val1 == boost::none || val2 == boost::none) {
      if (resolveDefined != boost::none && *resolveDefined == false) return boost::none;
      else return val1 == boost::none ? val2 : val1;
    }

    // resolve different numbers
    if (resolveMax != boost::none) {
      return *resolveMax ? max(*val1, *val2) : min(*val1, *val2);
    }

    // throw runtime_error("Cannot reconcile values " + val1 + " and " + val2 + " with config: [" + resolveDefined + ", " + resolveTrue + ", " + resolveMax + "]", val1, val2);
    throw runtime_error(string("Cannot reconcile integrals: ") + to_string(val1) + string(" vs ") + to_string(val2) + (!errMsg.empty() ? string(". ") + errMsg : string("")));
  }
  template <class T, typename std::enable_if<std::is_integral<T>::value, T>::type* = nullptr>
  boost::optional<T> reconcile(const boost::optional<T>& val1, const boost::optional<T>& val2, const string& errMsg = "") {
    return reconcile(val1, val2, boost::none, boost::none, boost::none, errMsg);
  }

  template <class T, typename std::enable_if<std::is_same<T, bool>::value, T>::type>
  boost::optional<T> reconcile(const boost::optional<T>& val1, const boost::optional<T>& val2, boost::optional<bool> resolveDefined, boost::optional<bool> resolveTrue, boost::optional<bool> resolveMax, const string& errMsg = "") {

    // check for equality
    if (val1 == val2) return val1;

    // resolve one value none
    if (val1 == boost::none || val2 == boost::none) {
      if (resolveDefined != boost::none && *resolveDefined == false) return boost::none;
      else return val1 == boost::none ? val2 : val1;
    }

    // resolve different booleans
    if (resolveTrue != boost::none) {
      return val1 == resolveTrue ? val1 : val2; // if resolve true, return true, else return false
    } else {
      throw runtime_error(string("Cannot reconcile booleans: ") + to_string(val1) + string(" vs ") + to_string(val2) + (!errMsg.empty() ? string(". ") + errMsg : string("")));
    }
  }
  template <class T, typename std::enable_if<std::is_same<T, bool>::value, T>::type>
  boost::optional<T> reconcile(const boost::optional<T>& val1, const boost::optional<T>& val2, const string& errMsg = "") {
    return reconcile(val1, val2, boost::none, boost::none, boost::none, errMsg);
  }

  template <class T>
  vector<T> reconcile(const vector<T>& v1, const vector<T>& v2, const string& errMsg = "") {

    // check for equality
    if (v1 == v2) return v1;

    // resolve one vector empty
    if (v1.empty()) return v2;
    if (v2.empty()) return v1;

    // otherwise cannot reconcile
    throw runtime_error("Cannot reconcile vectors" + (!errMsg.empty() ? string(". ") + errMsg : string("")));
  }

//    template <class T>
//    boost::optional<T> reconcile(const boost::optional<T>& val1, const boost::optional<T>& val2) {
//      return reconcile(val1, val2, boost::none, boost::none, boost::none);
//    }
}
#endif /* MoneroUtils_h */
