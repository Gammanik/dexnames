const assert = require('chai').assert;
const expect = require('chai').expect;
// const should = require('chai').should;

createTestCases = function(eos) {

// array store all test cases


  const contractAccount = 'nameswapsln1'
  const contractName = 'eosnameswaps'

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

  add_test('is able to sell (get table rows)', (done) => {

    const accounts = eos.api.getTableRows({
      json: true,
      scope: contractAccount,
      code: contractAccount,
      table: 'stats',
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
    testcases.forEach( (testcase) => {
        it(testcase.desc, testcase.func);
    });
  });
}



// _buyAccount = eos.api.transaction({
//       actions: [
//         eos.makeAction(contract, 'create', {  issuer: 'contract1111',
//             maximum_supply: '2000000.0000 BTC'} ),
//         // eos.makeAction('contract1111', 'issue', {   to: 'contract1111',
//         //     quantity: '500000.0000 BTC',
//         //     memo: 'Wow! I am rich' } )
//       ]
//     },
//     { expireInSeconds: 60, broadcast: true, sign: true },
//     (err, result) => {
//       expect(err).to.not.exist;
//
//       eos.api.getCurrencyBalance('contract1111', 'contract1111', 'BTC')
//         .then( (asset) => {
//           expect(asset).to.be.an('array');
//           assert.equal(JSON.stringify(asset), JSON.stringify([ '500000.0000 BTC' ]));
//           done();
//         });
//     });

module.exports.createTestCases = createTestCases;
module.exports.runTestCases = runTestCases;