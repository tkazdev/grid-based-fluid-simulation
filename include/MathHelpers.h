#pragma once

static inline int squareInt(int x) { return x * x; }
static inline int maxInt(int a, int b) { return a > b ? a : b; }
static inline int minInt(int a, int b) { return a < b ? a : b; }
static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
static inline float lerpQuad(float tl, float tr, float bl, float br, float tx, float ty) {
    return lerp(lerp(bl, br, tx), lerp(tl, tr, tx), ty);
}

static inline int clampInt(int v, int min, int max) {
    int r = (v < min) ? min : v;
    return (r > max) ? max : v;
}