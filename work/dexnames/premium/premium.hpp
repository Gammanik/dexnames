// #include <eosiolib/eosio.hpp>
// #include <eosiolib/print.hpp>
// #include <string>


#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/public_key.hpp>
#include <eosiolib/public_key.hpp>
#include <eosiolib/crypto.h>
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
  
  typedef uint32_t uuid;
  
  struct regname_type { // todo: DELETE THE SHIT
      name newname;
      eosio::public_key ownerkey;
      eosio::public_key activekey;
  };
  
  class [[eosio::contract]] premium : public contract {
  public:
    const uint16_t KEY_LENGTH = 53;
    
    // Constructor
    premium(name self, name code, datastream<const char *> ds) : eosio::contract(self, code, ds) {}
      
    [[eosio::action]]
    void regname(const regname_type &regname_data);
    
    [[eosio::action]]
    void handle_transfer(name from, name to, asset quantity, string memo);
    // user is asking for a price
    void askprice(name account, name nameasked);
    // user decided to buy the name for a given price
    void buyname(uuid id);
  
    // if an ask has expired ()
    [[eosio::action]]
    void expiredask(uuid id);
  
    [[eosio::action]]
    void expiredauction(uuid id, asset price);
  
    [[eosio::action]]
    void approve(uuid id, asset price);
  
    [[eosio::action]]
    void reject(uuid id);
    
  };
  
  
}