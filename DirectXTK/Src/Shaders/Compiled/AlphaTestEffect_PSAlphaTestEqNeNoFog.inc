#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// COLOR                    0   xyzw        0     NONE   float   xyzw
// TEXCOORD                 0   xy          1     NONE   float   xy  
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Target                0   xyzw        0   TARGET   float   xyzw
//
//
// Constant buffer to DX9 shader constant mappings:
//
// Target Reg Buffer  Start Reg # of Regs        Data Conversion
// ---------- ------- --------- --------- ----------------------
// c0         cb0             1         1  ( FLT, FLT, FLT, FLT)
//
//
// Sampler/Resource to DX9 shader sampler mappings:
//
// Target Sampler Source Sampler  Source Resource
// -------------- --------------- ----------------
// s0             s0              t0               
//
//
// Level9 shader bytecode:
//
    ps_2_0
    dcl t0  // pin<0,1,2,3>
    dcl t1.xy  // pin<4,5>
    dcl_2d s0

#line 123 "D:\directx-game-BenAlford\DirectXTK\Src\Shaders\AlphaTestEffect.fx"
    texld r0, t1, s0
    mad r1.w, r0.w, t0.w, -c0.x
    mul r0, r0, t0  // ::color<0,1,2,3>
    mov oC0, r0  // ::PSAlphaTestEqNeNoFog<0,1,2,3>
    abs r0.x, r1.w
    add r0.x, r0.x, -c0.y
    cmp r0, r0.x, c0.w, c0.z
    texkill r0

// approximately 8 instruction slots used (1 texture, 7 arithmetic)
ps_4_0
dcl_constantbuffer CB0[2], immediateIndexed
dcl_sampler s0, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_input_ps linear v0.xyzw
dcl_input_ps linear v1.xy
dcl_output o0.xyzw
dcl_temps 2
sample r0.xyzw, v1.xyxx, t0.xyzw, s0
mad r1.x, r0.w, v0.w, -cb0[1].x
mul r0.xyzw, r0.xyzw, v0.xyzw
mov o0.xyzw, r0.xyzw
lt r0.x, |r1.x|, cb0[1].y
movc r0.x, r0.x, cb0[1].z, cb0[1].w
lt r0.x, r0.x, l(0.000000)
discard_nz r0.x
ret 
// Approximately 0 instruction slots used
#endif

