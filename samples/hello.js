const {updateX} = require('./update');

const mallow = purr.loadTexture(`${__dirname__}/asset/mallow.png`);

let x = 0;

const update = dt => {
  x = updateX(dt, x);
};

const draw = () => purr.drawTexture(mallow, x, 0);

module.exports = {draw, update};
