#!/bin/sh



# /bin/sh -c   "nodeos -p eosio \
#     -d /mnt/dev/data \
#     --config-dir /mnt/dev/config"


# ls -l

echo "hello simpleprogrammer"

# rm -rf data/*
# nodeos -p eosio -d /mnt/dev/data --config-dir /mnt/dev/config


# docker-compose exec nodeos bash
alias cleos='docker-compose exec keosd /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://0.0.0.0:9876'


cleos wallet create --to-console
# master password: PW5J6DKtpNDSjwDC3nuoNfGgGULxfPcNBKNJu47Lu57ZjABWwRDNb

cleos create key  --to-console
# Private key: 5JV6wdAxByYn8eCyi2YZYAnBCFfJgDwLhbxVy1ktMBMBGVkb4pE
# Public key: EOS52Jd9aMVEUgYHafjdN5EJd8N1LLMVf7CLgoFQ64AQ31jEbX4Mf

cleos wallet import [private key]

# private-key for eosio
cleos wallet import --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3
cleos wallet keys 

cleos get accounts 

# then for contract compiling - kind of like alias
# eosiocpp() { d=/; docker exec smart_nodeosd opt/eosio/bin/eosiocpp $1 $d/$2 $d/$3; }
# eosbuild() { d=/; eosiocpp -o $1.wast $1.cpp; eosiocpp -g $1.abi $1.cpp; cleos set contract ekkis $d/${1%/*} $d/$1.wast $d/$1.abi; }


cleos set contract [eosio or $USERNAME] work/contracts/eosio.bios -p eosio

cleos push action nikki hi '["randd"]' -p eoss