const BYTE AlphaTestEffect_PSAlphaTestEqNeNoFog[] =
{
     68,  88,  66,  67, 184, 222, 
    209, 161, 179, 101,  59, 121, 
     69,  28, 200, 207,  15,  90, 
     50, 140,   1,   0,   0,   0, 
    228,   4,   0,   0,   4,   0, 
      0,   0,  48,   0,   0,   0, 
    252,   2,   0,   0,  96,   4, 
      0,   0, 176,   4,   0,   0, 
     65, 111, 110,  57, 196,   2, 
      0,   0, 196,   2,   0,   0, 
      0,   2, 255, 255, 144,   2, 
      0,   0,  52,   0,   0,   0, 
      1,   0,  40,   0,   0,   0, 
     52,   0,   0,   0,  52,   0, 
      1,   0,  36,   0,   0,   0, 
     52,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   2, 255, 255, 254, 255, 
    122,   0,  68,  66,  85,  71, 
     40,   0,   0,   0, 188,   1, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 108,   0, 
      0,   0,  11,   0,   0,   0, 
    112,   0,   0,   0,   3,   0, 
      0,   0, 128,   1,   0,   0, 
    200,   0,   0,   0,  68,  58, 
     92, 100, 105, 114, 101,  99, 
    116, 120,  45, 103,  97, 109, 
    101,  45,  66, 101, 110,  65, 
    108, 102, 111, 114, 100,  92, 
     68, 105, 114, 101,  99, 116, 
     88,  84,  75,  92,  83, 114, 
     99,  92,  83, 104,  97, 100, 
    101, 114, 115,  92,  65, 108, 
    112, 104,  97,  84, 101, 115, 
    116,  69, 102, 102, 101,  99, 
    116,  46, 102, 120,   0, 171, 
     40,   0,   0,   0,   0,   0, 
    255, 255, 240,   1,   0,   0, 
      0,   0, 255, 255, 252,   1, 
      0,   0,   0,   0, 255, 255, 
      8,   2,   0,   0, 123,   0, 
      0,   0,  20,   2,   0,   0, 
    125,   0,   0,   0,  36,   2, 
      0,   0, 123,   0,   0,   0, 
     56,   2,   0,   0, 123,   0, 
      0,   0,  72,   2,   0,   0, 
    125,   0,   0,   0,  84,   2, 
      0,   0, 125,   0,   0,   0, 
     96,   2,   0,   0, 125,   0, 
      0,   0, 112,   2,   0,   0, 
    125,   0,   0,   0, 132,   2, 
      0,   0,  80,  83,  65, 108, 
    112, 104,  97,  84, 101, 115, 
    116,  69, 113,  78, 101,  78, 
    111,  70, 111, 103,   0, 171, 
    171, 171,   1,   0,   3,   0, 
      1,   0,   4,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,   0,   0,   0,   0, 
      1,   0,   2,   0,   3,   0, 
     99, 111, 108, 111, 114,   0, 
    171, 171,   1,   0,   3,   0, 
      1,   0,   4,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,   0,   0, 
      1,   0,   2,   0,   3,   0, 
    112, 105, 110,   0,  68, 105, 
    102, 102, 117, 115, 101,   0, 
     84, 101, 120,  67, 111, 111, 
    114, 100,   0, 171, 171, 171, 
      1,   0,   3,   0,   1,   0, 
      2,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  36,   1, 
      0,   0,   4,   1,   0,   0, 
     44,   1,   0,   0,  56,   1, 
      0,   0,   5,   0,   0,   0, 
      1,   0,   6,   0,   1,   0, 
      2,   0,  72,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   2,   0,   3,   0, 
      1,   0,   0,   0,   4,   0, 
      5,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 200,   0, 
      0,   0, 224,   0,   0,   0, 
      1,   0,   0,   0, 240,   0, 
      0,   0,   0,   0,   0,   0, 
    252,   0,   0,   0,   4,   1, 
      0,   0,   1,   0,   0,   0, 
     20,   1,   0,   0, 200,   0, 
      0,   0,  32,   1,   0,   0, 
     88,   1,   0,   0,   2,   0, 
      0,   0, 104,   1,   0,   0, 
     77, 105,  99, 114, 111, 115, 
    111, 102, 116,  32,  40,  82, 
     41,  32,  72,  76,  83,  76, 
     32,  83, 104,  97, 100, 101, 
    114,  32,  67, 111, 109, 112, 
    105, 108, 101, 114,  32,  49, 
     48,  46,  49,   0,  31,   0, 
      0,   2,   0,   0,   0, 128, 
      0,   0,  15, 176,  31,   0, 
      0,   2,   0,   0,   0, 128, 
      1,   0,   3, 176,  31,   0, 
      0,   2,   0,   0,   0, 144, 
      0,   8,  15, 160,  66,   0, 
      0,   3,   0,   0,  15, 128, 
      1,   0, 228, 176,   0,   8, 
    228, 160,   4,   0,   0,   4, 
      1,   0,   8, 128,   0,   0, 
    255, 128,   0,   0, 255, 176, 
      0,   0,   0, 161,   5,   0, 
      0,   3,   0,   0,  15, 128, 
      0,   0, 228, 128,   0,   0, 
    228, 176,   1,   0,   0,   2, 
      0,   8,  15, 128,   0,   0, 
    228, 128,  35,   0,   0,   2, 
      0,   0,   1, 128,   1,   0, 
    255, 128,   2,   0,   0,   3, 
      0,   0,   1, 128,   0,   0, 
      0, 128,   0,   0,  85, 161, 
     88,   0,   0,   4,   0,   0, 
     15, 128,   0,   0,   0, 128, 
      0,   0, 255, 160,   0,   0, 
    170, 160,  65,   0,   0,   1, 
      0,   0,  15, 128, 255, 255, 
      0,   0,  83,  72,  68,  82, 
     92,   1,   0,   0,  64,   0, 
      0,   0,  87,   0,   0,   0, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      0,   0,   0,   0,  88,  24, 
      0,   4,   0, 112,  16,   0, 
      0,   0,   0,   0,  85,  85, 
      0,   0,  98,  16,   0,   3, 
    242,  16,  16,   0,   0,   0, 
      0,   0,  98,  16,   0,   3, 
     50,  16,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   0,   0, 
      0,   0, 104,   0,   0,   2, 
      2,   0,   0,   0,  69,   0, 
      0,   9, 242,   0,  16,   0, 
      0,   0,   0,   0,  70,  16, 
     16,   0,   1,   0,   0,   0, 
     70, 126,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  11,  18,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     58,  16,  16,   0,   0,   0, 
      0,   0,  10, 128,  32, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     56,   0,   0,   7, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  30,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 242,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     49,   0,   0,   9,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16, 128, 129,   0, 
      0,   0,   1,   0,   0,   0, 
     26, 128,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     55,   0,   0,  11,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  42, 128,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  58, 128,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  49,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     13,   0,   4,   3,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     62,   0,   0,   1,  73,  83, 
     71,  78,  72,   0,   0,   0, 
      2,   0,   0,   0,   8,   0, 
      0,   0,  56,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,  15, 
      0,   0,  62,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   3,   3, 
      0,   0,  67,  79,  76,  79, 
     82,   0,  84,  69,  88,  67, 
     79,  79,  82,  68,   0, 171, 
     79,  83,  71,  78,  44,   0, 
      0,   0,   1,   0,   0,   0, 
      8,   0,   0,   0,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
     95,  84,  97, 114, 103, 101, 
    116,   0, 171, 171
};
