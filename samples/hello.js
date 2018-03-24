const update = dt => {
  console.log('update');
};

const draw = () => {
  console.log('draw');
};

module.exports = {draw, update};

console.log('hello.js loaded', exports.draw, module.exports.update);
