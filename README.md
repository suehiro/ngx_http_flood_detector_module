#ngx_http_flood_detector_module#



##Compile##
Stub Status module is required.

`./configure --with-http_stub_status_module --add-module=path/to/ngx_http_flood_detector_module`


##Example##

    location / {
        ...
        flood_threshold 100;
        if ($flood_detected) {
            return 503;
        }
        ...
    }


##Directives##

###flood_threshold###

    Syntax:   flood_threshold threshold;
    Default:  flood_threshold 0;
    Context:  location

The value "0" means that flood detection is disabled.

##Variables##

###flood_detected###
This variable indicates that the number of active connections exceeds flood_threshold or not.

    Value   Meaning
    -----   ------------------------------
      1     exceeds flood_threshold
      0     doesn't exceed flood_threshold

if flood_threshold directive is 0, flood_detectd is 0.
