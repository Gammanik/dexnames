const assert = require('chai').assert;
const expect = require('chai').expect;
// const should = require('chai').should;




// const { Api, JsonRpc } = require('eosjs');
// const JsSignatureProvider = require('eosjs/dist/eosjs-jssig');  // development only
// const fetch = require('node-fetch');                            // node only; not needed in browsers
// const { TextDecoder, TextEncoder } = require('text-encoding');  // node, IE11 and IE Edge Browsers
//
// const privateKeys = ["5KUaRLic6GNDrcmKCLx9dPS3QnAtsP2XaZUqJznQzZJZHQi7AZ4"];
//
// const signatureProvider = new JsSignatureProvider(privateKeys);
// const rpc = new JsonRpc('http://127.0.0.1:8888', { fetch });
// const api = new Api({ rpc, signatureProvider, textDecoder: new TextDecoder(), textEncoder: new TextEncoder() });





const contractAccount = 'nameswapsln1'
const contractName = 'eosnameswaps'

createTestCases = function(eos) {

  let testcases = [];
  const add_test = function(desc, func) {
      testcases.push( { desc: desc, func: func } );
  }

  // add_test('is able to sell (get table rows)', (done) => {
  //   eos.api.transaction({
  //     actions: [
  //       eos.makeAction(contract, 'create', {  issuer: 'contract1111',
  //           maximum_supply: '2000000.0000 BTC'} ),
  //       // eos.makeAction('contract1111', 'issue', {   to: 'contract1111',
  //       //     quantity: '500000.0000 BTC',
  //       //     memo: 'Wow! I am rich' } )
  //     ]
  //   },
  //   { expireInSeconds: 60, broadcast: true, sign: true },
  //   (err, result) => {
  //     expect(err).to.not.exist;
  //
  //     eos.api.getCurrencyBalance('contract1111', 'contract1111', 'BTC')
  //       .then( (asset) => {
  //         expect(asset).to.be.an('array');
  //         assert.equal(JSON.stringify(asset), JSON.stringify([ '500000.0000 BTC' ]));
  //         done();
  //       });
  //   });
  // });
  //
  //

  // add_test('buy ram', async (done) => {
  //   const result = await api.transact({
  //     actions: [{
  //       account: 'eosio',
  //       name: 'buyrambytes',
  //       authorization: [{
  //         actor: 'useraaaaaaaa',
  //         permission: 'active',
  //       }],
  //       data: {
  //         payer: 'useraaaaaaaa',
  //         receiver: 'useraaaaaaaa',
  //         bytes: 8192,
  //       },
  //     }]
  //   }, {
  //     blocksBehind: 3,
  //     expireSeconds: 30,
  //   });
  // })


  // add_test('sell account', (done) => {
  //   eos.api.transaction({
  //     actions: [
  //       eos.makeAction(contractAccount, 'sell',
  //         {
  //           account4sale: "nameswapsln3",
  //           saleprice: "2.0000 EOS",
  //           paymentaccnt: "gettingmoney",
  //           message: "Test"
  //         },
  //         {authorization: 'nameswapsln3@owner'}
  //       )
  //     ]
  //   },
  //   {expireInSeconds: 60, broadcast: true, sign: true},
  //   (err, result) => {
  //
  //     console.log("log: ~~~~~~~~~~~~~~", result, err)
  //     expect(err).to.not.exist
  //     done()
  // }).then((res, err) => {
  //     console.log("~~~~~~~~~~~~~~", res, err)
  //
  //   });
  // })

  add_test('get table rows', (done) => {

    const accounts = eos.api.getTableRows({
      json: true,
      scope: contractAccount,
      code: contractAccount,
      table: 'accounts',
      limit: 500
    }).then((data) => {
      console.log("logg data: ", data)

      assert.equal(2, 2)
      done()
    }).catch( (e) => {
      console.log('error:', e)
      assert.equal(2, 3)
      done()
    })

  });

  add_test("selling name", async (done) => {


    const sellResult =  eos.api.transaction({
        actions: [
          eos.makeAction(contractAccount, 'sell',
            {
              account4sale: "nameswapsln2",
              saleprice: "1.0000 EOS",
              paymentaccnt: "gettingmoney",
              message: "Test"
            }),
          // eos.makeAction('contract1111', 'issue', {   to: 'contract1111',
          //     quantity: '500000.0000 BTC',
          //     memo: 'Wow! I am rich' } )
        ]
      },

      {expireInSeconds: 60, broadcast: true, sign: true, authorization: `nameswapsln2@active`},
      (err, result) => {

        expect(err).to.not.exist

        console.log('result: ', result)
        done()
      });
  })

  return testcases;
}



runTestCases = function(testcases, flags) {

  describe('nameswaps contract', function() {
    before(function() {
        console.log('----------------------------------------------------');
        console.log('---', 'total num of cases: ' + testcases.length);
        console.log('\n');
        console.log('----------------- running cases --------------------');
    });
    after(function() {
        flags.finished = true;
        console.log('\n');
        console.log('-------------------- report ------------------------');
    })

    // test cases
    // testcases.forEach( (testcase) => {
    //     it(testcase.desc, testcase.func);
    // });

    it(testcases[0].desc, testcases[0].func)
  });
}



_sellAccount = function(eos) {

}

module.exports.createTestCases = createTestCases;
module.exports.runTestCases = runTestCases;