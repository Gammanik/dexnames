/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "eosnameswaps.hpp"

namespace eosio
{

// Action: Sell an account
void eosnameswaps::sell(const sell_type &sell_data) {
    // ----------------------------------------------
    // Auth checks
    // ----------------------------------------------

    // Only the account4sale@owner can sell (contract@eosio.code must already be an owner)
    require_auth2(sell_data.account4sale.value, name("owner").value);

    // check if someone put a suffix name on sale (this could be implemented more efficient)
    const string sellName = name{sell_data.account4sale}.to_string();
    bool has_dot = false;

    if (sellName.length() < 12) {
      for (int lp = 0; lp <= sellName.length(); ++lp) {
        if (sellName[lp] == '.') has_dot = true;
      }

      if (!has_dot) eosio_assert(false, "Are you sure you want to sell suffix account? Contact us: eosnamesbids@gmail.com");
    }

    // ----------------------------------------------
    // Valid transaction checks
    // ----------------------------------------------

    const uint64_t guid = _accounts.available_primary_key();

    // Check the payment account exists
    eosio_assert(is_account(sell_data.paymentaccnt), "Sell Error: The payment account does not exist.");

    // Check the transfer is valid
    eosio_assert(sell_data.saleprice.symbol == symbol("EOS", 4), "Sell Error: Sale price must be in EOS. Ex: '10.0000 EOS'.");
    eosio_assert(sell_data.saleprice.is_valid(), "Sell Error: Sale price is not valid.");
    eosio_assert(sell_data.saleprice >= asset(10000, symbol("EOS", 4)), "Sell Error: Sale price must be at least 1 EOS. Ex: '1.0000 EOS'.");

    // Check the message is not longer than 100 characters
    eosio_assert(sell_data.message.length() <= 100, "Sell Error: The message must be <= 100 characters.");

    // ----------------------------------------------
    // Change account ownership
    // ----------------------------------------------

    // Change auth from account4sale@active to contract@active
    // This ensures eosio.code permission has been set to the contract
    account_auth(sell_data.account4sale, _self, name("active"), name("owner"), "None");

    // Change auth from contract@owner to owner@owner
    // This ensures the contract is the only owner
    account_auth(sell_data.account4sale, _self, name("owner"), name(""), "None");

    // ----------------------------------------------
    // Add data to tables
    // ----------------------------------------------

    // Place data in accounts table. Seller pays for ram storage
    _accounts.emplace(sell_data.account4sale, [&](auto &s) {
        s.account4sale = sell_data.account4sale;
        s.guid = guid;
        s.saleprice = sell_data.saleprice;
        s.paymentaccnt = sell_data.paymentaccnt;
        s.created = now();
    });

    // Place data in extras table. Seller pays for ram storage
    _extras.emplace(sell_data.account4sale, [&](auto &s) {
        s.account4sale = sell_data.account4sale;
        s.screened = false;
        s.numberofvotes = 0;
        s.last_voter = name("");
        s.message = sell_data.message;
    });

    // Place data in bids table. Bidder pays for ram storage
    _bids.emplace(sell_data.account4sale, [&](auto &s) {
        s.guid = guid;
        s.account4sale = sell_data.account4sale;
        s.bidaccepted = 1;
        s.bidprice = asset(0, symbol("EOS", 4));
        s.paymentaccnt = sell_data.paymentaccnt;
        s.bidder = name("");
    });

    // Place data in stats table. Contract pays for ram storage
    auto itr_stats = _stats.find(0);
    _stats.modify(itr_stats, _self, [&](auto &s) {
        s.num_listed++;
    });
    
    // Send message
    send_message(sell_data.paymentaccnt, string("EOSNameSwaps: Your account ") + name{sell_data.account4sale}.to_string() + string(" has been listed for sale. Keep an eye out for bids, and don't forget to vote for accounts you like!"));
}

// Action: Buy an account listed for sale
void eosnameswaps::handle_transfer(const transfer_type &transfer_data) {
    // ----------------------------------------------
    // Auth checks
    // ----------------------------------------------

    // Important: The transfer fees actions below will trigger this function without this
    if (transfer_data.from == _self) return;

    // EOSBet hack
    eosio_assert(transfer_data.to == _self, "Buy Error: Transfer must be direct to contract.");

    // ----------------------------------------------
    // Valid transaction checks
    // ----------------------------------------------

    // Check the buy code is valid
    const string buy_code = transfer_data.memo.substr(0, 3);
	// buy_code == "fii" in case if you want to send money on the conract then just write "fill" in memo
    eosio_assert(buy_code == "cn:" || buy_code == "sp:" || buy_code == "bd:" || buy_code == "", "Buy Error:: Malformed buy string:");

    if (buy_code == "") {
        return;
    }

    // Check the transfer is valid
    eosio_assert(transfer_data.quantity.symbol == symbol("EOS", 4), "Buy Error: You must pay in EOS.");
    eosio_assert(transfer_data.quantity.is_valid(), "Buy Error: Quantity is not valid.");

    // Strip buy code from memo
    const string memo = transfer_data.memo.substr(3);

    // Find the length of the auction id
    int guid_length = 0;
    for (int lp = 1; lp <= 120; ++lp) {
        if (memo[lp] == ',') {
            guid_length = lp;
            break;
         }
    }

    // Check the guid length is valid
    eosio_assert(guid_length > 0, "Buy Error: Malformed auction id.");

    // Extract account to buy from memo
    const string auction_guid_string = memo.substr(0, guid_length);
    const uint64_t auction_guid = std::strtoull(auction_guid_string.c_str(),NULL,0);

    // Extract keys
    const string owner_key = memo.substr(guid_length + 1, KEY_LENGTH);
    const string active_key = memo.substr(guid_length + 2 + KEY_LENGTH, KEY_LENGTH);

    // Call the requried function
    if (buy_code == "bd:") {
        bid(auction_guid, transfer_data.from, transfer_data.quantity, owner_key, active_key);
    }
    else if (buy_code == "cn:")
    {
        buy_custom(auction_guid, transfer_data.from, transfer_data.quantity, owner_key, active_key);
    }
    else if (buy_code == "sp:")
    {
        buy_saleprice(auction_guid, transfer_data.from, transfer_data.to, transfer_data.quantity, owner_key, active_key);
    }
}

void eosnameswaps::buy_custom(const uint64_t auction_guid, const name from, const asset quantity, const string owner_key, const string active_key) {
    eosio_assert(false, "Suffix sale is not supported yet");
}

void eosnameswaps::buy_saleprice(const uint64_t auction_guid, const name from, const name to, const asset quantity, const string owner_key, const string active_key) {

    // ----------------------------------------------
    // Sale/Bid price
    // ----------------------------------------------

    // Check the account is available to buy
    auto itr_accounts = _accounts.find(auction_guid);
    eosio_assert(itr_accounts != _accounts.end(), (std::string("Buy Error: Account: ") + itr_accounts->account4sale.to_string() + std::string(" is not for sale.")).c_str());

    // Sale price
    auto saleprice = itr_accounts->saleprice;
    eosio_assert(saleprice == quantity, "Buy Error: You have not transferred the correct amount of EOS. Check the sale price.");

    // ----------------------------------------------
    // Seller, Contract, & Referrer fees
    // ----------------------------------------------

    auto sellerfee = asset(0, symbol("EOS", 4));
    auto contractfee = asset(0, symbol("EOS", 4));

    // Fee amounts
    contractfee.amount = int(saleprice.amount * contract_pc);
    sellerfee.amount = saleprice.amount - contractfee.amount;

    // Transfer EOS from contract to contract fees account
    action(
        permission_level{_self, name("active")},
        name("eosio.token"), name("transfer"),
        std::make_tuple(_self, feesaccount, contractfee, std::string("EOSNameSwaps: Account contract fee: ") + itr_accounts->account4sale.to_string()))
        .send();

    // Transfer EOS from contract to seller minus the contract fees
    action(
        permission_level{_self, name("active")},
        name("eosio.token"), name("transfer"),
        std::make_tuple(_self, itr_accounts->paymentaccnt, sellerfee, std::string("EOSNameSwaps: Account seller fee: ") + itr_accounts->account4sale.to_string()))
        .send();

    // ----------------------------------------------
    // Update account owner
    // ----------------------------------------------

    // Remove contract@owner permissions and replace with buyer@active account and the supplied key
    account_auth(itr_accounts->account4sale, from, name("active"), name("owner"), active_key);

    // Remove seller@active permissions and replace with buyer@owner account and the supplied key
    account_auth(itr_accounts->account4sale, from, name("owner"), name(""), owner_key);

    // ----------------------------------------------
    // Cleanup
    // ----------------------------------------------

    // Erase account from the accounts table
    _accounts.erase(itr_accounts);

    // Erase account from the extras table
    auto itr_extras = _extras.find(itr_accounts->account4sale.value);
    _extras.erase(itr_extras);

    // Erase account from the bids table
    auto itr_bids = _bids.find(auction_guid);
    _bids.erase(itr_bids);



    // Place data in sold_names table
    _sold.emplace(_self, [&](auto &s) {
        s.guid = auction_guid;
        s.account4sale = itr_accounts->account4sale;
        s.saleprice = saleprice;
        s.paymentaccnt = itr_accounts->paymentaccnt;
        s.buyer = from;
        s.sold_at = now(); // TODO TODO:::::::   ::::::::::::: change to sold
    });


    // Place data in stats table. Contract pays for ram storage
    auto itr_stats = _stats.find(0);
    _stats.modify(itr_stats, _self, [&](auto &s) {
        s.num_listed--;
        s.num_purchased++;
        s.tot_sales += saleprice;
        s.tot_fees += contractfee;
    });

    send_message(from, string("eosnamesbids: You have successfully bought the account ") + itr_accounts->account4sale.to_string()
    + string(" from: ") + itr_accounts->paymentaccnt.to_string() + string(" with the price:") + saleprice.to_string() + string(". Please come again."));
}

// Action: Remove a listed account from sale
void eosnameswaps::cancel(const cancel_type &cancel_data) {

    // ----------------------------------------------
    // Auth checks
    // ----------------------------------------------

    // Check an account with that name is listed for sale
    auto itr_accounts = _accounts.find(cancel_data.guid);
    eosio_assert(itr_accounts != _accounts.end(), "Cancel Error: That account name is not listed for sale");

    // Only the payment account can cancel the sale (the contract has the owner key)
    eosio_assert(has_auth(itr_accounts->paymentaccnt), "Cancel Error: Only the payment account can cancel the sale.");

    // ----------------------------------------------
    // Update account owners
    // ----------------------------------------------

    // Change auth from contract@active to submitted active key
    account_auth(itr_accounts->account4sale, itr_accounts->paymentaccnt, name("active"), name("owner"), cancel_data.active_key_str);

    // Change auth from contract@owner to submitted owner key
    account_auth(itr_accounts->account4sale, itr_accounts->paymentaccnt, name("owner"), name(""), cancel_data.owner_key_str);

    auto itr_bids = _bids.find(cancel_data.guid);
    if (itr_bids->bidprice != asset(0, symbol("EOS", 4))) { // todo: will it work? maybe check for null?
		// Transfer EOS back to the bidder
        action(
            permission_level{_self, name("active")},
            name("eosio.token"), name("transfer"),
            std::make_tuple(_self, itr_bids->bidder, itr_bids->bidprice,
            std::string("EOSNameSwaps: The auction name:") +
            itr_accounts->account4sale.to_string() +
            std::string(" was cancelled by seller. Refunding your bid.")))
        .send();
    }

	// todo: add charging fee for the cancel?

    // ----------------------------------------------
    // Cleanup
    // ----------------------------------------------

    // Erase account from accounts table
    _accounts.erase(itr_accounts);

    // Erase account from the extras table
    auto itr_extras = _extras.find(itr_accounts->account4sale.value);
    _extras.erase(itr_extras);

    // Erase account from the bids table
    _bids.erase(itr_bids);

    // Place data in stats table. Contract pays for ram storage
    auto itr_stats = _stats.find(0);
    _stats.modify(itr_stats, _self, [&](auto &s) {
        s.num_listed--;
    });

    // Send message
    send_message(itr_accounts->paymentaccnt, string("EOSNameSwaps: You have successfully cancelled the sale of the account ") + name{itr_accounts->account4sale}.to_string() + string(". Please come again."));
}

// Action: Update the sale price
void eosnameswaps::updatesale(const updatesale_type &updatesale_data) {

    // Check an account with that name is listed for sale
    auto itr_accounts = _accounts.find(updatesale_data.account4sale.value);
    eosio_assert(itr_accounts != _accounts.end(), "Update Error: That account name is not listed for sale");

    // Only the payment account can update the sale price
    eosio_assert(has_auth(itr_accounts->paymentaccnt), "Update Error: Only the payment account can update a sale.");

    // ----------------------------------------------
    // Valid transaction checks
    // ----------------------------------------------

    // Check the transfer is valid
    eosio_assert(updatesale_data.saleprice.symbol == symbol("EOS", 4), "Update Error: Sale price must be in EOS. Ex: '10.0000 EOS'.");
    eosio_assert(updatesale_data.saleprice.is_valid(), "Update Error: Sale price is not valid.");
    eosio_assert(updatesale_data.saleprice >= asset(10000, symbol("EOS", 4)), "Update Error: Sale price must be at least 1 EOS. Ex: '1.0000 EOS'.");

    // Check the message is not longer than 100 characters
    eosio_assert(updatesale_data.message.length() <= 100, "Sell Error: The message must be <= 100 characters.");

    // ----------------------------------------------
    // Update tables
    // ----------------------------------------------

    // Place data in accounts table. Payment account pays for ram storage
    _accounts.modify(itr_accounts, itr_accounts->paymentaccnt, [&](auto &s) {
        s.saleprice = updatesale_data.saleprice;
    });

    // Place data in extras table. Payment account pays for ram storage
    auto itr_extras = _extras.find(updatesale_data.account4sale.value);
    _extras.modify(itr_extras, itr_accounts->paymentaccnt, [&](auto &s) {
        s.message = updatesale_data.message;
    });

    // Send message
    send_message(itr_accounts->paymentaccnt, string("EOSNameSwaps: You have successfully updated the sale of the account ") + name{updatesale_data.account4sale}.to_string());
}

// Action: Increment votes
void eosnameswaps::vote(const vote_type &vote_data) {
    // Confirm the voter is who they say they are
    eosio_assert(has_auth(vote_data.voter), "Vote Error: You are not who you say you are. Check permissions.");

    // ----------------------------------------------
    // Valid transaction checks
    // ----------------------------------------------
    // todo: find a way for not searching in the _accounts table
    // todo: add guid to the extra table?

    // Check an account with that name is listed for sale
    auto itr_extras = _extras.find(vote_data.account4sale.value);
    eosio_assert(itr_extras != _extras.end(), "Vote Error: That account name is not listed for sale.");

    // Can only vote once in a row
    eosio_assert(vote_data.voter != itr_extras->last_voter, "Vote Error: You have already voted for this account!");

    // ----------------------------------------------
    // Update table
    // ----------------------------------------------

    // Place data in extras table. Voter pays for ram storage
    _extras.modify(itr_extras, vote_data.voter, [&](auto &s) {
        s.numberofvotes++;
        s.last_voter = vote_data.voter;
    });
}

// Action: Bid for an account
void eosnameswaps::bid(const uint64_t auction_guid, const name bidder, const asset bidprice, const string owner_key, const string active_key) {

    // ----------------------------------------------
    // Valid transaction checks
    // ----------------------------------------------

    auto itr_accounts = _accounts.find(auction_guid);
    eosio_assert(itr_accounts != _accounts.end(), "Bid Error: Auction with the given guid is not found");

    // Check an account with that name is listed for sale
    auto itr_bids = _bids.find(auction_guid);


    // Check the transfer is valid
    eosio_assert(bidprice.symbol == symbol("EOS", 4), "Propose Bid Error: Bid price must be in EOS. Ex: '10.0000 EOS'.");
    eosio_assert(bidprice.is_valid(), "Propose Bid Error: Bid price is not valid.");
    eosio_assert(bidprice >= asset(10000, symbol("EOS", 4)), "Propose Bid Error: The minimum bid price is 1.0000 EOS.");

    // Only accept new bids if they are higher
    eosio_assert(bidprice > itr_bids->bidprice, "Bid Error: You must bid higher than the last bidder.");

    // Only accept new bids if they are lower than the sale price
    eosio_assert(bidprice <= itr_accounts->saleprice, "Propose Bid Error: You must bid lower than the sale price.");


    if (itr_bids->bidprice != asset(0, symbol("EOS", 4))) { // todo: will it work? maybe check for null?
        // Transfer EOS back to the bidder
        action(
            permission_level{_self, name("active")},
            name("eosio.token"), name("transfer"),
            std::make_tuple(_self, itr_bids->paymentaccnt, itr_bids->bidprice, std::string("EOSNameSwaps: auction: ") + itr_accounts->account4sale.to_string() + std::string(". you were re-bided. Refunding your bid.")))
            .send();
    }

    // ----------------------------------------------
    // Update table
    // ----------------------------------------------

    // Place data in bids table. Bidder pays for ram storage
    _bids.modify(itr_bids, _self, [&](auto &s) { // todo: there were 'bidder' instead of _self
        s.bidaccepted = 1; // todo: this field now is useless
        s.bidprice = bidprice;
        s.bidder = bidder;
        s.ownerkey = owner_key;
        s.activekey = active_key;
    });

    // Send message
    send_message(itr_accounts->paymentaccnt, string("EOSNameSwaps: Your account ") +
        name{itr_accounts->account4sale}.to_string() +
        string(" has received a bid. If you choose to accept it, you'll get the XX EOS. Others can still bid higher or pay the full sale price until then."));
}

void eosnameswaps::cancelbid(const cancelbid_type &cancelbid_data) {
	    // ----------------------------------------------
        // Auth checks
        // ----------------------------------------------

        // Check an account with that name is listed for sale
        auto itr_accounts = _accounts.find(cancelbid_data.guid);
        eosio_assert(itr_accounts != _accounts.end(), "Cancel Bid Error: That account name is not listed for sale.");

        // Only the bidder account can cancel bids
        auto itr_bids = _bids.find(cancelbid_data.guid);
        eosio_assert(itr_bids->bidprice > asset(0, symbol("EOS", 4)), "Cancel Bid Error: The auction does not have any bids.");
        eosio_assert(has_auth(itr_bids->bidder), "Cancel Bid Error: Only the bidder account can cancel bids.");

	    // ----------------------------------------------
        // Seller, Contract, & Referrer fees
        // ----------------------------------------------

        // Refund the bid to the bidder
	    auto bidderfee = asset(0, symbol("EOS", 4));
        auto cancelbidfee = asset(0, symbol("EOS", 4));

        // Fee amounts
        cancelbidfee.amount = int(itr_bids->bidprice.amount * cancelbid_pc);
        bidderfee.amount = itr_bids->bidprice.amount - cancelbidfee.amount;

        // Transfer EOS from contract to contract fees account
        action(
            permission_level{_self, name("active")},
            name("eosio.token"), name("transfer"),
            std::make_tuple(_self, feesaccount, cancelbidfee,
                std::string("EOSNameSwaps: Cancel bid fee by: ") + itr_bids->bidder.to_string() +
                std::string(". For the account: ") + itr_accounts->account4sale.to_string()))
            .send();

        // Transfer EOS from contract to seller minus the contract fees
        action(
            permission_level{_self, name("active")},
            name("eosio.token"), name("transfer"),
            std::make_tuple(_self, itr_bids->bidder, bidderfee,
                std::string("EOSNameSwaps: Bid refund fee for the auction: ") + itr_accounts->account4sale.to_string()))
            .send();


        // ----------------------------------------------
        // Update table
        // ----------------------------------------------

        // Place data in bids table. Bidder pays for ram storage
        _bids.modify(itr_bids, _self, [&](auto &s) { // todo: make the person cancelling the bid to pay for the ram?
            s.bidaccepted = 1; // todo: this field now is useless
            s.bidprice = asset(0, symbol("EOS", 4));
            s.bidder = name("");
            s.ownerkey = "";
            s.activekey = "";
        });


}

// Action: Accept or decline a bid for an account. paymentaccnt has to decide
void eosnameswaps::decidebid(const decidebid_type &decidebid_data) {

    // ----------------------------------------------
    // Auth checks
    // ----------------------------------------------

    // Check an account with that name is listed for sale
    auto itr_accounts = _accounts.find(decidebid_data.guid);
    eosio_assert(itr_accounts != _accounts.end(), "Decide Bid Error: That account name is not listed for sale.");

    // Only the payment account can accept bids
    eosio_assert(has_auth(itr_accounts->paymentaccnt), "Decide Bid Error: Only the payment account can decide on bids.");

    // ----------------------------------------------
    // Valid transaction checks
    // ----------------------------------------------

    auto itr_bids = _bids.find(decidebid_data.guid);

    // Check there is a bid to accept or reject
    eosio_assert(itr_bids->bidprice != asset(0, symbol("EOS", 4)), "Decide Bid Error: There are no bids to accept or reject.");

    // ----------------------------------------------
    // Update table
    // ----------------------------------------------

    // Place data in bids table. Bidder pays for ram storage
    if (decidebid_data.accept == true)
    {
        auto saleprice = itr_bids->bidprice;
        auto sellerfee = asset(0, symbol("EOS", 4));
        auto contractfee = asset(0, symbol("EOS", 4));

        // Fee amounts
        contractfee.amount = int(saleprice.amount * contract_pc);
        sellerfee.amount = saleprice.amount - contractfee.amount;

        const string owner_key = itr_bids->ownerkey;
        const string active_key = itr_bids->activekey;


        // Transfer EOS from contract to contract fees account
        action(
            permission_level{_self, name("active")},
            name("eosio.token"), name("transfer"),
            std::make_tuple(_self, feesaccount, contractfee, std::string("EOSNameSwaps: Account contract fee. Accept bid for: ") + itr_accounts->account4sale.to_string()))
            .send();

        // Transfer EOS from contract to seller minus the contract fees
        action(
            permission_level{_self, name("active")},
            name("eosio.token"), name("transfer"),
            std::make_tuple(_self, itr_accounts->paymentaccnt, sellerfee, std::string("EOSNameSwaps: Account seller fee. Accept bid for: ") + itr_accounts->account4sale.to_string()))
            .send();

	    // ----------------------------------------------
	    // Update account owner
	    // ----------------------------------------------

		//todo: do i need to store active & owner keys in the _bids table?
	    // Remove contract@owner permissions and replace with buyer@active account and the supplied key
	    account_auth(itr_accounts->account4sale, itr_bids->bidder, name("active"), name("owner"), active_key);

	    // Remove seller@active permissions and replace with buyer@owner account and the supplied key
	    account_auth(itr_accounts->account4sale, itr_bids->bidder, name("owner"), name(""), owner_key);

	    // Erase account from the accounts table
	    _accounts.erase(itr_accounts);

	    // Erase account from the extras table
	    auto itr_extras = _extras.find(itr_accounts->account4sale.value);
	    _extras.erase(itr_extras);

	    // Erase account from the bids table
	    _bids.erase(itr_bids);

    }
    else
    {

        // Bid rejected
        _bids.modify(itr_bids, itr_accounts->paymentaccnt, [&](auto &s) {
            s.bidaccepted = 0;
        });

        // Send message
        send_message(itr_bids->bidder, string("EOSNameSwaps: Your bid for ") + name{itr_accounts->account4sale}.to_string() + string(" has been rejected. Increase your bid offer"));
    }
}

// Message Action
void eosnameswaps::message(const message_type &message_data) {

    // ----------------------------------------------
    // Auth checks
    // ----------------------------------------------

    // Only the contract can send a message
    eosio_assert(has_auth(_self), "Message Error: Only the contract can send messages.");

    // ----------------------------------------------

    // Notify the specified account
    require_recipient(message_data.receiver);
}

// Action: Perform admin tasks
void eosnameswaps::screener(const screener_type &screener_data) {

    // Only the contract account can perform screening
    require_auth(_self);

    // Set the screening status of an account listed for sale
    int screened = screener_data.option;
    eosio_assert(screened == 0 || screened == 1, "Admin Error: Malformed screening data.");

    // Place data in table. Contract pays for ram storage
    auto itr_extras = _extras.find(screener_data.account4sale.value);
    _extras.modify(itr_extras, _self, [&](auto &s) {
        s.screened = screened;
    });

    // ----------------------------------------------
}

// Init the stats table
void eosnameswaps::initstats() {

    // ----------------------------------------------
    // Auth checks
    // ----------------------------------------------

    // Only the contract account can init the stats table
    require_auth(_self);

    // ----------------------------------------------

    // Init stats table
    auto itr_stats = _stats.find(0);
    if (itr_stats == _stats.end())
    {
    
        for (int index = 0; index <= 4; index++) {        
            _stats.emplace(_self, [&](auto &s) {
                s.index = index;
                s.num_listed = 0;
                s.num_purchased = 0;
                s.tot_sales = asset(0, symbol("EOS", 4));
                s.tot_fees = asset(0, symbol("EOS", 4));
            });
        }
    
    }
    
    
}

// Broadcast message
void eosnameswaps::send_message(name receiver, string message) {

    action(permission_level{_self, name("active")},
           name(_self), name("message"),
           std::make_tuple(
               receiver,
               message))
        .send();
}

// Changes the owner/active permissions
void eosnameswaps::account_auth(name account4sale, name changeto, name perm_child, name perm_parent, string pubkey_str) {

    // Setup authority for contract. Choose either a new key, or account, or both.
    authority contract_authority;
    if (pubkey_str != "None")
    {
        // Convert the public key string to the requried type
        const abieos::public_key pubkey = abieos::string_to_public_key(pubkey_str);

        // Array to hold public key
        std::array<char, 33> pubkey_char;
        std::copy(pubkey.data.begin(), pubkey.data.end(), pubkey_char.begin());

        eosiosystem::key_weight kweight{
            .key = {(uint8_t)abieos::key_type::k1, pubkey_char},
            .weight = (uint16_t)1};

        // Key is supplied
        contract_authority.threshold = 1;
        contract_authority.keys = {kweight};
        contract_authority.accounts = {};
        contract_authority.waits = {};
    }
    else
    {

        // Account to take over permission changeto@perm_child
        eosiosystem::permission_level_weight accounts{
            .permission = permission_level{changeto, perm_child},
            .weight = (uint16_t)1};

        // Key is not supplied
        contract_authority.threshold = 1;
        contract_authority.keys = {};
        contract_authority.accounts = {accounts};
        contract_authority.waits = {};
    }

    // Remove contract permissions and replace with changeto account.
    action(permission_level{account4sale, name("owner")},
           name("eosio"), name("updateauth"),
           std::make_tuple(
               account4sale,
               perm_child,
               perm_parent,
               contract_authority))
        .send();
} // namespace eosio


//void eosnameswaps::erasetables() {
//	// Only the contract account can init the stats table
//    require_auth(_self);

//	auto itr_bids = _bids->begin();
//    while (itr_bids != table->end()) {
//        itr_bids = table->erase(itr_bids);
//    }

//}

extern "C"
{
    void apply(uint64_t receiver, uint64_t code, uint64_t action)
    {

        if (code == name("eosio.token").value && action == name("transfer").value)
        {
            execute_action(name(receiver), name(code), &eosnameswaps::handle_transfer);
        }
        else if (code == receiver && action == name("sell").value)
        {
            execute_action(name(receiver), name(code), &eosnameswaps::sell);
        }
        else if (code == receiver && action == name("cancel").value)
        {
            execute_action(name(receiver), name(code), &eosnameswaps::cancel);
        }
        else if (code == receiver && action == name("updatesale").value)
        {
            execute_action(name(receiver), name(code), &eosnameswaps::updatesale);
        }
        else if (code == receiver && action == name("vote").value)
        {
            execute_action(name(receiver), name(code), &eosnameswaps::vote);
        }
        else if (code == receiver && action == name("decidebid").value)
        {
            execute_action(name(receiver), name(code), &eosnameswaps::decidebid);
        }
        else if (code == receiver && action == name("message").value)
        {
            execute_action(name(receiver), name(code), &eosnameswaps::message);
        }
        else if (code == receiver && action == name("screener").value)
        {
            execute_action(name(receiver), name(code), &eosnameswaps::screener);
        }
        else if (code == receiver && action == name("initstats").value)
        {
            execute_action(name(receiver), name(code), &eosnameswaps::initstats);
        }
        else if (code == receiver && action == name("cancelbid").value)
	    {
	        execute_action(name(receiver), name(code), &eosnameswaps::cancelbid);
	    }
        eosio_exit(0);
    }
}

} // namespace eosio
