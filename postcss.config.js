var tailwindcss = require('tailwindcss');
module.exports = {
  plugins: [
    tailwindcss('./src/tailwind.js.cfg'),
    require('autoprefixer'),
  ]
}
