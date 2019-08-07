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

#include "monero_daemon_model.h"

#include "utils/monero_utils.h"
#include "include_base_utils.h"
#include "common/util.h"

/**
 * Public library interface.
 */
namespace monero {

  // ----------------------- UNDECLARED PRIVATE HELPERS -----------------------

  void merge_tx(vector<shared_ptr<monero_tx>>& txs, const shared_ptr<monero_tx>& tx) {
    for (const shared_ptr<monero_tx>& aTx : txs) {
      if (aTx->m_id.get() == tx->m_id.get()) {
        aTx->merge(aTx, tx);
        return;
      }
    }
    txs.push_back(tx);
  }

  // ------------------------- INITIALIZE CONSTANTS ---------------------------

  const string monero_tx::DEFAULT_PAYMENT_ID = string("0000000000000000");

  // ------------------------- SERIALIZABLE STRUCT ----------------------------

  string serializable_struct::serialize() const {
    return monero_utils::serialize(to_property_tree());
  }

  // ------------------------- MONERO BLOCK HEADER ----------------------------

  boost::property_tree::ptree monero_block_header::to_property_tree() const {
    boost::property_tree::ptree node;
    if (m_id != boost::none) node.put("id", *m_id);
    if (m_height != boost::none) node.put("height", *m_height);
    if (m_timestamp != boost::none) node.put("timestamp", *m_timestamp);
    if (m_size != boost::none) node.put("size", *m_size);
    if (m_weight != boost::none) node.put("weight", *m_weight);
    if (m_long_term_weight != boost::none) node.put("longTermWeight", *m_long_term_weight);
    if (m_depth != boost::none) node.put("depth", *m_depth);
    if (m_difficulty != boost::none) node.put("difficulty", *m_difficulty);
    if (m_cumulative_difficulty != boost::none) node.put("cumulativeDifficulty", *m_cumulative_difficulty);
    if (m_major_version != boost::none) node.put("majorVersion", *m_major_version);
    if (m_minor_version != boost::none) node.put("minorVersion", *m_minor_version);
    if (m_nonce != boost::none) node.put("m_nonce", *m_nonce);
    if (m_miner_tx_id != boost::none) node.put("minerTxId", *m_miner_tx_id);
    if (m_num_txs != boost::none) node.put("numTxs", *m_num_txs);
    if (m_orphan_status != boost::none) node.put("orphanStatus", *m_orphan_status);
    if (m_prev_id != boost::none) node.put("prevId", *m_prev_id);
    if (m_reward != boost::none) node.put("m_reward", *m_reward);
    if (m_pow_hash != boost::none) node.put("powHash", *m_pow_hash);
    return node;
  }

  void monero_block_header::merge(const shared_ptr<monero_block_header>& self, const shared_ptr<monero_block_header>& other) {
    if (this != self.get()) throw runtime_error("this != self");
    if (self == other) return;
    m_id = monero_utils::reconcile(m_id, other->m_id);
    m_height = monero_utils::reconcile(m_height, other->m_height, boost::none, boost::none, true, "block height"); // height can increase
    m_timestamp = monero_utils::reconcile(m_timestamp, other->m_timestamp, boost::none, boost::none, true, "block header timestamp");  // timestamp can increase
    m_size = monero_utils::reconcile(m_size, other->m_size, "block header size");
    m_weight = monero_utils::reconcile(m_weight, other->m_weight, "block header weight");
    m_long_term_weight = monero_utils::reconcile(m_long_term_weight, other->m_long_term_weight, "block header long term weight");
    m_depth = monero_utils::reconcile(m_depth, other->m_depth, "block header depth");
    m_difficulty = monero_utils::reconcile(m_difficulty, other->m_difficulty, "difficulty");
    m_cumulative_difficulty = monero_utils::reconcile(m_cumulative_difficulty, other->m_cumulative_difficulty, "m_cumulative_difficulty");
    m_major_version = monero_utils::reconcile(m_major_version, other->m_major_version, "m_major_version");
    m_minor_version = monero_utils::reconcile(m_minor_version, other->m_minor_version, "m_minor_version");
    m_nonce = monero_utils::reconcile(m_nonce, other->m_nonce, "m_nonce");
    m_miner_tx_id = monero_utils::reconcile(m_miner_tx_id, other->m_miner_tx_id);
    m_num_txs = monero_utils::reconcile(m_num_txs, other->m_num_txs, "block header m_num_txs");
    m_orphan_status = monero_utils::reconcile(m_orphan_status, other->m_orphan_status);
    m_prev_id = monero_utils::reconcile(m_prev_id, other->m_prev_id);
    m_reward = monero_utils::reconcile(m_reward, other->m_reward, "block header m_reward");
    m_pow_hash = monero_utils::reconcile(m_pow_hash, other->m_pow_hash);
  }

