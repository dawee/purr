const engine = require('engine');
const graphics = require('graphics');

const hat = require('hat');

console.log(hat());

const {updateX} = require('./update/update-x');

const runSample = async () => {
  let x = 0;
  let movingRight = false;

  const mallow = await graphics.loadTexture(`${__dirname__}/asset/mallow.png`);

  engine.on('keydown', ({key}) => {
    movingRight = (key === 'right') || movingRight;
  });

  engine.on('keyup', ({key}) => {
    movingRight = !(key === 'right') && movingRight;
  });

  engine.on('update', ({dt}) => {
    x = movingRight ? updateX(dt, x) : x;
  });

  engine.on('draw', () => graphics.drawTexture(mallow, x, 100));
};

runSample();
