#ifndef HTML_H
#define HTML_H

#define base_start "<html><body><head>\
    <meta charset='UTF-8'>\
    <title>This is cweb.</title>\
    </head>"

#define base_end "</body></html>"

#define not_found\
    base_start \
    "<h1>404 Page Not Found</h1>\
    <p>The requested resource was not found on cweb.</p>"\
    base_end

#endif