  // ----------------------------- MONERO BLOCK -------------------------------

  boost::property_tree::ptree monero_block::to_property_tree() const {
    boost::property_tree::ptree node = monero_block_header::to_property_tree();
    if (m_hex != boost::none) node.put("hex", *m_hex);
    if (m_miner_tx != boost::none) node.add_child("minerTx", (*m_miner_tx)->to_property_tree());
    if (!m_txs.empty()) node.add_child("txs", monero_utils::to_property_tree(m_txs));
    if (!m_tx_ids.empty()) node.add_child("txIds", monero_utils::to_property_tree(m_tx_ids));
    return node;
  }

  void monero_block::merge(const shared_ptr<monero_block_header>& self, const shared_ptr<monero_block_header>& other) {
    merge(static_pointer_cast<monero_block>(self), static_pointer_cast<monero_block>(other));
  }

  void monero_block::merge(const shared_ptr<monero_block>& self, const shared_ptr<monero_block>& other) {
    if (this != self.get()) throw runtime_error("this != self");
    if (self == other) return;

    // merge header fields
    monero_block_header::merge(self, other);

    // merge reconcilable block extensions
    m_hex = monero_utils::reconcile(m_hex, other->m_hex);
    m_tx_ids = monero_utils::reconcile(m_tx_ids, other->m_tx_ids);

    // merge miner tx
    if (m_miner_tx == boost::none) m_miner_tx = other->m_miner_tx;
    if (other->m_miner_tx != boost::none) {
      other->m_miner_tx.get()->m_block = self;
      m_miner_tx.get()->merge(m_miner_tx.get(), other->m_miner_tx.get());
    }

    // merge non-miner txs
    if (!other->m_txs.empty()) {
      for (const shared_ptr<monero_tx> otherTx : other->m_txs) { // NOTE: not using reference so shared_ptr is not deleted when block is dereferenced
        otherTx->m_block = self;
        merge_tx(self->m_txs, otherTx);
      }
    }
  }

  // ------------------------------- MONERO TX --------------------------------

