#include "premium.hpp"


namespace eosio {

void premium::regname(const regname_type &regname_data) {
  
  // todo: hardcode an account who has control?
  eosio_assert(has_auth(_self), "Regname error: only allowed account can register name.");

  const array<char, 33> owner_pubkey_char = regname_data.ownerkey.data;
  const array<char, 33> active_pubkey_char = regname_data.activekey.data;
  
//  const eosiosystem::authority owner_auth = eosiosystem::authority{
//          1, {{(uint8_t)abieos::key_type::k1, owner_pubkey_char}}, {}, {}};
//          1, {{{(uint8_t)abieos::key_type::k1, owner_pubkey_char}, 1}}, {}, {}};
  const eosiosystem::authority active_auth = eosiosystem::authority{
          1, {{{(uint8_t)abieos::key_type::k1, active_pubkey_char}, 1}}, {}, {}};
//          1, {{1, 1}}, {}, {}};
  
  
  
  const std::string suffix_name = "sex";
  
  const string accountToCreate = "nameswap.sex"; // todo: add suffix here?
  
  action(permission_level{name(suffix_name), name("active")},
       name("eosio"), name("newaccount"),
       std::make_tuple(
               name("sex"),
               name(accountToCreate),
               active_auth,
               active_auth
       )).send();

  const auto ramAmount = asset(500, symbol("EOS", 4)); // todo: out it in the config/singleton
  const auto cpu = asset(1500, symbol("EOS", 4));
  const auto net = asset(500, symbol("EOS", 4));
  
  // todo: use buyrambytes "uint32" instead? 2700 bytes is enough
  action(permission_level{name(_self), name("active")},
         name("eosio"), name("buyram"),
         std::make_tuple(
                 name(_self),
                 name(accountToCreate),
                 ramAmount // todo: out it in the config/singleton
         )).send();
  
  action(permission_level{name(_self), name("active")},
         name("eosio"), name("delegatebw"),
         std::make_tuple(
                 name(_self),
                 name(accountToCreate),
                 net, cpu, 1
         )).send();
  
  
}
  
void premium::handle_transfer(name from, name to, asset quantity, string memo) {
  // Important: The transfer fees actions below will trigger this function without this
  if (from == _self) return;
  // EOSBet hack
  eosio_assert(to == _self, "Buy Error: Transfer must be direct to contract.");
  // todo: add assert for quantity
  
  const int codeLen = 4;
  const string transfer_code = memo.substr(0, codeLen);
  // buy_code == "fii" in case if you want to send money on the conract then just write nothing in memo
  eosio_assert(transfer_code == "ask:" || transfer_code == "", ("Ask error: Malformed ask code: " + transfer_code).c_str());
  
  if (transfer_code == "") {
    return;
  }
  
  // Strip buy code from memo
  memo = memo.substr(codeLen);
  
  
  if (transfer_code == "ask:") {
//    const string owner_key = memo.substr(0, KEY_LENGTH);
//    const string active_key = memo.substr(KEY_LENGTH + 1, 2*KEY_LENGTH + 1);
    
    const name nameasked = name(memo.substr(0, memo.length()));
    askprice(from, nameasked);
  } else if (transfer_code == "buy:") {
  
  }
}

void premium::askprice(name account, name nameasked) {
  // check the rules
  
  
  
}

void premium::buyname(uuid id) {
  // check the rules
  
  
}


//void premium::




extern "C"
{
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  
  if (code == name("eosio.token").value && action == name("transfer").value) {
    execute_action(name(receiver), name(code), &premium::handle_transfer);
  } else if (code == receiver && action == name("regname").value) {
    execute_action(name(receiver), name(code), &premium::regname);
  } else if (code == receiver && action == name("regname").value) {
    execute_action(name(receiver), name(code), &premium::regname);
  }
}


}

}// pace eosio