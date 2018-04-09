const engine = require('engine');

exports.loadTexture = texturePath => new Promise(resolve => {
  const texture = _graphics.loadTexture(texturePath);

  const checkLoaded = () => {
    if (texture.loaded) {
      engine.removeListener('update', checkLoaded);
      resolve(texture);
    }
  };

  engine.on('update', checkLoaded);
});

exports.drawTexture = (texture, x = 0, y = 0, opts = {}) => {
  if (!texture.loaded) {
    return;
  }

  const {
    width = texture.width,
    height = texture.height,
    ox = 0,
    oy = 0
  } = opts;

  _graphics.drawTexture(texture, x, y, width, height, ox, oy);
}
