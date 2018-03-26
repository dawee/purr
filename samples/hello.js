const mallow = purr.loadTexture(`${__dirname__}/asset/mallow.bmp`);

let x = 0;

const update = () => {
  x += 0.05;
}

const draw = () => purr.drawTexture(mallow, x, 0);

module.exports = {draw, update};
