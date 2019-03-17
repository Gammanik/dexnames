// #include <eosiolib/eosio.hpp>
// #include <eosiolib/print.hpp>
// #include <string>


#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/public_key.hpp>
#include <eosiolib/public_key.hpp>
#include <eosiolib/crypto.h>
#include <eosiolib/transaction.hpp>
#include "includes/eosio.system.hpp"
#include "includes/abieos_numeric.hpp"
#include <string>
#include <array>

//using namespace eosio;

namespace eosiosystem {
    class system_contract;
}

using std::string;
using std::array;


namespace eosio {
  
  typedef uint64_t uuid;
  typedef uint64_t unix_time;
  
//  struct regname_type { // todo: DELETE THE SHIT
//      name newname;
//      eosio::public_key ownerkey;
//      eosio::public_key activekey;
//  };
  
  class [[eosio::contract]] premium : public contract {
  public:
  
    // ask decision responses //todo: make them as strings?
//    const uint8_t ASK_ACCEPTED = "accepted";
    const uint8_t ASK_ACCEPTED = 0;
    const uint8_t ASK_DECLINED = 1;
    const uint8_t ASK_EXPIRED = 2;
  
    const uint16_t KEY_LENGTH = 53;
    
    // Constructor
    premium(name self, name code, datastream<const char *> ds) : eosio::contract(self, code, ds),
    ConfigSingleton(_self, _self.value),
    _asks(_self, _self.value),
    _responses(_self, _self.value)
    { }
      
//    [[eosio::action]]
    void regname(const name newname, const string owner_key_string, const string active_key_string);
    
    void handle_transfer(const name from, const name to, const asset quantity, const string memo);
    // user is asking for a price
    [[eosio::action]] // todo: temporary
    void askprice(const name account, const name nametobuy);
  
    [[eosio::action]]
    void init();
    
    // approve ask with a given id and set a price to
    [[eosio::action]]
    void approveask(uuid id, asset price, name admin);
    [[eosio::action]] // admin declined
    void declineask(uuid id, name admin);
    [[eosio::action]] // if an ask has expired (3 days has gone)
    void expireask(uuid id);
    
    // todo:: temporary
    [[eosio::action]]
    void tstexp(uuid id);
//
//    // managing the contract
//    [[eosio::action]] // add a person who can make decisions (approve or reject asks)
//    void addadmin(name newadmin);
//    [[eosio::action]]
//    void deleteadmin(name admin);
    
    
    // user decided to buy the name for a given price
    void buyname(uuid id, asset price, string active_key, string owner_key);
    // user decided to decline the name for a given price
    [[eosio::action]]
    void declinebuy(uuid id);
  
  // ----------------------------------------------
  // Actions for an admins
  // ----------------------------------------------
    [[eosio::action]]
    void deleteconfig();
    [[eosio::action]]
    void droptable(string table);
    // modify the suffix table
//    [[eosio::action]]
//    void addsuffix();
//    [[eosio::action]]
//    void deletesuffix();
  

  private:
  
    struct [[eosio::table("settings")]] Config {
      uuid last_id = 1;
      asset	askdeposit = asset(2000, symbol("EOS", 4));
      // 3 days for ask expiration in seconds
      unix_time askexpiration =  60; // todo: 3*24*36
      // 3 days for response expiration in seconds
      unix_time responseexp = 6000; // todo: 3*24*36
    };
    typedef singleton<name("settings"), Config> SingletonType;
    SingletonType ConfigSingleton;
  
    // accessors
    premium::Config _get_config();
    void _update_config(const premium::Config config);
    uuid _next_id();
    [[eosio::action]]
    void fixid(const uuid newid);
    
    void send_message(const name receiver, const string message);
    [[eosio::action]]
    void message(const name receiver, const string message);
    
    struct [[eosio::table]] asks_table {
      uuid id; // index
      // asked name in format **.suffix
      name nametobuy;
      // the account applied to buy a name
      name requester;
      // the time the request is created at (to count 3 days from it)
      unix_time asktime;
  
      uint64_t primary_key() const { return id; }
    };
    eosio::multi_index<name("asks"), asks_table> _asks;
  
  
    struct [[eosio::table]] responses_table {
      uuid id; // index
      // asked name in format **.suffix
      name nametobuy;
      // the account applied to buy a name
      name requester;
      // the time the request was created
      unix_time asktime;
      // the time the administrator responded
      unix_time respondtime;
      // asked price for a premium name
      asset price;
      
      uint8_t status; // expired | approved | rejected
      
      uint64_t primary_key() const { return id; }
    };
    eosio::multi_index<name("responses"), responses_table> _responses;
  };
  
  
  
  
  
}