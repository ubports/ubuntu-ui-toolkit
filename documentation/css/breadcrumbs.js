$(document).ready(function(){
    var ul = $("ul.breadcrumb");
    var li = ul.children("li:has(a)").remove();
    ul.prepend(li);
});
