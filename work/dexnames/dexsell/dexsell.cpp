#include "dexsell.hpp"

#include <eosiolib/asset.hpp>


using namespace eosio; //todo: do not use

#include <eosiolib/eosio.hpp>

class nameofclass : public eosio::contract {
  public:
    using contract::contract;

    void action1() {
        /*action body*/
      print("Ciao, ");
    };

  private:
  // @abi table users
  struct table_name {
    uint64_t key;
    /*more fields here*/

    auto primary_key() const { return key; }
  };
};

EOSIO_DISPATCH( nameofclass, (action1) )
