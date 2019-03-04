#include "premium.hpp"


namespace eosio {

void premium::regname(const name newname, const string owner_key_str, const string active_key_str) {
  
  const abieos::public_key active_key = abieos::string_to_public_key(active_key_str);
  const abieos::public_key owner_key = abieos::string_to_public_key(owner_key_str);
  
  const array<uint8_t, 33> active_pubkey = active_key.data;
  const array<uint8_t, 33> owner_pubkey = owner_key.data;
  
  array<char, 33> active_pubkey_char;
  std::copy(active_pubkey.begin(), active_pubkey.end(), active_pubkey_char.begin());
  array<char, 33> owner_pubkey_char;
  std::copy(owner_pubkey.begin(), owner_pubkey.end(), owner_pubkey_char.begin());
  
  
  const eosiosystem::authority owner_auth = eosiosystem::authority{
          1, {{{(uint8_t)abieos::key_type::k1, owner_pubkey_char}, 1}}, {}, {}};
  const eosiosystem::authority active_auth = eosiosystem::authority{
          1, {{{(uint8_t)abieos::key_type::k1, active_pubkey_char}, 1}}, {}, {}};
//          1, {{1, 1}}, {}, {}}; //
  
  
  
  const std::string suffix_name = "sex";
  // todo: add a table with available suffixes
  // eosio_assert(itr_suffixes != _suffixes, "You are trying to register a name with an unavailable suffix");
  
  const name accountToCreate = newname;
//          "nameswap.sex"; //
  
  action(permission_level{name(suffix_name), name("active")},
       name("eosio"), name("newaccount"),
       std::make_tuple(
               name("sex"),
               accountToCreate,
               owner_auth,
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
                 accountToCreate,
                 ramAmount // todo: out it in the config/singleton
         )).send();
  
  action(permission_level{name(_self), name("active")},
         name("eosio"), name("delegatebw"),
         std::make_tuple(
                 name(_self),
                 accountToCreate,
                 net, cpu, 1
         )).send();
  
  
}
  
void premium::handle_transfer(name from, name to, asset quantity, string memo) {
  // Important: The transfer fees actions below will trigger this function without this
  if (from == _self) return;
  // EOSBet hack
  eosio_assert(to == _self, "Buy Error: Transfer must be direct to contract.");
  
  // Check the transfer is valid
  eosio_assert(quantity.symbol == symbol("EOS", 4), "Buy Error: You must pay in EOS.");
  eosio_assert(quantity.is_valid(), "Buy Error: Quantity is not valid.");
  
  // Important: The transfer fees actions below will trigger this function without this
  if (from == _self) return;
  
  const int codeLen = 4;
  const string transfer_code = memo.substr(0, codeLen);
  // buy_code == "fii" in case if you want to send money on the conract then just write nothing in memo
  eosio_assert(transfer_code == "ask:" || transfer_code == "buy:" || transfer_code == "", ("Ask error: Malformed ask code: " + transfer_code).c_str());
  
  if (transfer_code == "") {
    return;
  }
  
  // Strip buy code from memo
  memo = memo.substr(codeLen);
  
  
  if (transfer_code == "ask:") {
    Config conf = _get_config();
    eosio_assert(conf.askdeposit == quantity, ("Handle transfer err: wrong deposit amount. Ask deposit is: " + std::to_string(conf.askdeposit.amount)).c_str());
    const name nameasked = name(memo.substr(0, memo.length()));
    askprice(from, nameasked);
  } else if (transfer_code == "buy:") {
    // Find the length of the auction id
    int id_length = 0;
    for (int lp = 1; lp <= 120; ++lp) {
      if (memo[lp] == ',') {
        id_length = lp;
        break;
      }
    }
    eosio_assert(id_length > 0, "Buy Error: Malformed auction id.");
  
    // Extract id
    const string id_string = memo.substr(0, id_length);
    const uuid id = std::strtoull(id_string.c_str(),NULL,0);
  
    // Extract public key
    const string active_key_string = memo.substr(id_length + 1, KEY_LENGTH);
    const string owner_key_string = memo.substr(id_length + 2 + KEY_LENGTH, KEY_LENGTH);
    
    buyname(id, quantity, active_key_string, owner_key_string);
  }
}

void premium::askprice(const name requester, const name nametobuy) {
  // check the rules
  eosio_assert(has_auth(requester), "Ask price Error: You're not who you say you are.");
  eosio_assert(!is_account(nametobuy), "Ask price Error: The payment account you are requesting is already exists.");
  
//   increment the uuid of the ask
  uuid new_id = _next_id();
  
  // Cannot charge RAM to other accounts during notify
  _asks.emplace(_self, [&](asks_table &a) {
    a.id = new_id;
    a.nametobuy = nametobuy;
    a.requester = requester;
    a.asktime = now();
  });
  
  Config conf = _get_config();
  // deferred action in 3 days call expired ask
  transaction out{};
  out.actions.emplace_back(permission_level{_self, name("active")}, name(_self), name("expireask"),
                           std::make_tuple(new_id));
  out.delay_sec = conf.askexpiration; // todo: conf.askexpiration;
  out.send(new_id, _self);
  
}


void premium::approveask(uuid id, asset price, name admin) {
  eosio_assert(has_auth(admin), "Ask price Error: You're not who you say you are.");
  
  // Check the transfer is valid
  eosio_assert(price.symbol == symbol("EOS", 4), "Approve ask Error: Price must be in EOS.");
  eosio_assert(price.is_valid(), "Approve ask Error: Price is not valid.");
  
  Config conf = _get_config();
  eosio_assert(price > conf.askdeposit, ("Approve ask error: price should be bigger than the askdeposit. askdeposit: " + std::to_string(price.amount)).c_str());
  // todo: add check if an admin from the table
  // _admins.find(admin) & assert

  auto itr_ask = _asks.find(id);
  eosio_assert(itr_ask != _asks.end(), ("Approve ask error: ask with a given id does not exists. id: " + std::to_string(id)).c_str());
  
  _responses.emplace(admin, [&](responses_table &a) {
    a.id = id;
    a.status = ASK_ACCEPTED;
    a.nametobuy = itr_ask->nametobuy;
    a.requester = itr_ask->requester;
    a.asktime = itr_ask->asktime;
    a.respondtime = now();
    a.price = price;
  });

  _asks.erase(itr_ask);
  
  send_message(itr_ask->requester, "Nameos: your ask was approved. You have 3 days to buy the name");
  
  // todo: cancel expireask
//  cancel_deferred()
  
  // todo: check if 3 days yet not expired (just in case)
  // todo: add deferred expirebuy
//  Config conf = _get_config();
//  transaction out{};
//  out.actions.emplace_back(permission_level{_self, name("active")}, name(_self), name("expirebuy"),
//                           std::make_tuple(new_id));
//  out.delay_sec = 10; // todo: conf.askexpiration;
//  out.send(new_id, _self);
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
    a.asktime = itr_ask->asktime;
    a.respondtime = now(); // todo: now() is returning 0??
    a.price = asset(0, symbol("EOS", 4)); // todo: figure out
  });
  
  Config conf = _get_config();
  // send the deposit amount back
  action(permission_level{_self, name("active")},
    name("eosio.token"), name("transfer"),
    std::make_tuple(_self, itr_ask->requester, conf.askdeposit,
       std::string("Nameos: Your request was declined. Return deposit for a name: " + itr_ask->nametobuy.to_string())))
    .send();
  
  
  _asks.erase(itr_ask);
  send_message(itr_ask->requester, "Nameos: your ask was declined. Sorry.");
  
  // todo: cancel expireask