  shared_ptr<monero_tx> monero_tx::copy(const shared_ptr<monero_tx>& src, const shared_ptr<monero_tx>& tgt) const {
    MTRACE("monero_tx::copy(const shared_ptr<monero_tx>& src, const shared_ptr<monero_tx>& tgt)");
    tgt->m_id = src->m_id;
    tgt->m_version = src->m_version;
    tgt->m_is_miner_tx = src->m_is_miner_tx;
    tgt->m_payment_id = src->m_payment_id;
    tgt->m_fee = src->m_fee;
    tgt->m_mixin = src->m_mixin;
    tgt->m_do_not_relay = src->m_do_not_relay;
    tgt->m_is_relayed = src->m_is_relayed;
    tgt->m_is_confirmed = src->m_is_confirmed;
    tgt->m_in_tx_pool = src->m_in_tx_pool;
    tgt->m_num_confirmations = src->m_num_confirmations;
    tgt->m_unlock_time = src->m_unlock_time;
    tgt->m_last_relayed_timestamp = src->m_last_relayed_timestamp;
    tgt->m_received_timestamp = src->m_received_timestamp;
    tgt->m_is_double_spend_seen = src->m_is_double_spend_seen;
    tgt->m_key = src->m_key;
    tgt->m_full_hex = src->m_full_hex;
    tgt->m_pruned_hex = src->m_pruned_hex;
    tgt->m_prunable_hex = src->m_prunable_hex;
    tgt->m_prunable_hash = src->m_prunable_hash;
    tgt->m_size = src->m_size;
    tgt->m_weight = src->m_weight;
    if (!src->m_vins.empty()) {
      tgt->m_vins = vector<shared_ptr<monero_output>>();
      for (const shared_ptr<monero_output>& vin : src->m_vins) {
        shared_ptr<monero_output> vinCopy = vin->copy(vin, make_shared<monero_output>());
        vinCopy->m_tx = tgt;
        tgt->m_vins.push_back(vinCopy);
      }
    }
    if (!src->m_vouts.empty()) {
      tgt->m_vouts = vector<shared_ptr<monero_output>>();
      for (const shared_ptr<monero_output>& vout : src->m_vouts) {
        shared_ptr<monero_output> voutCopy = vout->copy(vout, make_shared<monero_output>());
        voutCopy->m_tx = tgt;
        tgt->m_vouts.push_back(voutCopy);
      }
    }
    if (!src->m_output_indices.empty()) tgt->m_output_indices = vector<uint32_t>(src->m_output_indices);
    tgt->m_metadata = src->m_metadata;
    tgt->m_common_tx_sets = src->m_common_tx_sets;
    if (!src->m_extra.empty()) throw runtime_error("extra deep copy not implemented");  // TODO: implement extra
    tgt->m_rct_signatures = src->m_rct_signatures;
    tgt->m_rct_sig_prunable = src->m_rct_sig_prunable;
    tgt->m_is_kept_by_block = src->m_is_kept_by_block;
    tgt->m_is_failed = src->m_is_failed;
    tgt->m_last_failed_height = src->m_last_failed_height;
    tgt->m_last_failed_id = src->m_last_failed_id;
    tgt->m_max_used_block_height = src->m_max_used_block_height;
    tgt->m_max_used_block_id = src->m_max_used_block_id;
    if (!src->m_signatures.empty()) tgt->m_signatures = vector<string>(src->m_signatures);
    return tgt;
  }

  boost::property_tree::ptree monero_tx::to_property_tree() const {
    boost::property_tree::ptree node;
    if (m_id != boost::none) node.put("id", *m_id);
    if (m_version != boost::none) node.put("version", *m_version);
    if (m_is_miner_tx != boost::none) node.put("isMinerTx", *m_is_miner_tx);
    if (m_payment_id != boost::none) node.put("paymentId", *m_payment_id);
    if (m_fee != boost::none) node.put("fee", *m_fee);
    if (m_mixin != boost::none) node.put("mixin", *m_mixin);
    if (m_do_not_relay != boost::none) node.put("doNotRelay", *m_do_not_relay);
    if (m_is_relayed != boost::none) node.put("isRelayed", *m_is_relayed);
    if (m_is_confirmed != boost::none) node.put("isConfirmed", *m_is_confirmed);
    if (m_in_tx_pool != boost::none) node.put("inTxPool", *m_in_tx_pool);
    if (m_num_confirmations != boost::none) node.put("numConfirmations", *m_num_confirmations);
    if (m_unlock_time != boost::none) node.put("unlockTime", *m_unlock_time);
    if (m_last_relayed_timestamp != boost::none) node.put("lastRelayedTimestamp", *m_last_relayed_timestamp);
    if (m_received_timestamp != boost::none) node.put("receivedTimestamp", *m_received_timestamp);
    if (m_is_double_spend_seen != boost::none) node.put("isDoubleSpendSeen", *m_is_double_spend_seen);
    if (m_key != boost::none) node.put("key", *m_key);
    if (m_full_hex != boost::none) node.put("fullHex", *m_full_hex);
    if (m_pruned_hex != boost::none) node.put("prunedHex", *m_pruned_hex);
    if (m_prunable_hex != boost::none) node.put("prunableHex", *m_prunable_hex);
    if (m_prunable_hash != boost::none) node.put("prunableHash", *m_prunable_hash);
    if (m_size != boost::none) node.put("size", *m_size);
    if (m_weight != boost::none) node.put("weight", *m_weight);
    if (!m_vins.empty()) node.add_child("vins", monero_utils::to_property_tree(m_vins));
    if (!m_vouts.empty()) node.add_child("vouts", monero_utils::to_property_tree(m_vouts));
    if (!m_output_indices.empty()) throw runtime_error("m_output_indices not implemented");
    if (m_metadata != boost::none) node.put("metadata", *m_metadata);
    if (m_common_tx_sets != boost::none) throw runtime_error("m_common_tx_sets not implemented");
    if (!m_extra.empty()) node.add_child("extra", monero_utils::to_property_tree(m_extra));
    if (m_rct_signatures != boost::none) throw runtime_error("m_rct_signatures not implemented");
    if (m_rct_sig_prunable != boost::none) throw runtime_error("m_rct_sig_prunable not implemented");
    if (m_is_kept_by_block != boost::none) node.put("isKeptByBlock", *m_is_kept_by_block);
    if (m_is_failed != boost::none) node.put("isFailed", *m_is_failed);
    if (m_last_failed_height != boost::none) node.put("lastFailedHeight", *m_last_failed_height);
    if (m_last_failed_id != boost::none) node.put("lastFailedId", *m_last_failed_id);
    if (m_max_used_block_height != boost::none) node.put("maxUsedBlockHeight", *m_max_used_block_height);
    if (m_max_used_block_id != boost::none) node.put("maxUsedBlockId", *m_max_used_block_id);
    if (!m_signatures.empty()) throw runtime_error("m_signatures not implemented");
    return node;
  }

