/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>

#include "includes/eosio.system/native.hpp" //todo: was changed to it to work on a local net
#include "includes/abieos_numeric.hpp"
//#include <eosiolib/action.hpp>

namespace eosiosystem {
class system_contract;
}

namespace eosio
{

using std::string;


//@abi table
struct wait_weight
{
    uint32_t wait_sec;
    uint16_t weight;

    EOSLIB_SERIALIZE(wait_weight, (wait_sec)(weight))
};

// The definition in native.hpp is different (wrong?)
struct authority
{
    uint32_t threshold;
    std::vector<eosiosystem::key_weight> keys;
    std::vector<eosiosystem::permission_level_weight> accounts;
    std::vector<wait_weight> waits;

    EOSLIB_SERIALIZE(authority, (threshold)(keys)(accounts)(waits))
};

struct sell_type
{
    name account4sale;
    asset saleprice;
    name paymentaccnt;
    string message;
};

struct cancel_type
{
    uint64_t guid;
    string owner_key_str;
    string active_key_str;
};

struct cancelbid_type {
	uint64_t guid;
};

struct updatesale_type
{
    name account4sale;
    asset saleprice;
    string message;
};

struct vote_type
{
    name account4sale;
    name voter;
};

struct decidebid_type
{
    uint64_t guid;
    bool accept;
};

struct message_type
{
    name receiver;
    string message;
};

struct screener_type
{
    name account4sale;
    uint8_t option;
};

struct delegated_bandwidth
{
    name from;
    name to;
    asset net_weight;
    asset cpu_weight;

    uint64_t primary_key() const { return to.value; }
};
/*
struct user_resources
{
    name owner;
    asset net_weight;
    asset cpu_weight;
    int64_t ram_bytes;

    uint64_t primary_key() const { return owner.value; }
};
*/
struct transfer_type
{
    name from;
    name to;
    asset quantity;
    string memo;
};

class [[eosio::contract]] nameos : public contract
{

  public:
    // Transfr memo
    const uint16_t KEY_LENGTH = 53;

    // Bid decision
    const uint16_t BID_REJECTED = 0;
    const uint16_t BID_UNDECIDED = 1;
    const uint16_t BID_ACCEPTED = 2;

    // Constructor
    nameos(name self,name code, datastream<const char*> ds) : eosio::contract(self,code,ds),
    _accounts(_self,_self.value),
    _sold(_self, _self.value),
    _extras(_self,_self.value),
    _bids(_self,_self.value),
    _stats(_self, _self.value)  { }

    // Buy or bid action
    void handle_transfer(const transfer_type &transfer_data);

    // Sell account action
    void sell(const sell_type &sell_data);

    // Cancel sale action
    [[eosio::action("cancell")]]
    void cancel(const cancel_type &cancel_data);

    // Update the sale price
    void updatesale(const updatesale_type &updatesale_data);

    // Increment the vote count
    void vote(const vote_type &vote_data);

    // Propose a bid
    void bid(const uint64_t auction_guid, const name bidder,  const asset bidprice, const string owner_key, const string active_key);

    void cancelbid(const cancelbid_type &cancelbid_data);

    // Decide on a bid
    void decidebid(const decidebid_type &decidebid_data);

    // Broadcast a message to a user
    void message(const message_type &message_data);

    // Perform screening
    void screener(const screener_type &screener_data);

    // Init the stats table
    void initstats();

    // Buy an account listed for sale
    void buy_saleprice(const uint64_t auction_guid, const name from, const name to, const asset quantity, const string owner_key, const string active_key);

    // Buy custom accounts
    void buy_custom(const uint64_t auction_guid, const name from, const asset quantity, const string owner_key, const string active_key);

    // Update the auth for account4sale
    void account_auth(name account4sale, name changeto, name perm_child, name perm_parent, string pubkey);

    // Send a message action
    void send_message(name to, string message);

  private:

    // Contract & Referrer fee %
    const float contract_pc = 0.02;
    // Percent for the cancelling the bid
    const float cancelbid_pc = 0.01; // todo: make a fix price instead? - what if the bid is big

    // Fees Accounts
    name feesaccount = name("dexnamesfees"); //todo: set the desired account at this place

    // struct for account table
    struct account_table {
        // The id of the auction
        uint64_t guid;

        // Name of account being sold
        name account4sale;

        // Sale price in EOS
        asset saleprice;

        // Account that payment will be sent to
        name paymentaccnt;

        // Auctions creation datetime
        uint64_t created;

        uint64_t primary_key() const { return guid; }
        // making it as a secondary index
        // to be able to retrieve auction guid when creating a sell
        uint64_t by_account4sale() const { return account4sale.value; }
    };

    eosio::multi_index<name("accounts"), account_table,
      indexed_by<name("account4sale"), const_mem_fun<account_table, uint64_t, &account_table::by_account4sale>>
    > _accounts;



    struct sold_table {
      // The id of the auction
      uint64_t guid;

      // Name of account sold
      name account4sale;

      // Sale price in EOS
      asset saleprice;

      // Account that payment is sent to
      name paymentaccnt;

      // Account who has bought the name
      name buyer;

      // Timestamp when the contract was sold
      //todo: decide if it is a uint32_t or uint64_t
      uint64_t sold_at;

      uint64_t primary_key() const { return guid; }
    };

    eosio::multi_index<name("sold"), sold_table> _sold;


    // Struct for extras table
    struct extras_table
    {
        // Name of account being sold
        name account4sale;

        // Has the account been screened for deferred actions?
        bool screened;

        // Number of votes for this name
        uint64_t numberofvotes;

        // Last account to vote for this name
        name last_voter;

        // Message
        string message;

        uint64_t primary_key() const { return account4sale.value; }
    };

    eosio::multi_index<name("extras"), extras_table> _extras;

    // Struct for bids table
    struct bids_table
    {
        // The id of the auction bid belongs to
        uint64_t guid;

        // Name of account being sold
        name account4sale;

        // Accepted (2), Undecided (1), Rejected (0)
        uint16_t bidaccepted;

        // The bid price
        asset bidprice;

        // The account selling the name to watch 'my auctions'
        name paymentaccnt;

        // The account making the bid
        name bidder;

        // In case bid is accepted it's getting the new active & owner keys
        string ownerkey;
        string activekey;

        uint64_t primary_key() const { return guid; }
    };

    eosio::multi_index<name("bids"), bids_table> _bids;

    // Struct for the stats table
    struct stats_table
    {

        // Index
        uint64_t index;

        // Number of accounts currently listed
        uint64_t num_listed;

        // Number of accounts purchased
        uint64_t num_purchased;

        // Total sales
        asset tot_sales;

        // Total sales fees
        asset tot_fees;

        uint64_t primary_key() const { return index; }
    };

    eosio::multi_index<name("stats"), stats_table> _stats;
};

} // namespace eosio
