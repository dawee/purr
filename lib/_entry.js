const engine = require('engine');

exports.draw = () => engine.emit('draw');
exports.update = dt => engine.emit('update', {dt});
