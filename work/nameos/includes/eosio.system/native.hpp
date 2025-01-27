/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/action.hpp>
#include <eosiolib/public_key.hpp>
#include <eosiolib/transaction.hpp> //it was eosiolib/transaction before
#include <eosiolib/print.hpp>
#include <eosiolib/privileged.h>
//#include <eosiolib/optional.hpp> // Removed eosiolib/optional.hpp. Use std::optional as a replacement.
#include <eosiolib/producer_schedule.hpp>
#include <eosiolib/contract.hpp>

namespace eosiosystem {
   using eosio::permission_level;
   using eosio::public_key;

   typedef std::vector<char> bytes;

   struct permission_level_weight {
      permission_level  permission;
       uint16_t      weight; //todo: was    weight_type       weight; //todo: was


       // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( permission_level_weight, (permission)(weight) )
   };

   struct key_weight {
      public_key   key;
      uint16_t weight; //todo: was    weight_type       weight

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( key_weight, (key)(weight) )
   };

   struct authority {
      uint32_t                              threshold;
      uint32_t                              delay_sec;
      std::vector<key_weight>               keys;
      std::vector<permission_level_weight>  accounts;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( authority, (threshold)(delay_sec)(keys)(accounts) )
   };

   struct block_header {
      uint32_t                                  timestamp;
      capi_name                              producer; //todo: was account_name
      uint16_t                                  confirmed = 0;
       //https://github.com/EOSIO/eos/blob/master/contracts/eosiolib/types.h#L113
       capi_checksum256                             previous; //todo: was block_id_type - removed
      capi_checksum256                               transaction_mroot;
      capi_checksum256                               action_mroot;
      uint32_t                                  schedule_version = 0;
      std::optional<eosio::producer_schedule> new_producers; //todo: Removed eosiolib/optional.hpp. Use std::optional as a replacement.

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE(block_header, (timestamp)(producer)(confirmed)(previous)(transaction_mroot)(action_mroot)
                                     (schedule_version)(new_producers))
   };


   /*
    * Method parameters commented out to prevent generation of code that parses input data.
    */
   class native : public eosio::contract {
      public:

         using eosio::contract::contract;

         /**
          *  Called after a new account is created. This code enforces resource-limits rules
          *  for new accounts as well as new account naming conventions.
          *
          *  1. accounts cannot contain '.' symbols which forces all acccounts to be 12
          *  characters long without '.' until a future account auction process is implemented
          *  which prevents name squatting.
          *
          *  2. new accounts must stake a minimal number of tokens (as set in system parameters)
          *     therefore, this method will execute an inline buyram from receiver for newacnt in
          *     an amount equal to the current new account creation fee.
          */
         void newaccount( capi_name     creator,
                          capi_name     newact
                          /*  no need to parse authorites
                          const authority& owner,
                          const authority& active*/ );


         void updateauth( /*account_name     account,
                                 permission_name  permission,
                                 permission_name  parent,
                                 const authority& data*/ ) {}

         void deleteauth( /*account_name account, permission_name permission*/ ) {}

         void linkauth( /*account_name    account,
                               account_name    code,
                               action_name     type,
                               permission_name requirement*/ ) {}

         void unlinkauth( /*account_name account,
                                 account_name code,
                                 action_name  type*/ ) {}

         void canceldelay( /*permission_level canceling_auth, transaction_id_type trx_id*/ ) {}

         void onerror( /*const bytes&*/ ) {}

   };
}
