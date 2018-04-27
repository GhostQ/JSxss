var webPage = require('webpage');
var page = webPage.create();

page.viewportSize = { width: 1920, height:1080 };
page.open("https://www.zhihu.com/", function start(status) {
  page.render('google.png');
  console.log("hello");
  phantom.exit();
});
//page.open("https://mqshen.gitbooks.io/prml/Chapter8/inference/sum_product_algorithm.html", function start(status) {
//  page.render('sum_product_algorithm.jpg', {format: 'jpeg', quality: '100'});
//  phantom.exit();
//});