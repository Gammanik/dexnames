cleos push action newaccount '{'creator':'collectmoney', 'name':'sex', 'owner':'collectmoney', }'


cleos --url http://jungle2.cryptolions.io:80 system newaccount --stake-net "0.2000 EOS" --stake-cpu "1.1000 EOS" --buy-ram-kbytes 10 collectmoney sex EOS53Ad9acDA2mQZXDGmpJxB5MBopDoLY99qS4RoNs8D2SBXosoJx
cleos --url http://jungle2.cryptolions.io:80 push action eosio updateauth '{"account":"sex","permission":"active","parent":"owner","auth":{"threshold": 1,"keys": [{"key":"EOS8g6Eb7nixavjSaKh3PSseDu5Az47Xhy2QN52h42KzNmH8FqR2M","weight":1}],"waits":[],"accounts": [{"permission":{"actor":"premiumtest1","permission":"eosio.code"},"weight":1}]}}' -p sex
cleos --url http://jungle2.cryptolions.io:80 set account permission premiumtest1 active --add-code


cleos --url http://jungle2.cryptolions.io:80 push action eosio updateauth '{"account":"premiumtest1","permission":"owner","parent":"","auth":{"threshold": 1,"keys": [{"key":"EOS53Ad9acDA2mQZXDGmpJxB5MBopDoLY99qS4RoNs8D2SBXosoJx","weight":1}],"waits":[],"accounts": [{"permission":{"actor":"premiumtest1","permission":"eosio.code"},"weight":1}]}}' -p premiumtest1

cleos --url http://jungle2.cryptolions.io:80 set contract premiumtest1  ./premium
eosio-abigen premium.cpp --contract=premium --output=premium.abi



cleos --url http://jungle2.cryptolions.io:80 push action premiumtest1 regname '{"newname":"12345", "ownerkey": "EOS53Ad9acDA2mQZXDGmpJxB5MBopDoLY99qS4RoNs8D2SBXosoJx", "activekey": "EOS53Ad9acDA2mQZXDGmpJxB5MBopDoLY99qS4RoNs8D2SBXosoJx"}' -p premiumtest1

cleos --url http://jungle2.cryptolions.io:80 push action eosio.token transfer '{"from":"collectmoney", "to":"premiumtest1", "quantity":"2.0000 EOS", "memo":"ask:namesw12.sex"}' -p collectmoney


cleos --url http://jungle.eosbcn.com:8080 push action premiumtest1 askprice '{"account":"collectmoney", "nametobuy": "pornpnr1.sex"}' -p collectmoney
cleos --url http://jungle.eosbcn.com:8080 push action premiumtest1 approveask '{"id":"5", "price":"1.0001 EOS", "admin": "premiumtest1"}' -p premiumtest1



 cleos --url http://jungle2.cryptolions.io:80 push action premiumtest1 init '{}' -p premiumtest1



 steps::::

// ask for a name
 cleos --url http://jungle2.cryptolions.io:80 push action eosio.token transfer '{"from":"collectmoney", "to":"premiumtest1", "quantity":"0.2000 EOS", "memo":"ask:namesw22.sex"}' -p collectmoney

// admin respond
 cleos --url http://jungle.eosbcn.com:8080 push action premiumtest1 approveask '{"id":"17", "price":"5.0000 EOS", "admin": "premiumtest1"}' -p premiumtest1
  cleos --url http://jungle.eosbcn.com:8080 push action premiumtest1 declineask '{"id":"16", "admin": "premiumtest1"}' -p premiumtest1

// user decides
  cleos --url http://jungle2.cryptolions.io:80 push action eosio.token transfer '{"from":"collectmoney", "to":"premiumtest1", "quantity":"3.8000 EOS", "memo":"buy:15,EOS53Ad9acDA2mQZXDGmpJxB5MBopDoLY99qS4RoNs8D2SBXosoJx,EOS53Ad9acDA2mQZXDGmpJxB5MBopDoLY99qS4RoNs8D2SBXosoJx"}' -p collectmoney
  cleos --url http://jungle.eosbcn.com:8080 push action premiumtest1 declinebuy '{"id":"17"}' -p premiumtest1






