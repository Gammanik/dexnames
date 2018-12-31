# dexnames
a decentralized names auction based on EOS



Правила аукциона: 

Пользователь может выставить имя на продажу. Для выставления имени нужно указать {"account4sale","saleprice",
"asset", "paymentaccnt", "message"} и подписаться ключом от аккаунта который выставлен на продажу. 
paymentaccnt - аккаунт на который нужно будет перечислять деньги в случае удачной покупки выставленного аккаунта.


Далее покупатель для каждого выставленного имени может либо:

 - купить его за указанную цену. (для этого нужно перевести требуемое количество EOS за покупку на адрес данного смарт-контракта) либо
 
 - предложить свою цену (action::  "proposebid":: "fields": [{"name": "account4sale", "type": "name"},{"name": "bidprice","type": "asset"},{"name": "bidder","type": "name"}]


Если покупатель предложил свою ставку, то продавец может на нее либо согласиться, либо нет. Для того, чтобы дать на нее ответ, продавец отправляет: action:: "decidebid", "fields": [{"name": "account4sale","type": "name"},{"name": "accept","type": "bool"}]
где accept - true/false обозначает его решение.

Если продавец одобрил ставку, то покупателю посылается сообщение и он может перевести на контракт ту сумму, которую он предложил как ставку (вместо настоящей цены).
<br /><br /><br />











### IMPORTANT:
после выкладки контракта в сеть обязательно нужно послать пустой action initstats на имя контракта. 
Иначе таблица со статистикой не будет инициализирована и все действия будут крашиться. 

stats_table - для статистики внутри контракта     struct stats_table
```
{       
    // Index uint64_t 
    index; 

    // Number of accounts currently listed
    uint64_t num_listed;
    
    // Number of accounts purchased
    uint64_t num_purchased;

    // Total sales
    asset tot_sales;

    // Total sales fees
    asset tot_fees;

    uint64_t primary_key() const { return index; }
};   
```
    
    <br /><br /><br /><br />
    
    
    
## Теперь некоторые примеры с использованием cleos::  

А еще нужно дать контракту разрешение отправлять actions. Разрешить самому себе. Без этого не работало крч
```
cleos --url http://jungle2.cryptolions.io:80 push action eosio updateauth '{"account":"nameswapsln1","permission":"active","parent":"owner","auth":{"threshold": 1,"keys": [{"key":"EOS8g6Eb7nixavjSaKh3PSseDu5Az47Xhy2QN52h42KzNmH8FqR2M","weight":1}],"waits":[],"accounts": [{"permission":{"actor":"nameswapsln1","permission":"eosio.code"},"weight":1}]}}' -p nameswapsln1@owner
```


Перед совершением продажи аккаунта нужно обновить permissions. Здесь nameswapsln2 - аккаунт, который мы продаем. nameswapsln1 -  аккаунт, на котором расположен смарт-контракт.
```
cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80 push action eosio updateauth '{"account":"nameswapsln2","permission":"owner","parent":"","auth":{"threshold": 1,"keys": [{"key":"EOS8g6Eb7nixavjSaKh3PSseDu5Az47Xhy2QN52h42KzNmH8FqR2M","weight":1}],"waits":[],"accounts": [{"permission":{"actor":"nameswapsln1","permission":"eosio.code"},"weight":1}]}}' -p nameswapsln2@owner
```
<br /><br />

**Продажа аккаунта:** (продаем аккаунт nameswapsln3)
```
 cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80 push action nameswapsln1 sell '{ "account4sale":"nameswapsln3", "saleprice":"2.0000 EOS", "paymentaccnt":"gettingmoney","message":"Test"}' -p nameswapsln3@owner
```
Ответ:
```
executed transaction: 8af0ff21946d0de06a005a9abf4b56e1dc0e6e52705ec339c27f0316b3c0327a  136 bytes  1422 us
#  nameswapsln1 <= nameswapsln1::sell           {"account4sale":"nameswapsln3","saleprice":"2.0000 EOS","paymentaccnt":"gettingmoney","message":"Tes...
#         eosio <= eosio::updateauth            {"account":"nameswapsln3","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"ac...
#         eosio <= eosio::updateauth            {"account":"nameswapsln3","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"accounts...
#  nameswapsln1 <= nameswapsln1::message        {"receiver":"gettingmoney","message":"EOSNameSwaps: Your account nameswapsln3 has been listed for sa...
#  gettingmoney <= nameswapsln1::message        {"receiver":"gettingmoney","message":"EOSNameSwaps: Your account nameswapsln3 has been listed for sa...
```
<br /> <br />

**Отмена выставления аккаунта на продажу** (для него нужно авторизироваться с помощью аккаунта, который был указан как получающий деньги)
```
cleos --url http://jungle2.cryptolions.io:80 push action nameswapsln1 cancel '{"account4sale":"nameswapsln2", "owner_key":"EOS8g6Eb7nixavjSaKh3PSseDu5Az47Xhy2QN52h42KzNmH8FqR2M", "active_key":"EOS8g6Eb7nixavjSaKh3PSseDu5Az47Xhy2QN52h42KzNmH8FqR2M"}' -p gettingmoney@owner
```
<br /> <br />

**Показать на данный момент продающиеся аккаунты**
```
cleos --url http://jungle2.cryptolions.io:80 get table nameswapsln1 eosnameswaps accounts
```
<br /> <br />

**Предложить ставку:**
```
cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80 push action nameswapsln1 proposebid '[ "nameswapsln2", "1.5000 EOS", "sellswapsln1"]' -p sellswapsln1@owner
```
Ответ:
```
executed transaction: fa20b73a85163860a45326d43f12971da4e05b9ef4cb7456547c3966257d1eea  128 bytes  1096 us
#  nameswapsln1 <= nameswapsln1::proposebid     {"account4sale":"nameswapsln3","bidprice":"1.5000 EOS","bidder":"sellswapsln1"}
#  nameswapsln1 <= nameswapsln1::message        {"receiver":"gettingmoney","message":"EOSNameSwaps: Your account nameswapsln3 has received a bid. If...
#  gettingmoney <= nameswapsln1::message        {"receiver":"gettingmoney","message":"EOSNameSwaps: Your account nameswapsln3 has received a bid. If...
```
<br/><br/>


**Продавец определяет что ему делать с предложенной ставкой::**
```
cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80 push action nameswapsln1 decidebid '[ "nameswapsln3", true]'  -p gettingmoney@owner
```
<br/> <br/> <br/>



**Покупка аккаунта::
```
cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80 push action eosio.token transfer '[ "sellswap1", "gettingmoney", "1.5000 EOS", "sp:nameswapsln3"]' -p sellswapsln@active
```
Ответ::
```
Error 3090003: Provided keys, permissions, and delays do not satisfy declared authorizations
Ensure that you have the related private keys inside your wallet and your wallet is unlocked.
Error Details:
transaction declares authority '{"actor":"sellswapsln","permission":"active"}', but does not have signatures for it.
root@keosd:/work/dexnames# 
root@keosd:/work/dexnames# 
root@keosd:/work/dexnames# cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80 push action eosio.token transfer '[ "sellswapsln1", "gettingmoney", "1.5000 EOS", "sp:nameswapsln3"]' -p sellswapsln1@active
executed transaction: 909adc469a7453aec179ca2bb05f971635535a736ec4eb725d1345318c9a4a7f  144 bytes  628 us
#   eosio.token <= eosio.token::transfer        {"from":"sellswapsln1","to":"gettingmoney","quantity":"1.5000 EOS","memo":"sp:nameswapsln3"}
#  sellswapsln1 <= eosio.token::transfer        {"from":"sellswapsln1","to":"gettingmoney","quantity":"1.5000 EOS","memo":"sp:nameswapsln3"}
#  gettingmoney <= eosio.token::transfer        {"from":"sellswapsln1","to":"gettingmoney","quantity":"1.5000 EOS","memo":"sp:nameswapsln3"}
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
```
или такой::
```
lswapsln1", "gettingmoney", "1.5000 EOS", "sp: nameswapsln3"]' -p sellswapsln1@active
executed transaction: 0f3c5c3bcc8a702f9c95874410d57cdaca43e64fc1ebfdb38cd657b2eae1a376  144 bytes  477 us
#   eosio.token <= eosio.token::transfer        {"from":"sellswapsln1","to":"gettingmoney","quantity":"1.5000 EOS","memo":"sp: nameswapsln3"}
#  sellswapsln1 <= eosio.token::transfer        {"from":"sellswapsln1","to":"gettingmoney","quantity":"1.5000 EOS","memo":"sp: nameswapsln3"}
#  gettingmoney <= eosio.token::transfer        {"from":"sellswapsln1","to":"gettingmoney","quantity":"1.5000 EOS","memo":"sp: nameswapsln3"}
```
<br /> <br />


# Инфа по таблицам 

extras_table - в ней хранятся данные о голосах для аккаунта для голосований
bids - хранятся все ставки для всех имен и их статус (ACCEPTED/REJECTED)
