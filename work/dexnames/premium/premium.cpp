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

void premium::askprice(const name requester, const name nametobuy) {
  // check the rules
  eosio_assert(has_auth(requester), "Ask price Error: You're not who you say you are.");
  
  // increment the uuid of the ask
  
  uuid new_id = _next_id();
  
  // Cannot charge RAM to other accounts during notify
  _asks.emplace(_self, [&](asks_table &a) {
    a.id = new_id;
    a.nametobuy = nametobuy;
    a.requester = requester;
    a.asktime = now();
  });
  
  // todo: add deferred action in 3 days call expired ask
  
  
}


void premium::approveask(uuid id, asset price, name admin) {
  eosio_assert(has_auth(admin), "Ask price Error: You're not who you say you are.");
  // todo: add check if an admin from the table
  // _admins.find(admin) & assert

  auto itr_ask = _asks.find(id);
  eosio_assert(itr_ask != _asks.end(), ("Approve ask error: ask with a given id does not exists. id: " + std::to_string(id)).c_str());
  
  _responses.emplace(admin, [&](responses_table &a) {
    a.id = id;
    a.status = ASK_ACCEPTED;
    a.nametobuy = itr_ask->nametobuy;
    a.requester = itr_ask->requester;
    a.asktime = a.asktime;
    a.respondtime = a.respondtime;
    a.price = price;
  });

  _asks.erase(itr_ask);
  
  // todo: send a message to a itr_ask.requester that his ask was approved
  send_message(itr_ask->requester, "Nameos: your ask was approved. You have 3 days to buy the name");
  
  
  // todo: check if 3 days yet not expired (just in case)
}

void premium::declineask(uuid id, name admin) {
  eosio_assert(has_auth(admin), "Ask price Error: You're not who you say you are.");
  
  auto itr_ask = _asks.find(id);
  eosio_assert(itr_ask != _asks.end(), ("Approve ask error: ask with a given id does not exists. id: " + std::to_string(id)).c_str());
  
  _responses.emplace(admin, [&](responses_table &a) {
    a.id = id;
    a.status = ASK_DECLINED;
    a.nametobuy = itr_ask->nametobuy;
    a.requester = itr_ask->requester;
    a.asktime = a.asktime;
    a.respondtime = a.respondtime;
    a.price = asset(-1, symbol("EOS", 4));
  });
  
  _asks.erase(itr_ask);
  
  
  send_message(itr_ask->requester, "Nameos: your ask was declined. Sorry.");
  // todo: send the pledge back
  
}

void premium::buyname(uuid id, asset price, string active_key, string owner_key) {
  eosio_assert(false, "ths on is ok todo");
  auto itr_resp = _responses.find(id);
  eosio_assert(itr_resp != _responses.end(), string("Buy name error: no response for a given ask id. id: " + std::to_string(id)).c_str());
  
  // check if
}



// Message Action
void premium::send_message(const name receiver, const string message) {
  action(permission_level{_self, name("active")},
         name(_self), name("message"),
         std::make_tuple(
                 receiver,
                 message))
          .send();
}

void premium::message(const name receiver, const string message) {
  // Only the contract can send a message
  eosio_assert(has_auth(_self), "Message Error: Only the contract can send messages.");
  // Notify the specified account
  require_recipient(receiver);
}

//void premium::init() {
////  require_auth(_self);
//  ConfigSingleton.set(Config{}, _self);
//}

/* ****************************************** */
/* ------------ Private Functions ----------- */
/* ****************************************** */

uuid premium::_next_id() {
  Config state = _get_config();
  state.last_id = state.last_id + 1;
  eosio_assert(state.last_id > 0, "_next_id overflow detected");
  _update_config(state);
  
  return state.last_id;
};

premium::Config premium::_get_config() {
  premium::Config config;
  
  if (ConfigSingleton.exists()) {
    config = ConfigSingleton.get();
  } else {
    config = Config{};
    ConfigSingleton.set(config, _self);
  }
  
  return config;
}

void premium::_update_config(const premium::Config config) {
  ConfigSingleton.set(config, _self);
}




extern "C"
{
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  
  if (code == name("eosio.token").value && action == name("transfer").value) {
    execute_action(name(receiver), name(code), &premium::handle_transfer);
  } else if (code == receiver && action == name("regname").value) {
    execute_action(name(receiver), name(code), &premium::regname);
    
  } else if (code == receiver && action == name("approveask").value) {
    execute_action(name(receiver), name(code), &premium::approveask);
  } else if (code == receiver && action == name("declineask").value) {
    execute_action(name(receiver), name(code), &premium::declineask);
    
  } else if (code == receiver && action == name("askprice").value) {
    execute_action(name(receiver), name(code), &premium::askprice);
  }

}


}

}// pace eosio