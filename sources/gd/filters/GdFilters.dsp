import("stdfaust.lib");

///
declare author "Jean Pierre Cimalando";
declare license "BSD-2-Clause";

///
process = one, one with {
  one = (cf, rs, _) <: ((!, !, _), lpReson6dB, hpReson6dB, lpReson12dB, hpReson12dB) : ba.selectn(5, ty);
  ty = hslider("[0] type [style:menu{'Off':0;'LP6':1;'HP6':2;'LP12':3;'HP12':4}]", 0, 0, 4, 1);
  cf = hslider("[1] cutoff [scale:log]", 500.0, 1.0, 20000.0, 1.0);
  rs = hslider("[2] resonance", 0.0, 0.0, 20.0, 0.01) : ba.db2linear;
};

///
entryLP6 = (controls.cf, controls.rs, _) : lpReson6dB;
entryHP6 = (controls.cf, controls.rs, _) : hpReson6dB;
entryLP12 = (controls.cf, controls.rs, _) : lpReson12dB;
entryHP12 = (controls.cf, controls.rs, _) : hpReson12dB;

controls = environment {
  cf = hslider("[1] cutoff [scale:log]", 500.0, 1.0, 20000.0, 1.0);
  rs = hslider("[2] resonance", 0.0, 0.0, ba.db2linear(24.0), 0.001);
};

///
lpReson6dB(f, q) = lp : peak with {
  lp = fi.lowpass(1, f);
  peak = fi.tf22t(b0/a0,b1/a0,b2/a0,a1/a0,a2/a0);
  w = f*(2*ma.PI/ma.SR);
  A = sqrt(q);
  S = sin(w); C = cos(w);
  b0 = 1+S*A; b1 =-2*C; b2 = 1-S*A;
  a0 = 1+S/A; a1 =-2*C; a2 = 1-S/A;
};

hpReson6dB(f, q) = lp : peak with {
  lp = fi.highpass(1, f);
  peak = fi.tf22t(b0/a0,b1/a0,b2/a0,a1/a0,a2/a0);
  w = f*(2*ma.PI/ma.SR);
  A = sqrt(q);
  S = sin(w); C = cos(w);
  b0 = 1+S*A; b1 =-2*C; b2 = 1-S*A;
  a0 = 1+S/A; a1 =-2*C; a2 = 1-S/A;
};

///
lpReson12dB(f, q) = fi.tf22t(b0/a0,b1/a0,b2/a0,a1/a0,a2/a0) with {
  a = sin(w)/(2*q);
  w = f*(2*ma.PI/ma.SR);
  b0 = 0.5*(1.0-cos(w)); b1 = 1.0-cos(w); b2 = 0.5*(1.0-cos(w));
  a0 = 1.0+a; a1 = -2.0*cos(w); a2 = 1.0-a;
};

hpReson12dB(f, q) = fi.tf22t(b0/a0,b1/a0,b2/a0,a1/a0,a2/a0) with {
  a = sin(w)/(2*q);
  w = f*(2*ma.PI/ma.SR);
  b0 = 0.5*(1.0+cos(w)); b1 = -1.0-cos(w); b2 = 0.5*(1.0+cos(w));
  a0 = 1.0+a; a1 = -2.0*cos(w); a2 = 1.0-a;
};
