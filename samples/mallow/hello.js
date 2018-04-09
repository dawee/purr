const engine = require('engine');
const {updateX} = require('./update');

const mallow = graphics.loadTexture(`${__dirname__}/asset/mallow.png`);

let x = 0;
let moving = false;

engine.on('keydown', ({key}) => {
  if (key === 'Right') {
    moving = true;
  }
});

engine.on('keyup', ({key}) => {
  if (key === 'Right') {
    moving = false;
  }
});

engine.on('update', ({dt}) => {
  if (moving) {
    x = updateX(dt, x);
  }
});

engine.on('draw', () => graphics.drawTexture(mallow, x, 100, 100, 100, 110, 100));