  boost::optional<uint64_t> monero_tx::get_height() const {
    if (m_block == boost::none) return boost::none;
    return *((*m_block)->m_height);
  }

  void monero_tx::merge(const shared_ptr<monero_tx>& self, const shared_ptr<monero_tx>& other) {
    if (this != self.get()) throw runtime_error("this != self");
    if (self == other) return;

    // merge blocks if they're different which comes back to merging txs
    if (m_block != other->m_block) {
      if (m_block == boost::none) {
        m_block = make_shared<monero_block>();
        m_block.get()->m_txs.push_back(self);
        m_block.get()->m_height = other->get_height();
      }
      if (other->m_block == boost::none) {
        other->m_block = make_shared<monero_block>();
        other->m_block.get()->m_txs.push_back(other);
        other->m_block.get()->m_height = self->get_height();
      }
      m_block.get()->merge(m_block.get(), other->m_block.get());
      return;
    }

    // otherwise merge tx fields
    m_id = monero_utils::reconcile(m_id, other->m_id);
    m_version = monero_utils::reconcile(m_version, other->m_version);
    m_payment_id = monero_utils::reconcile(m_payment_id, other->m_payment_id);
    m_fee = monero_utils::reconcile(m_fee, other->m_fee, "tx fee");
    m_mixin = monero_utils::reconcile(m_mixin, other->m_mixin, "tx m_mixin");
    m_is_confirmed = monero_utils::reconcile(m_is_confirmed, other->m_is_confirmed);
    m_do_not_relay = monero_utils::reconcile(m_do_not_relay, other->m_do_not_relay);
    m_is_relayed = monero_utils::reconcile(m_is_relayed, other->m_is_relayed);
    m_is_double_spend_seen = monero_utils::reconcile(m_is_double_spend_seen, other->m_is_double_spend_seen);
    m_key = monero_utils::reconcile(m_key, other->m_key);
    m_full_hex = monero_utils::reconcile(m_full_hex, other->m_full_hex);
    m_pruned_hex = monero_utils::reconcile(m_pruned_hex, other->m_pruned_hex);
    m_prunable_hex = monero_utils::reconcile(m_prunable_hex, other->m_prunable_hex);
    m_prunable_hash = monero_utils::reconcile(m_prunable_hash, other->m_prunable_hash);
    m_size = monero_utils::reconcile(m_size, other->m_size, "tx size");
    m_weight = monero_utils::reconcile(m_weight, other->m_weight, "tx weight");
    //m_output_indices = monero_utils::reconcile(m_output_indices, other->m_output_indices);  // TODO
    m_metadata = monero_utils::reconcile(m_metadata, other->m_metadata);
    m_common_tx_sets = monero_utils::reconcile(m_common_tx_sets, other->m_common_tx_sets);
    //m_extra = monero_utils::reconcile(m_extra, other->m_extra);  // TODO
    m_rct_signatures = monero_utils::reconcile(m_rct_signatures, other->m_rct_signatures);
    m_rct_sig_prunable = monero_utils::reconcile(m_rct_sig_prunable, other->m_rct_sig_prunable);
    m_is_kept_by_block = monero_utils::reconcile(m_is_kept_by_block, other->m_is_kept_by_block);
    m_is_failed = monero_utils::reconcile(m_is_failed, other->m_is_failed);
    m_last_failed_height = monero_utils::reconcile(m_last_failed_height, other->m_last_failed_height, "m_last_failed_height");
    m_last_failed_id = monero_utils::reconcile(m_last_failed_id, other->m_last_failed_id);
    m_max_used_block_height = monero_utils::reconcile(m_max_used_block_height, other->m_max_used_block_height, "max_used_block_height");
    m_max_used_block_id = monero_utils::reconcile(m_max_used_block_id, other->m_max_used_block_id);
    //m_signatures = monero_utils::reconcile(m_signatures, other->m_signatures); // TODO
    m_unlock_time = monero_utils::reconcile(m_unlock_time, other->m_unlock_time, "m_unlock_time");
    m_num_confirmations = monero_utils::reconcile(m_num_confirmations, other->m_num_confirmations, "m_num_confirmations");

    // merge vins
    if (!other->m_vins.empty()) {
      for (const shared_ptr<monero_output>& merger : other->m_vins) {
        bool merged = false;
        merger->m_tx = self;
        for (const shared_ptr<monero_output>& mergee : m_vins) {
          if ((*mergee->m_key_image)->m_hex == (*merger->m_key_image)->m_hex) {
            mergee->merge(mergee, merger);
            merged = true;
            break;
          }
        }
        if (!merged) m_vins.push_back(merger);
      }
    }

    // merge vouts
    if (!other->m_vouts.empty()) {
      for (const shared_ptr<monero_output>& vout : other->m_vouts) vout->m_tx = self;
      if (m_vouts.empty()) m_vouts = other->m_vouts;
      else {

        // determine if key images present
        int numKeyImages = 0;
        for (const shared_ptr<monero_output> vout : m_vouts) {
          if (vout->m_key_image != boost::none) {
            if ((*vout->m_key_image)->m_hex == boost::none) throw runtime_error("Key image hex cannot be null");
            numKeyImages++;
          }
        }
        for (const shared_ptr<monero_output>& vout : other->m_vouts) {
          if (vout->m_key_image != boost::none) {
            if ((*vout->m_key_image)->m_hex == boost::none) throw runtime_error("Key image hex cannot be null");
            numKeyImages++;
          }
        }
        if (numKeyImages != 0 && m_vouts.size() + other->m_vouts.size() != numKeyImages) throw runtime_error("Some vouts have a key image and some do not");

        // merge by key images
        if (numKeyImages > 0) {
          for (const shared_ptr<monero_output>& merger : other->m_vouts) {
            bool merged = false;
            merger->m_tx = self;
            for (const shared_ptr<monero_output>& mergee : m_vouts) {
              if ((*mergee->m_key_image)->m_hex == (*merger->m_key_image)->m_hex) {
                mergee->merge(mergee, merger);
                merged = true;
                break;
              }
            }
            if (!merged) m_vouts.push_back(merger);
          }
        }

        // merge by position
        else {
          if (m_vouts.size() != other->m_vouts.size()) throw runtime_error("Vout sizes are different");
          for (int i = 0; i < other->m_vouts.size(); i++) {
            m_vouts.at(i)->merge(m_vouts.at(i), other->m_vouts.at(i));
          }
        }
      }
    }

    // handle unrelayed -> relayed -> confirmed
    if (*m_is_confirmed) {
      m_in_tx_pool = false;
      m_received_timestamp = boost::none;
      m_last_relayed_timestamp = boost::none;
    } else {
      m_in_tx_pool = monero_utils::reconcile(m_in_tx_pool, other->m_in_tx_pool, boost::none, true, boost::none); // unrelayed -> tx pool
      m_received_timestamp = monero_utils::reconcile(m_received_timestamp, other->m_received_timestamp, boost::none, boost::none, false, "m_received_timestamp"); // take earliest receive time
      m_last_relayed_timestamp = monero_utils::reconcile(m_last_relayed_timestamp, other->m_last_relayed_timestamp, boost::none, boost::none, true, "m_last_relayed_timestamp"); // take latest relay time
    }
  }

