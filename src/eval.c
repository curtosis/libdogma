/* libdogma
 * Copyright (C) 2012 Romain "Artefact2" Dalmaso <artefact2@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include "eval.h"
#include "tables.h"
#include "attribute.h"

int dogma_eval_expression(dogma_context_t* ctx,
                          dogma_env_t* self,
                          dogma_env_t* other,
                          expressionid_t id,
                          dogma_expctx_t* result) {
	const dogma_expression_t* exp;
	dogma_expctx_t resarg1, resarg2;

	result->type = DOGMA_CTXTYPE_UNDEFINED;
	assert(dogma_get_expression(id, &exp) == DOGMA_OK);

	switch(exp->operand) {

		/* Language constructs */

	case DOGMA_COMBINE:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		break;

	case DOGMA_ADD:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_FLOAT || resarg1.type == DOGMA_CTXTYPE_INT);
		assert(resarg2.type == DOGMA_CTXTYPE_FLOAT || resarg2.type == DOGMA_CTXTYPE_INT);
		result->type = DOGMA_CTXTYPE_FLOAT;
		if(resarg1.type == DOGMA_CTXTYPE_FLOAT) {
			if(resarg2.type == DOGMA_CTXTYPE_INT) {
				result->float_value = resarg1.float_value + resarg2.float_value;
			} else {
				result->float_value = resarg1.float_value + (double)resarg2.int_value;
			}
		} else {
			if(resarg2.type == DOGMA_CTXTYPE_INT) {
				result->float_value = (double)resarg1.int_value + resarg2.float_value;
			} else {
				result->float_value = (double)resarg1.int_value + (double)resarg2.int_value;
			}
		}
		break;

	case DOGMA_SUB:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_FLOAT || resarg1.type == DOGMA_CTXTYPE_INT);
		assert(resarg2.type == DOGMA_CTXTYPE_FLOAT || resarg2.type == DOGMA_CTXTYPE_INT);
		result->type = DOGMA_CTXTYPE_FLOAT;
		if(resarg1.type == DOGMA_CTXTYPE_FLOAT) {
			if(resarg2.type == DOGMA_CTXTYPE_INT) {
				result->float_value = resarg1.float_value - resarg2.float_value;
			} else {
				result->float_value = resarg1.float_value - (double)resarg2.int_value;
			}
		} else {
			if(resarg2.type == DOGMA_CTXTYPE_INT) {
				result->float_value = (double)resarg1.int_value - resarg2.float_value;
			} else {
				result->float_value = (double)resarg1.int_value - (double)resarg2.int_value;
			}
		}
		break;

	case DOGMA_MUL:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_FLOAT || resarg1.type == DOGMA_CTXTYPE_INT);
		assert(resarg2.type == DOGMA_CTXTYPE_FLOAT || resarg2.type == DOGMA_CTXTYPE_INT);
		result->type = DOGMA_CTXTYPE_FLOAT;
		if(resarg1.type == DOGMA_CTXTYPE_FLOAT) {
			if(resarg2.type == DOGMA_CTXTYPE_INT) {
				result->float_value = resarg1.float_value * resarg2.float_value;
			} else {
				result->float_value = resarg1.float_value * (double)resarg2.int_value;
			}
		} else {
			if(resarg2.type == DOGMA_CTXTYPE_INT) {
				result->float_value = (double)resarg1.int_value * resarg2.float_value;
			} else {
				result->float_value = (double)resarg1.int_value * (double)resarg2.int_value;
			}
		}
		break;

	case DOGMA_IF:
		assert(exp->arg1 != 0);
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		assert(resarg1.type == DOGMA_CTXTYPE_BOOL);
		if(resarg1.bool_value) {
			dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		}
		break;

	case DOGMA_AND:
		assert(exp->arg1 != 0);
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		assert(resarg1.type == DOGMA_CTXTYPE_BOOL);
		if(!resarg1.bool_value) {
			/* FIXME: assuming AND is lazy */
			result->type = DOGMA_CTXTYPE_BOOL;
			result->bool_value = false;
		} else {
			assert(exp->arg2 != 0);
			dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
			assert(resarg2.type == DOGMA_CTXTYPE_BOOL);
			result->type = DOGMA_CTXTYPE_BOOL;
			result->bool_value = resarg2.bool_value;
		}
		break;

	case DOGMA_OR:
		assert(exp->arg1 != 0);
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		assert(resarg1.type == DOGMA_CTXTYPE_BOOL);
		if(resarg1.bool_value) {
			/* FIXME: assuming OR is lazy */
			result->type = DOGMA_CTXTYPE_BOOL;
			result->bool_value = true;
		} else {
			assert(exp->arg2 != 0);
			dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
			assert(resarg2.type == DOGMA_CTXTYPE_BOOL);
			result->type = DOGMA_CTXTYPE_BOOL;
			result->bool_value = resarg2.bool_value;
		}
		break;

	case DOGMA_EQ:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_FLOAT || resarg1.type == DOGMA_CTXTYPE_INT);
		assert(resarg2.type == DOGMA_CTXTYPE_FLOAT || resarg2.type == DOGMA_CTXTYPE_INT);
		result->type = DOGMA_CTXTYPE_BOOL;
		if(resarg1.type == DOGMA_CTXTYPE_FLOAT) {
			if(resarg2.type == DOGMA_CTXTYPE_INT) {
				result->bool_value = resarg1.float_value == resarg2.float_value;
			} else {
				result->bool_value = resarg1.float_value == (double)resarg2.int_value;
			}
		} else {
			if(resarg2.type == DOGMA_CTXTYPE_INT) {
				result->bool_value = (double)resarg1.int_value == resarg2.float_value;
			} else {
				result->bool_value = resarg1.int_value == resarg2.int_value;
			}
		}
		break;

	case DOGMA_GT:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_FLOAT || resarg1.type == DOGMA_CTXTYPE_INT);
		assert(resarg2.type == DOGMA_CTXTYPE_FLOAT || resarg2.type == DOGMA_CTXTYPE_INT);
		result->type = DOGMA_CTXTYPE_BOOL;
		if(resarg1.type == DOGMA_CTXTYPE_FLOAT) {
			if(resarg2.type == DOGMA_CTXTYPE_INT) {
				result->bool_value = resarg1.float_value > resarg2.float_value;
			} else {
				result->bool_value = resarg1.float_value > (double)resarg2.int_value;
			}
		} else {
			if(resarg2.type == DOGMA_CTXTYPE_INT) {
				result->bool_value = (double)resarg1.int_value > resarg2.float_value;
			} else {
				result->bool_value = resarg1.int_value > resarg2.int_value;
			}
		}
		break;

	case DOGMA_GTE:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_FLOAT || resarg1.type == DOGMA_CTXTYPE_INT);
		assert(resarg2.type == DOGMA_CTXTYPE_FLOAT || resarg2.type == DOGMA_CTXTYPE_INT);
		result->type = DOGMA_CTXTYPE_BOOL;
		if(resarg1.type == DOGMA_CTXTYPE_FLOAT) {
			if(resarg2.type == DOGMA_CTXTYPE_INT) {
				result->bool_value = resarg1.float_value >= resarg2.float_value;
			} else {
				result->bool_value = resarg1.float_value >= (double)resarg2.int_value;
			}
		} else {
			if(resarg2.type == DOGMA_CTXTYPE_INT) {
				result->bool_value = (double)resarg1.int_value >= resarg2.float_value;
			} else {
				result->bool_value = resarg1.int_value >= resarg2.int_value;
			}
		}
		break;

	case DOGMA_RS:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_ENV);
		assert(resarg2.type == DOGMA_CTXTYPE_TYPEID);
		result->type = DOGMA_CTXTYPE_BOOL;
		assert(dogma_env_requires_skill(resarg1.env_value,
		                                resarg2.typeid_value,
		                                &(result->bool_value)) == DOGMA_OK);
		break;

		/* Attribute manipulation */

	case DOGMA_INC:
		assert(false);
		break;

	case DOGMA_DEC:
		assert(false);
		break;

	case DOGMA_INCN:
		assert(false);
		break;

	case DOGMA_DECN:
		assert(false);
		break;

	case DOGMA_GET:
		assert(false);
		break;

	case DOGMA_SET:
		assert(false);
		break;

		/* Filters */

	case DOGMA_ATT:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_ENV || resarg1.type == DOGMA_CTXTYPE_MODIFIER);
		assert(resarg2.type == DOGMA_CTXTYPE_ATTRIBUTEID);
		if(resarg1.type == DOGMA_CTXTYPE_ENV) {
			result->type = DOGMA_CTXTYPE_MODIFIER;
			result->modifier_value.filter.type = DOGMA_FILTERTYPE_PASS;
			result->modifier_value.targetenv = resarg1.env_value;
		} else {
			*result = resarg1;
		}
		result->modifier_value.targetattribute = resarg2.attributeid_value;
		break;

	case DOGMA_LG:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_ENV);
		assert(resarg2.type == DOGMA_CTXTYPE_GROUPID);
		result->type = DOGMA_CTXTYPE_MODIFIER;
		result->modifier_value.filter.type = DOGMA_FILTERTYPE_GROUP;
		result->modifier_value.filter.groupid = resarg2.groupid_value;
		result->modifier_value.targetenv = resarg1.env_value;
		break;

	case DOGMA_LS:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_ENV);
		assert(resarg2.type == DOGMA_CTXTYPE_TYPEID);
		result->type = DOGMA_CTXTYPE_MODIFIER;
		result->modifier_value.filter.type = DOGMA_FILTERTYPE_SKILL_REQUIRED;
		result->modifier_value.filter.typeid = resarg2.typeid_value;
		result->modifier_value.targetenv = resarg1.env_value;
		break;

	case DOGMA_RSA:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_TYPEID);
		assert(resarg2.type == DOGMA_CTXTYPE_ATTRIBUTEID);
		result->type = DOGMA_CTXTYPE_MODIFIER;
		result->modifier_value.filter.type = DOGMA_FILTERTYPE_SKILL_REQUIRED;
		result->modifier_value.filter.typeid = resarg1.typeid_value;
		result->modifier_value.targetattribute = resarg2.attributeid_value;
		result->modifier_value.targetenv = ctx->ship; /* FIXME: or char? */
		break;

	case DOGMA_IA:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		assert(resarg1.type == DOGMA_CTXTYPE_ATTRIBUTEID);
		result->type = DOGMA_CTXTYPE_ATTRIBUTEID;
		result->attributeid_value = resarg1.attributeid_value;
		break;

	case DOGMA_GA:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_GROUPID);
		assert(resarg2.type == DOGMA_CTXTYPE_ATTRIBUTEID);
		result->type = DOGMA_CTXTYPE_MODIFIER;
		result->modifier_value.filter.type = DOGMA_FILTERTYPE_GROUP;
		result->modifier_value.filter.groupid = resarg1.groupid_value;
		result->modifier_value.targetattribute = resarg2.attributeid_value;
		result->modifier_value.targetenv = ctx->ship; /* FIXME: or char? */
		break;

	case DOGMA_GM:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_ENV);
		assert(resarg2.type == DOGMA_CTXTYPE_GROUPID);
		result->type = DOGMA_CTXTYPE_MODIFIER;
		result->modifier_value.filter.type = DOGMA_FILTERTYPE_GROUP;
		result->modifier_value.filter.groupid = resarg2.groupid_value;
		result->modifier_value.targetenv = resarg1.env_value;
		break;

	case DOGMA_EFF:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_ASSOCIATION);
		assert(resarg2.type == DOGMA_CTXTYPE_MODIFIER);
		*result = resarg2;
		result->modifier_value.assoctype = resarg1.assoc_value;
		break;

		/* Modifiers */

	case DOGMA_AGGM:
		/* TODO */
		break;

	case DOGMA_RGGM:
		/* TODO */
		break;

	case DOGMA_AGIM:
		/* TODO */
		break;

	case DOGMA_RGIM:
		/* TODO */
		break;

	case DOGMA_AGORSM:
		/* TODO */
		break;

	case DOGMA_RGORSM:
		/* TODO */
		break;

	case DOGMA_AGRSM:
		/* TODO */
		break;

	case DOGMA_RGRSM:
		/* TODO */
		break;

	case DOGMA_AIM:
	case DOGMA_ALM:
	case DOGMA_ALGM:
	case DOGMA_ALRSM:
	case DOGMA_AORSM:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_MODIFIER);
		assert(resarg2.type == DOGMA_CTXTYPE_ATTRIBUTEID);
		resarg1.modifier_value.sourceattribute = resarg2.attributeid_value;
		resarg1.modifier_value.sourceenv = self;
		assert(dogma_add_modifier(&(resarg1.modifier_value)) == DOGMA_OK);
		break;

	case DOGMA_RIM:
	case DOGMA_RLM:
	case DOGMA_RLGM:
	case DOGMA_RLRSM:
	case DOGMA_RORSM:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		dogma_eval_expression(ctx, self, other, exp->arg2, &resarg2);
		assert(resarg1.type == DOGMA_CTXTYPE_MODIFIER);
		assert(resarg2.type == DOGMA_CTXTYPE_ATTRIBUTEID);
		resarg1.modifier_value.sourceattribute = resarg2.attributeid_value;
		resarg1.modifier_value.sourceenv = self;
		assert(dogma_remove_modifier(&(resarg1.modifier_value)) == DOGMA_OK);
		break;

		/* Definitions */

	case DOGMA_DEFASSOCIATION:
		result->type = DOGMA_CTXTYPE_ASSOCIATION;
		result->assoc_value = exp->assoctype;
		break;

	case DOGMA_DEFATTRIBUTE:
		result->type = DOGMA_CTXTYPE_ATTRIBUTEID;
		result->attributeid_value = exp->attributeid;
		break;

	case DOGMA_DEFBOOL:
		result->type = DOGMA_CTXTYPE_BOOL;
		result->bool_value = exp->boolv;
		break;

	case DOGMA_DEFENVIDX:
		result->type = DOGMA_CTXTYPE_ENV;
		switch(exp->envidx) {

		case DOGMA_Self:
			result->env_value = self;
			break;

		case DOGMA_Char:
			result->env_value = ctx->character;
			break;

		case DOGMA_Ship:
			result->env_value = ctx->ship;
			break;

		case DOGMA_Target:
			result->env_value = ctx->target;
			break;

		case DOGMA_Area:
			result->env_value = ctx->area;
			break;

		case DOGMA_Other:
			result->env_value = other;
			break;

		default:
			return DOGMA_NOT_FOUND;
		}
		break;

	case DOGMA_DEFFLOAT:
		result->type = DOGMA_CTXTYPE_FLOAT;
		result->float_value = exp->floatv;
		break;

	case DOGMA_DEFGROUP:
		result->type = DOGMA_CTXTYPE_GROUPID;
		result->groupid_value = exp->groupid;
		break;

	case DOGMA_DEFINT:
		result->type = DOGMA_CTXTYPE_INT;
		result->int_value = exp->intv;
		break;

	case DOGMA_DEFSTRING:
		result->type = DOGMA_CTXTYPE_STRING;
		/* value is a constant string, no need for strcpy() here */
		result->string_value = exp->value;
		break;

	case DOGMA_DEFTYPEID:
		result->type = DOGMA_CTXTYPE_TYPEID;
		result->typeid_value = exp->typeid;
		break;

		/* Checks */

	case DOGMA_SKILLCHECK:

	case DOGMA_TOOLTARGETSKILLS:

	case DOGMA_VERIFYTARGETGROUP:
		assert(false);
		break;

		/* Misc. stuff */

	case DOGMA_GETTYPE:
		dogma_eval_expression(ctx, self, other, exp->arg1, &resarg1);
		assert(exp->arg2 == 0);
		assert(resarg1.type == DOGMA_CTXTYPE_ENV);
		result->type = DOGMA_CTXTYPE_TYPEID;
		result->typeid_value = (resarg1.env_value)->id;
		break;

		/* Dummy effects */

	case DOGMA_ATTACK:
	case DOGMA_CARGOSCAN:
	case DOGMA_CHEATTELEDOCK:
	case DOGMA_CHEATTELEGATE:
	case DOGMA_DECLOAKWAVE:
	case DOGMA_ECMBURST:
	case DOGMA_EMPWAVE:
	case DOGMA_LAUNCH:
	case DOGMA_LAUNCHDEFENDERMISSILE:
	case DOGMA_LAUNCHDRONE:
	case DOGMA_LAUNCHFOFMISSILE:
	case DOGMA_MINE:
	case DOGMA_POWERBOOST:
	case DOGMA_SHIPSCAN:
	case DOGMA_SURVEYSCAN:
	case DOGMA_TARGETHOSTILES:
	case DOGMA_TARGETSILENTLY:
		break;

	case DOGMA_UE:

	default:
		return DOGMA_NOT_FOUND;
	}

	return DOGMA_OK;
}