//  cancel_deferred(id)

}

void premium::expireask(uuid id) {
  
  auto itr_ask = _asks.find(id);
  eosio_assert(itr_ask != _asks.end(), ("Expire ask error: ask with a given id does not exists. id: " + std::to_string(id)).c_str());
  eosio_assert(has_auth(_self) || has_auth(itr_ask->requester), "Expire ask Error: Only requester or contract can execute this actions.");
  
  // it will fall if response is already given
  _responses.emplace(_self, [&](responses_table &a) {
    a.id = id;
    a.status = ASK_EXPIRED;
    a.nametobuy = itr_ask->nametobuy;
    a.requester = itr_ask->requester;
    a.asktime = itr_ask->asktime;
    a.respondtime = now();
    a.price = asset(0, symbol("EOS", 4));
  });
  
  Config conf = _get_config();
  // send the deposit amount back
  action(permission_level{_self, name("active")},
         name("eosio.token"), name("transfer"),
         std::make_tuple(_self, itr_ask->requester, conf.askdeposit,
                         std::string("Nameos: Your request has expired. Return deposit for a name: " + itr_ask->nametobuy.to_string())))
          .send();
  
  // just remove the ask from the table
  _asks.erase(itr_ask);
}

void premium::tstexp(uuid id) {
  Config conf = _get_config();
  transaction out{};
  out.actions.emplace_back(permission_level{_self, name("active")}, name(_self), name("message"),
                           std::make_tuple(name("nameswapsaa1"), string("Hello with a delay")));
  out.delay_sec = 10;
  out.send(id, _self);
}