  // --------------------------- MONERO KEY IMAGE -----------------------------

  shared_ptr<monero_key_image> monero_key_image::copy(const shared_ptr<monero_key_image>& src, const shared_ptr<monero_key_image>& tgt) const {
    if (this != src.get()) throw runtime_error("this != src");
    tgt->m_hex = src->m_hex;
    tgt->m_signature = src->m_signature;
    return tgt;
  }

  boost::property_tree::ptree monero_key_image::to_property_tree() const {
    boost::property_tree::ptree node;
    if (m_hex != boost::none) node.put("hex", *m_hex);
    if (m_signature != boost::none) node.put("signature", *m_signature);
    return node;
  }

  void monero_key_image::merge(const shared_ptr<monero_key_image>& self, const shared_ptr<monero_key_image>& other) {
    throw runtime_error("Not implemented");
  }

  // ------------------------------ MONERO OUTPUT -----------------------------

  shared_ptr<monero_output> monero_output::copy(const shared_ptr<monero_output>& src, const shared_ptr<monero_output>& tgt) const {
    if (this != src.get()) throw runtime_error("this != src");
    tgt->m_tx = src->m_tx;  // reference same parent tx by default
    if (src->m_key_image != boost::none) tgt->m_key_image = src->m_key_image.get()->copy(src->m_key_image.get(), make_shared<monero_key_image>());
    tgt->m_amount = src->m_amount;
    tgt->m_index = src->m_index;
    if (!src->m_ring_output_indices.empty()) tgt->m_ring_output_indices = vector<uint64_t>(src->m_ring_output_indices);
    tgt->m_stealth_public_key = src->m_stealth_public_key;
    return tgt;
  }

