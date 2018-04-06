const {updateX} = require('./update');

require('engine');

const mallow = graphics.loadTexture(`${__dirname__}/asset/mallow.png`);

let x = 0;
let moving = false;

const dispatch = event => {
  const {type, key} = event;

  console.log(JSON.stringify(event));

  if (type === 'keydown' && key === 'Right') {
    moving = true;
  }

  if (type === 'keyup' && key === 'Right') {
    moving = false;
  }
};

const update = dt => {
  if (moving) {
    x = updateX(dt, x);
  }
};

const draw = () => graphics.drawTexture(mallow, x, 100, 100, 100, 110, 100);

module.exports = {dispatch, draw, update};