void premium::buyname(uuid id, asset sent_amount, string active_key, string owner_key) {
  auto itr_resp = _responses.find(id);
  eosio_assert(itr_resp != _responses.end(), ("Buy name error: no response for a given ask id. id: " + std::to_string(id)).c_str());
  eosio_assert(itr_resp->status == ASK_ACCEPTED, ("Buy name error: your ask should be accepted in order to buy the name"));
  
  Config conf = _get_config();
  
  // check for the price
  // TODO:::  the amount is divided by 10. Why??
  eosio_assert(sent_amount != itr_resp->price - conf.askdeposit, ("Wrong amount sent. You need to transfer:" + std::to_string(itr_resp->price.amount - conf.askdeposit.amount)).c_str());
  regname(itr_resp->nametobuy , active_key, owner_key);
  
  _responses.erase(itr_resp);
  send_message(itr_resp->requester, "Nameos: thank you for buying premium name. Come again!");
}

void premium::declinebuy(uuid id) {
  auto itr_resp = _responses.find(id);
  eosio_assert(itr_resp != _responses.end(), ("Decline error: no response for a given ask id. id: " + std::to_string(id)).c_str());
  eosio_assert(itr_resp->status == ASK_ACCEPTED, ("Decline error: your ask should be accepted in order to decline it"));
  
  // todo: add a stats table like: num_declined, num_sold
  send_message(itr_resp->requester, string("Nameos: you declined a name: " + itr_resp->nametobuy.to_string()));
  _responses.erase(itr_resp);
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

void premium::init() {
  require_auth(_self);
  ConfigSingleton.set(Config{}, _self);
}

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
//  eosio_assert(has_auth(_self), "Message Error: Only the contract can update config.");
  ConfigSingleton.set(config, _self);
}

void premium::deleteconfig() {
  eosio_assert(has_auth(_self), "Message Error: Only the contract can delete config.");
  eosio_assert(ConfigSingleton.exists(), "Nothing to delete. The config does not exists.");
  ConfigSingleton.remove();
}

void premium::droptable(string table) {
  if (table == "asks") {
    for(auto itr = _asks.begin(); itr != _asks.end();) {
      itr = _asks.erase(itr);
    }
  } else if (table == "responses") {
    for(auto itr = _responses.begin(); itr != _responses.end();) {
      itr = _responses.erase(itr);
    }
  } else {
    eosio_assert(false, ("nameos: no option to delete table: " + table).c_str());
  }
}

void premium::fixid(const uuid newid) {
  eosio_assert(has_auth(_self), "Only a contract owner can fix the id");
  
  Config state = _get_config();
  state.last_id = newid;
  eosio_assert(state.last_id > 0, "_next_id overflow detected");
  _update_config(state);
}




extern "C" {
void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  
  if (code == name("eosio.token").value && action == name("transfer").value) {
    execute_action(name(receiver), name(code), &premium::handle_transfer);
  } else if (code == receiver && action == name("regname").value) {
    execute_action(name(receiver), name(code), &premium::regname);
    
  } else if (code == receiver && action == name("approveask").value) {
    execute_action(name(receiver), name(code), &premium::approveask);
  } else if (code == receiver && action == name("declineask").value) {
    execute_action(name(receiver), name(code), &premium::declineask);
  } else if (code == receiver && action == name("expireask").value) {
    execute_action(name(receiver), name(code), &premium::expireask);
    
    
  } else if (code == receiver && action == name("askprice").value) {
    execute_action(name(receiver), name(code), &premium::askprice);
  
    /// for a user
  } else if (code == receiver && action == name("declinebuy").value) {
    execute_action(name(receiver), name(code), &premium::declinebuy);
    
    
    /// for an admin
  } else if (code == receiver && action == name("deleteconfig").value) {
    execute_action(name(receiver), name(code), &premium::deleteconfig);
  } else if (code == receiver && action == name("init").value) {
    execute_action(name(receiver), name(code), &premium::init);
  } else if (code == receiver && action == name("droptable").value) {
    execute_action(name(receiver), name(code), &premium::droptable);
    
    
    /// testing purposes
  } else if (code == receiver && action == name("tstexp").value) {
    execute_action(name(receiver), name(code), &premium::tstexp);
  }
  
  
  
  
  eosio_exit(0);
}


}

}// pace eosio