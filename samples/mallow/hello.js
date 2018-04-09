const engine = require('engine');
const graphics = require('graphics');

const {updateX} = require('./update');

const mallow = graphics.loadTexture(`${__dirname__}/asset/mallow.png`);

let x = 0;
let moving = false;

engine.on('keydown', ({key}) => {
  if (key === 'right') {
    moving = true;
  }
});

engine.on('keyup', ({key}) => {
  if (key === 'right') {
    moving = false;
  }
});

engine.on('update', ({dt}) => {
  if (moving) {
    x = updateX(dt, x);
  }
});

engine.on('draw', () => graphics.drawTexture(mallow, x, 100, {
  width: 100,
  height: 100,
  ox: 100,
  oy: 110,
}));