  boost::property_tree::ptree monero_output::to_property_tree() const {
    boost::property_tree::ptree node;
    if (m_key_image != boost::none) node.add_child("keyImage", (*m_key_image)->to_property_tree());
    if (m_amount != boost::none) node.put("amount", *m_amount);
    if (m_index != boost::none) node.put("index", *m_index);
    if (!m_ring_output_indices.empty()) node.add_child("ringOutputIndices", monero_utils::to_property_tree(m_ring_output_indices));
    if (m_stealth_public_key != boost::none) node.put("stealthPublicKey", *m_stealth_public_key);
    return node;
  }

  void monero_output::merge(const shared_ptr<monero_output>& self, const shared_ptr<monero_output>& other) {
    if (this != self.get()) throw runtime_error("this != self");
    if (self == other) return;

    // merge txs if they're different which comes back to merging outputs
    if (m_tx != other->m_tx) {
      m_tx->merge(m_tx, other->m_tx);
      return;
    }

    // otherwise merge output fields
    if (m_key_image == boost::none) m_key_image = other->m_key_image;
    else if (other->m_key_image != boost::none) m_key_image.get()->merge(m_key_image.get(), other->m_key_image.get());
    m_amount = monero_utils::reconcile(m_amount, other->m_amount, "output amount");
    m_index = monero_utils::reconcile(m_index, other->m_index, "output index");
  }
}
