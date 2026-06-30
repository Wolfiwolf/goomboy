#include "number.h"
#include "renderer.h"
#include "wd.h"
#include <stdio.h>
#include <string.h>

void number_init(number_t *n, int x, int y)
{
	int i;

	memset(n, 0, sizeof(number_t));

	n->go.x = x;
	n->go.y = y;
	n->val = -1;

	for (i = 0; i < NUMBER_MAX_DIGITS; ++i) {
		number_digit_t *dig = &n->digits[i];
		dig->ro.parent = &dig->go;
		dig->go.active = false;
	}
}

static int _num_of_digits(int val)
{
	int num_of_digits = 0;
	int num;

	num = val;
	while (num) {
		num /= 10;
		num_of_digits += 1;
	}

	return num_of_digits;
}

static int _get_digits(int val, int *digits)
{
	int num_of_digits = _num_of_digits(val);
	int num;
	int i;

	if (num_of_digits == 0) {
		digits[0] = 0;
		return 1;
	}

	num = val;
	for (i = num_of_digits-1; i >= 0; --i) {
		int d = num % 10;

		digits[i] = d;

		num /= 10;
	}

	return num_of_digits;
}

void number_set_val(number_t *n, int val)
{
	int digits[NUMBER_MAX_DIGITS];
	int i;

	if (n->val == val)
		return;

	n->num_of_digits = _get_digits(val, digits);

	for (i = 0; i < n->num_of_digits; ++i) {
		number_digit_t *dig, *prev_dig;
		const asset_info_t *ass_inf;
		int d = digits[i];

		dig = &n->digits[i];

		ass_inf = wd_get_asset_info(ASSET_TYPE_NUM0+d)	;

		// Cleanup if it was set
		if (dig->go.active)
			wd_not_using(ASSET_TYPE_NUM0+dig->val);

		if (i == 0)
			prev_dig = NULL;
		else
			prev_dig = &n->digits[i-1];

		if (prev_dig)
			dig->go.x = prev_dig->go.x + ((float)prev_dig->ro.w);
		else
			dig->go.x = n->go.x;
			
		dig->go.y = n->go.y;
		dig->val = d;

		dig->ro.buff = wd_get_asset(ASSET_TYPE_NUM0+d);
		dig->ro.w = ass_inf->w;
		dig->ro.h = ass_inf->h;
		dig->go.active = true;
	}
}

void number_destroy(number_t *n)
{
	int i;

	for (i = 0; i < NUMBER_MAX_DIGITS; ++i) {
		number_digit_t *dig = &n->digits[i];
		if (dig->go.active)
			wd_not_using(ASSET_TYPE_NUM0+dig->val);
	}
}

void number_render(number_t *n)
{
	int i;

	for (i = 0; i < n->num_of_digits; ++i) {
		number_digit_t *dig = &n->digits[i];

		if (dig->go.active)
			renderer_render(&dig->ro);
	}
}
