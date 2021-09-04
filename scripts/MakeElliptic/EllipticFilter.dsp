/* Copyright (c) 2021, Jean Pierre Cimalando
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

import("stdfaust.lib");

directForm(data, fc) = fi.iir((par(i, data.NB, b(i))), (par(i, data.NA, a(i)))) with {
  nf = int(0.5+(data.NF-1)*(fc-data.F0)/(data.F1-data.F0)) : max(0) : min(data.NF-1);
  vpf = data.NB+data.NA;
  off = nf*vpf;
  b(i) = (data.NF*vpf, data.BA, off+i) : rdtable;
  a(i) = b(data.NB+i);
};

secondOrderSections(data, fc) = seq(i, data.NS, bq(i)) : *(g) with {
  nf = int(0.5+(data.NF-1)*(fc-data.F0)/(data.F1-data.F0)) : max(0) : min(data.NF-1);
  vpf = 5*data.NS+1;
  off = nf*vpf;
  tab(i) = (data.NF*vpf, data.BA, off+i) : rdtable;
  bq(i) = fi.tf22t(tab(5*i), tab(5*i+1), tab(5*i+2), tab(5*i+3), tab(5*i+4));
  g = tab(vpf-1);
};

// Generate with
// -------------
// ./MakeElliptic.jl --lang Faust > EllipticFilterDirect.dsp
// ./MakeElliptic.jl --lang Faust --sos > EllipticFilterSOS.dsp

directData = library("EllipticFilterDirect.dsp");
secondOrderData = library("EllipticFilterSOS.dsp");

process = _ <: (filterDirect, filterSOS) :> select2(sel) with {
  cutoff = hslider("[1] cutoff", 0.5, 0.0, 0.5, 0.001);
  sel = checkbox("[2] second-order sections");
  filterDirect = directForm(directData, cutoff);
  filterSOS = secondOrderSections(secondOrderData, cutoff);
};
