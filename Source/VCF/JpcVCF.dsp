// SPDX-License-Identifier: ISC

declare license "ISC";
declare author "Jean Pierre Cimalando";

import("stdfaust.lib");

vcf(f, r) = (+ : cascade) ~ feedback with {
  cascade = seq(i, 4, poleEntry : tptPole(f));
  feedback = *(tanhK) : tableTanh : *(-4.0*r);
  poleEntry = +(noiseA*no.noise) : *(tanhK) : tableTanh;
  tptPole(f) = next with {
    next(x) = y letrec {
    'y = s+G*(x-s);
    's = s+G*(x-s)+G*(x-s);
  };
    g = f*(ma.PI/ma.SR);
    G = g/(1.0+g);
  };
  tanhK = 1.0;
  noiseA = ba.db2linear(-80.0);
};

tableTanh(x) = value with {
  extent = 3.0;
  points = 128;
  //
  expr = (ba.time/float(points)) : *(2.0*extent) : -(extent) : ma.tanh;
  read = max(0) : min(points-1) : rdtable(points, expr);
  //
  pos = x : +(extent) : *(points*0.5/extent);
  mu = pos-int(pos);
  value = read(int(pos))+mu*(read(1+int(pos))-read(int(pos)));
};

process(in, fc, res) = in : vcf(fc, res);
