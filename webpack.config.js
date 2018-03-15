const ExtractTextPlugin = require('extract-text-webpack-plugin')

module.exports = {
  entry: ['./lib/js/src/index.js', './src/style.css'],
  output: {
    path: __dirname + '/public',
    filename: 'bundle.js',
    library: 'starter',
  },
  module: {
    rules: [
      {
        test: /\.css$/,
        use: ExtractTextPlugin.extract({
          use: [
            { loader: 'css-loader', options: { importLoaders: 1 } },
            'postcss-loader'
          ]
        })
      }
    ]
  },
  plugins: [
    new ExtractTextPlugin('style.css')
  ]
};

