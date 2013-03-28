/*
 * Copyright (c) Masatoshi Suehiro <suehiro.masatoshi@synergy101.jp>
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_uint_t  threshold;
} ngx_http_flood_detector_loc_conf_t;

typedef struct {
   int detected;
} ngx_http_flood_detector_ctx_t;

static ngx_int_t ngx_http_flood_detector_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_flood_detector_variable(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);

static ngx_int_t ngx_http_flood_detector_add_variables(ngx_conf_t *cf);
static ngx_int_t ngx_http_flood_detector_init(ngx_conf_t *cf);
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
    ngx_http_flood_detector_init,          /* postconfiguration */

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


static ngx_int_t
ngx_http_flood_detector_handler(ngx_http_request_t *r)
{
    ngx_http_flood_detector_ctx_t       *ctx;
    ngx_http_flood_detector_loc_conf_t  *fdlcf;

    ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_flood_detector_ctx_t));
    if (ctx == NULL) {
        return NGX_ERROR;
    }

    ngx_http_set_ctx(r, ctx, ngx_http_flood_detector_module);

    fdlcf = ngx_http_get_module_loc_conf(r, ngx_http_flood_detector_module);

    if (fdlcf != NULL && (fdlcf->threshold > 0) && (*ngx_stat_active > fdlcf->threshold))
    {
        ctx->detected = 1;
    } else {
        ctx->detected = 0;
    }
    ngx_log_debug3(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "http flood detector: ngx_stat_active = %ui, threshold = %ui, detected = %ui", *ngx_stat_active, fdlcf->threshold, ctx->detected);

    return NGX_DECLINED;
}

static ngx_int_t
ngx_http_flood_detector_variable(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_flood_detector_ctx_t  *ctx;

    ctx = ngx_http_get_module_ctx(r, ngx_http_flood_detector_module);

    if (ctx == NULL) {
        v->not_found = 1;
        return NGX_OK;
    }

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;
    v->len = ngx_sprintf(v->data, "%ui", ctx->detected) - v->data;

    return NGX_OK;
}

static ngx_int_t
ngx_http_flood_detector_add_variables(ngx_conf_t *cf)
{
    ngx_http_variable_t  *var, *v;

    for (v = ngx_http_flood_detector_vars; v->name.len; v++) {
        var = ngx_http_add_variable(cf, &v->name, v->flags);
        if (var == NULL) {
            return NGX_ERROR;
        }

        var->get_handler = v->get_handler;
        var->data = v->data;
    }

    return NGX_OK;
}

static ngx_int_t
ngx_http_flood_detector_init(ngx_conf_t *cf)
{
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_POST_READ_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_flood_detector_handler;

    return NGX_OK;
}

static void *
ngx_http_flood_detector_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_flood_detector_loc_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_flood_detector_loc_conf_t));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }
    conf->threshold = NGX_CONF_UNSET_UINT;
    return conf;
}

static char *
ngx_http_flood_detector_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_flood_detector_loc_conf_t  *prev = parent;
    ngx_http_flood_detector_loc_conf_t  *conf = child;

    ngx_conf_merge_uint_value(conf->threshold, prev->threshold, 0);

    if (conf->threshold < 0) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "flood_threshold must be equal or more than 0");
        return NGX_CONF_ERROR;
    }

    return NGX_CONF_OK;
}

