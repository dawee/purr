const engine = require('engine');

exports.draw = () => engine.emit('draw');
exports.update = dt => engine.emit('update', {dt});
exports.dispatch = ({type, key, ...data}) => engine.emit(type, {
  key: key && key.toLowerCase(),
  ...data
});
