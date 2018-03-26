const mallow = purr.loadTexture(`${__dirname__}/asset/mallow.bmp`);

const draw = () => purr.drawTexture(mallow);

module.exports = {draw};
