#!/bin/sh



# /bin/sh -c   "nodeos -p eosio \
#     -d /mnt/dev/data \
#     --config-dir /mnt/dev/config"



# docker-compose exec nodeos bash
alias cleos='docker-compose exec keosd /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://0.0.0.0:9876'


# REG USER::
cleos wallet create --to-console
# cleos wallet unlock --password PW5Kf11xWoGUxzwgRfUWFcnUEqxacXffbeAwvVYzxBygNAssCuNzA



cleos create key  --to-console
# Private key: 
# Public key: 

cleos wallet import --private-key [private key]

# private-key for eosio (in case of local net)- (eosio public key: EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV)
cleos wallet import --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3
# cleos wallet keys 
# cleos get accounts [public key of imported wallet]
# cleos wallet unlock [master-password PW5J6DKtpNDSjwDC3nuoNfGgGULxfPcNBKNJu47Lu57ZjABWwRDNb]
cleos --wallet-url http://0.0.0.0:9876/ --url  https://api.main.alohaeos.com:443 get account eosnamesbids
# cleos create account eosio user [pub_key pub_key]



# compile smartcontract using eosio.cdt:: 
eosio-cpp -abigen [name.cpp] -o [name.wasm]
eosio-abigen hello.cpp --contract=hello --output=hello.abi

# the most activey used
cleos set contract USERNAME work/contracts/eosio.bios -p eosio


cleos push action eosio proposebid '{"account": "eosio", "action": "proposebid", "data": ""}' -p eosio


#publishing the shit
cleos --url https://api.main.alohaeos.com:443  set contract eosnamesbids ./eosnameswaps/ -p eosnamesbids






cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80 get info
cleos --wallet-url http://0.0.0.0:9876 --url  https://api.main.alohaeos.com:443 get account arealgangsta
cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80  set contract arealgangsta ./eosnameswaps/
cleos --url https://api.main.alohaeos.com:443 push action arealgangsta screener '["eosnameswap1",1]'  -p arealgangsta






push action nameswapsln1 sell '[ "eosnameswap1", "10.0000 EOS", "eosnameswap3","Test"]' -p eosnameswap1@owner


cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80 push action eosio updateauth '{"account":"nameswapsln2","permission":"owner","parent":"","auth":{"threshold": 1,"keys": [{"key":"EOS8g6Eb7nixavjSaKh3PSseDu5Az47Xhy2QN52h42KzNmH8FqR2M","weight":1}],"waits":[],"accounts": [{"permission":{"actor":"nameswapsln1","permission":"eosio.code"},"weight":1}]}}' -p nameswapsln2@owner
cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80 push action eosio updateauth '{"account":"nameswapsln3","permission":"owner","parent":"","auth":{"threshold": 1,"keys": [{"key":"EOS8g6Eb7nixavjSaKh3PSseDu5Az47Xhy2QN52h42KzNmH8FqR2M","weight":1}],"waits":[],"accounts": [{"permission":{"actor":"nameswapsln1","permission":"eosio.code"},"weight":1}]}}' -p nameswapsln3@owner


cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80 push action nameswapsln1 sell '[ "nameswapsln2", "1.0000 EOS", "gettingmoney","Test"]' -p nameswapsln2@owner
cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80 push action nameswapsln1 proposebid '[ "nameswapsln2", "1.5000 EOS", "sellswapsln1"]' -p sellswapsln1@owner

# Give contract permission to send actions
cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80 push action eosio updateauth '{"account":"nameswapsln1","permission":"active","parent":"owner","auth":{"threshold": 1,"keys": [{"key":"EOS8g6Eb7nixavjSaKh3PSseDu5Az47Xhy2QN52h42KzNmH8FqR2M","weight":1}],"waits":[],"accounts": [{"permission":{"actor":"nameswapsln1","permission":"eosio.code"},"weight":1}]}}' -p nameswapsln1@owner


cleos --wallet-url http://0.0.0.0:9876 --url http://jungle2.cryptolions.io:80 get table nameswapsln1 eosnameswaps accounts