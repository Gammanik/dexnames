#!/bin/sh



# /bin/sh -c   "nodeos -p eosio \
#     -d /mnt/dev/data \
#     --config-dir /mnt/dev/config"



# docker-compose exec nodeos bash
alias cleos='docker-compose exec keosd /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://0.0.0.0:9876'


# REG USER::
cleos wallet create --to-console
cleos wallet unlock --password PW5HypDWZ2y9ZdPSnUZSWoCtPc2p9e5QWs4DTh6vNGis2h3guYBBZ



cleos create key  --to-console
# Private key: 
# Public key: 

cleos wallet import --private-key [private key]

# private-key for eosio (in case of local net)- (eosio public key: EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV)
cleos wallet import --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3
# cleos wallet keys 
# cleos get accounts [public key of imported wallet]
# cleos wallet unlock [master-password PW5J6DKtpNDSjwDC3nuoNfGgGULxfPcNBKNJu47Lu57ZjABWwRDNb]
cleos --url https://api.main.alohaeos.com:443 get account eosnamesbids
cleos --url http://jungle2.cryptolions.io:80 set contract nameswapsai1  ./eosnameswaps/
# cleos create account eosio user [pub_key pub_key]



# compile smartcontract using eosio.cdt:: 
eosio-cpp -abigen [name.cpp] -o [name.wasm]
eosio-abigen hello.cpp --contract=hello --output=hello.abi

cleos --url https://api.main.alohaeos.com:443 push action nameosmainsc initstats '{}' -p nameosmainsc
cleos --url http://jungle2.cryptolions.io:80 system bidname collectmoney sex '101 EOS' -p collectmoney

# the most activey used
cleos set contract USERNAME work/contracts/eosio.bios -p eosio


cleos push action eosio proposebid '{"account": "eosio", "action": "proposebid", "data": ""}' -p eosio


#publishing the shit
cleos --url https://api.main.alohaeos.com:443  set contract eosnamesbids ./eosnameswaps/ -p eosnamesbids


cleos --url http://jungle2.cryptolions.io:80 system buyram collectmoney  nameswapsai1 '50 EOS' -p collectmoney
cleos --url http://jungle2.cryptolions.io:80 system buyram premiumtest1  premiumtest1 '50 EOS' -p premiumtest1




cleos --url http://jungle2.cryptolions.io:80 get info
cleos --url  https://api.main.alohaeos.com:443 get account arealgangsta
cleos --url http://jungle2.cryptolions.io:80  set contract arealgangsta ./eosnameswaps/
cleos --url https://api.main.alohaeos.com:443 push action arealgangsta screener '["eosnameswap1",1]'  -p arealgangsta

cleos --url https://api.main.alohaeos.com:443 push action eosnamesbid buy




cleos --url http://jungle2.cryptolions.io:80 push action nameswapsln1 sell '[ "nameswapsaf2", "10.0000 EOS", "collectmoney","Test"]' -p nameswapsaf2@owner


cleos --url http://jungle2.cryptolions.io:80 push action eosio updateauth '{"account":"nameswapsln2","permission":"owner","parent":"","auth":{"threshold": 1,"keys": [{"key":"EOS8g6Eb7nixavjSaKh3PSseDu5Az47Xhy2QN52h42KzNmH8FqR2M","weight":1}],"waits":[],"accounts": [{"permission":{"actor":"nameswapsln1","permission":"eosio.code"},"weight":1}]}}' -p nameswapsln2@owner
cleos --url http://jungle2.cryptolions.io:80 push action eosio updateauth '{"account":"nameswapsln3","permission":"owner","parent":"","auth":{"threshold": 1,"keys": [{"key":"EOS8g6Eb7nixavjSaKh3PSseDu5Az47Xhy2QN52h42KzNmH8FqR2M","weight":1}],"waits":[],"accounts": [{"permission":{"actor":"nameswapsln1","permission":"eosio.code"},"weight":1}]}}' -p nameswapsln3@owner
cleos --url http://jungle2.cryptolions.io:80 push action eosio updateauth '{"account":"nameswapsln4","permission":"owner","parent":"","auth":{"threshold": 1,"keys": [{"key":"EOS8g6Eb7nixavjSaKh3PSseDu5Az47Xhy2QN52h42KzNmH8FqR2M","weight":1}],"waits":[],"accounts": [{"permission":{"actor":"nameswapsln1","permission":"eosio.code"},"weight":1}]}}' -p nameswapsln4@owner


cleos  --u http://jungle2.cryptolions.io:80 push action nameswapsln1 sell '[ "nameswapsln2", "1.0000 EOS", "gettingmoney","Test"]' -p nameswapsln2@owner
cleos --u http://jungle2.cryptolions.io:80 push action nameswapsln1 proposebid '[ "nameswapsln2", "1.5000 EOS", "sellswapsln1"]' -p sellswapsln1@owner

cleos  --u http://jungle2.cryptolions.io:80 push action nameswapsln1 updatesale '{ "account4sale":"nameswapsln2", "saleprice":"5.5000 EOS", "message":"Test2"}' -p nameswapsln2@active

# Give contract permission to send actions
cleos --url https://api.main.alohaeos.com:443 push action eosio updateauth '{"account":"nameosmainsc","permission":"active","parent":"owner","auth":{"threshold": 1,"keys": [{"key":"EOS6c4xog78xYFTmxGbGowvPE5kyY9bCLqkA27Yz27mZAZ62btFas","weight":1}],"waits":[],"accounts": [{"permission":{"actor":"nameosmainsc","permission":"eosio.code"},"weight":1}]}}' -p nameosmainsc@owner
cleos --url https://api.main.alohaeos.com:443 push action eosio updateauth '{"account":"nameosmainsc","permission":"owner","parent":"","auth":{"threshold": 1,"keys": [{"key":"EOS6c4xog78xYFTmxGbGowvPE5kyY9bCLqkA27Yz27mZAZ62btFas","weight":1}],"waits":[],"accounts": [{"permission":{"actor":"nameosmainsc","permission":"eosio.code"},"weight":1}]}}' -p nameosmainsc@owner

cleos --url http://jungle2.cryptolions.io:80 get table nameswapsab1 nameswapsab1 accounts


cleos --url http://jungle2.cryptolions.io:80 push action eosio.token transfer '{"from":"collectmoney", "to":"sex", "quantity":"100.0000 EOS", "memo":"fil"}' -p collectmoney
cleos --url https://api.main.alohaeos.com:443 push action eosio.token transfer '{"from":"arealgangsta", "to":"nameosmainsc", "quantity":"0.0001 EOS", "memo":"fil"}' -p arealgangsta


cleos --url http://jungle2.cryptolions.io:80 push action nameswapsai1 cancelbid '{"guid":"0"}'

# clear the account from the contract
cleos --url https://api.main.alohaeos.com:443 set contract -c eosnamesbids