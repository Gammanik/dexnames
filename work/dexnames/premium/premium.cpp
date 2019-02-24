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

//  action(permission_level{name(suffix_name), name("active")},
//         name("eosio"), name("newaccount"),
//         std::make_tuple(
//                 name("sex"),
//                 name("12341"),
//                 owner_auth,
//                 active_auth
//         )).send();
  
  const string accountToCreate = "nameswapsaab"; // todo: add suffix here?
  
  action(permission_level{name(_self), name("active")},
         name("eosio"), name("newaccount"),
         std::make_tuple(
             name(_self),
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


extern "C"
{
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  
  if (code == receiver && action == name("regname").value) {
    execute_action(name(receiver), name(code), &premium::regname);
  }
}


}

}// pace eosio