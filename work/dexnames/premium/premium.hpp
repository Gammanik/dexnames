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
  
  struct regname_type { // todo: make id (or better list of id's)
      name newname;
      eosio::public_key ownerkey;
      eosio::public_key activekey;
  };
  
  class [[eosio::contract]] premium : public contract {
  public:
    // Constructor
    premium(name self, name code, datastream<const char *> ds) : eosio::contract(self, code, ds) {}
      
    [[eosio::action]]
    void regname(const regname_type &regname_data);
    
  };
  
  
}