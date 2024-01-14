#ifndef HTML_H
#define HTML_H

#define base_start "<html><body>\
    <meta charset='UTF-8'>\
    <title>This is cweb.</title>"

#define head "<div style='background-color:Orange;'>\
This is simple text that lives on top\
</div>"

#define style "\
<style>\
    h1 {color: blue;} p {color: red;}\
    body { margin: 0; }\
    header { height: 8em; background-color: red; }\
</style>\
"

#define table "<table>\
  <tr>\
    <td>Cell 1</td>\
    <td>Cell 2</td>\
    <td>Cell 3</td>\
  </tr>\
  <tr>\
    <td>Cell 4</td>\
    <td>Cell 5</td>\
    <td>Cell 6</td>\
  </tr>\
</table>\
"

#define base_end "</body></html>"

#endif
