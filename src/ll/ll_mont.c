/******************************************************************************
 *                                                                            *
 * Copyright 2020-2021 Meng-Shan Jiang                                        *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License");            *
 * you may not use this file except in compliance with the License.           *
 * You may obtain a copy of the License at                                    *
 *                                                                            *
 *    http://www.apache.org/licenses/LICENSE-2.0                              *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 *                                                                            *
 *****************************************************************************/

#include <fp256/fp256_ll.h>
#include "ll_local.h"

void ll_mont_mul(u64 *rd, const u64 *Ad, const u64 *Bd, const u64 *Nd, u64 k0, size_t l, u64 *td)
{
    size_t i;
    u64 y;

    ll_mul_limb(td, Ad, Bd[0], l);
    y = td[0] * k0;
    ll_muladd_limb(td, Nd, y, l + 1, l);
    Bd++;
    td++;

    for (i = 1; i < l; i++) {
        ll_muladd_limb(td, Ad, Bd[0], l + 2, l);
        y = td[0] * k0;
        ll_muladd_limb(td, Nd, y, l + 2, l);
        Bd++;
        td++;
    }

    ll_mont_cond_sub_limbs(rd, td, Nd, l);
    return;
}

void ll_mont_reduce(u64 *rd, const u64 *Ad2, const u64 *Nd, u64 k0, size_t l, u64 *td)
{
    size_t i;
    u64 y;

    /* copy lower l limbs */
    ll_copy_limbs(td, Ad2, l);
    /* clear higher l limbs */
    ll_set_zero(td + l, l);

    /* reduce */
    for (i = 0; i < l; i++) {
        y = td[0] * k0;
        ll_muladd_limb(td, Nd, y, l, l);
        td++;
    }

    td[l] = ll_add_limbs(td, td, rd, l);
    ll_mont_cond_sub_limbs(rd, td, Nd, l);
    return;
}

void ll_mont_sqr(u64 *rd, const u64 *Ad, const u64 *Nd, u64 k0, size_t l, u64 *td)
{
    ll_mont_mul(rd, Ad, Ad, Nd, k0, l, td);
    return;
}

void ll_to_mont(u64 *Ad, const u64 *ad, const u64 *Nd, const u64 *RR, u64 k0, size_t l, u64 *td)
{
    ll_mont_mul(Ad, ad, RR, Nd, k0, l, td);
    return;
}

void ll_from_mont(u64 *ad, const u64 *Ad, const u64 *Nd, u64 k0, size_t l, u64 *td)
{
    ll_mont_reduce(ad, Ad, Nd, k0, l, td);
    return;
}