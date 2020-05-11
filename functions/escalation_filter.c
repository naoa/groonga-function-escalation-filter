/*
  Copyright(C) 2016 Naoya Murakami <naoya@createfield.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License version 2.1 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <string.h>
#include <stdlib.h>
#include <groonga/plugin.h>

#ifdef __GNUC__
# define GNUC_UNUSED __attribute__((__unused__))
#else
# define GNUC_UNUSED
#endif

static grn_rc
selector_escalation_filter(grn_ctx *ctx, GNUC_UNUSED grn_obj *table, GNUC_UNUSED grn_obj *index,
                           GNUC_UNUSED int nargs, grn_obj **args,
                           grn_obj *res, GNUC_UNUSED grn_operator op)
{
  grn_obj *filter;
  uint32_t threshold = 0;
  unsigned int n_hits;

  if (nargs < 2) {
    GRN_PLUGIN_ERROR(ctx, GRN_INVALID_ARGUMENT,
                     "escalation_filter(): wrong number of arguments (%d for 1..)",
                     nargs - 1);
    return GRN_INVALID_ARGUMENT;
  }
  filter = args[1];

  if (nargs > 2) {
    if (!(args[2]->header.type == GRN_BULK &&
          ((args[2]->header.domain == GRN_DB_INT32)))) {
      GRN_PLUGIN_ERROR(ctx, GRN_INVALID_ARGUMENT,
                       "escalation_filter(): 1st argument must be UINT");
      return GRN_INVALID_ARGUMENT;
    }
    threshold = GRN_UINT32_VALUE(args[2]);
  }

  n_hits = grn_table_size(ctx, res);
  if (n_hits <= threshold && GRN_TEXT_LEN(filter) > 0) {
    grn_obj *v, *cond = NULL;

    GRN_EXPR_CREATE_FOR_QUERY(ctx, table, cond, v);
    grn_expr_parse(ctx, cond,
                   GRN_TEXT_VALUE(filter),
                   GRN_TEXT_LEN(filter),
                   NULL,
                   GRN_OP_MATCH,
                   GRN_OP_AND,
                   GRN_EXPR_SYNTAX_SCRIPT);
    if (ctx->rc != GRN_SUCCESS) {
      goto exit;
    }
    grn_table_select(ctx, table, cond, res, op);
    if (ctx->rc != GRN_SUCCESS) {
      goto exit;
    }
    if (cond) {
      grn_obj_unlink(ctx, cond);
    }
  }

exit :
  
  return GRN_SUCCESS;
}

grn_rc
GRN_PLUGIN_INIT(GNUC_UNUSED grn_ctx *ctx)
{
  return GRN_SUCCESS;
}

grn_rc
GRN_PLUGIN_REGISTER(grn_ctx *ctx)
{
  {
    grn_obj *selector_proc;

    selector_proc = grn_proc_create(ctx, "escalation_filter", -1, GRN_PROC_FUNCTION,
                                    NULL, NULL, NULL, 0, NULL);
    grn_proc_set_selector(ctx, selector_proc, selector_escalation_filter);
    grn_proc_set_selector_operator(ctx, selector_proc, GRN_OP_NOP);
  }


  return ctx->rc;
}

grn_rc
GRN_PLUGIN_FIN(GNUC_UNUSED grn_ctx *ctx)
{
  return GRN_SUCCESS;
}
