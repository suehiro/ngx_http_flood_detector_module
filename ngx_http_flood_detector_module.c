/*
 * Copyright (c) Masatoshi Suehiro <suehiro.masatoshi@synergy101.jp>
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_int_t    threshold;
} ngx_http_flood_detector_loc_conf_t;


static ngx_int_t ngx_http_flood_detector_variable(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);

static ngx_int_t ngx_http_flood_detector_add_variables(ngx_conf_t *cf);
static void *ngx_http_flood_detector_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_flood_detector_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);


static ngx_command_t ngx_http_flood_detector_commands[] = {
    {  ngx_string("flood_threshold"),
       NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
       ngx_conf_set_num_slot,
       NGX_HTTP_LOC_CONF_OFFSET,
       offsetof(ngx_http_flood_detector_loc_conf_t, threshold),
       NULL },

    ngx_null_command
};

static ngx_http_module_t ngx_http_flood_detector_module_ctx = {
    ngx_http_flood_detector_add_variables, /* preconfiguration */
    NULL,                                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    ngx_http_flood_detector_create_loc_conf, /* create location configration */
    ngx_http_flood_detector_merge_loc_conf   /* merge location configration */
};

ngx_module_t ngx_http_flood_detector_module = {
    NGX_MODULE_V1,
    &ngx_http_flood_detector_module_ctx,   /* module context */
    ngx_http_flood_detector_commands,      /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_http_variable_t ngx_http_flood_detector_vars[] = {
    { ngx_string("flood_detected"), NULL, ngx_http_flood_detector_variable, 0,
      NGX_HTTP_VAR_CHANGEABLE|NGX_HTTP_VAR_NOCACHEABLE|NGX_HTTP_VAR_NOHASH, 0 },

    { ngx_null_string, NULL, NULL, 0, 0, 0 }
};


static ngx_int_t ngx_http_flood_detector_variable(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data)
{
};

static ngx_int_t ngx_http_flood_detector_add_variables(ngx_conf_t *cf)
{
};

static void *ngx_http_flood_detector_create_loc_conf(ngx_conf_t *cf)
{
};

static char *ngx_http_flood_detector_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
};

