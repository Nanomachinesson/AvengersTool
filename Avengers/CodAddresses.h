#pragma once
#ifndef CODADDRESSES_H
#define CODADDRESSES_H

constexpr int addr_inair = 0x007944E4;  //1023 - in air, !1023 not
constexpr int addr_velocity = 0x0079449C;
constexpr int addr_velocity_writeable = 0x013255D0;
constexpr int addr_position = 0x00794490;
constexpr int addr_position_writeable = 0x013255C4;
constexpr int addr_ingame = 0xC5F900;
constexpr int addr_timescale = 0x0CBABC88;
constexpr int addr_view = 0x79B698;  //vec3f
constexpr int addr_maxfps_wtmod = 0x00795150;
constexpr int addr_maxfps_3xp = 0x00771F84;
constexpr int addr_delta_angles = 0x0074E3D0;
constexpr int addr_usercmd = 0x00CC4FF8;
constexpr int addr_sprint = 0x00794560; // >= 20 is sprint
constexpr int addr_lean = 0x007944D0;
constexpr int addr_maxfps = 0x0CBAB8AC;  //float
constexpr int addr_engine_draw = 0x658860;
constexpr int addr_writeableAngles = 0x00C84FD8;
constexpr int addr_deltaAngles = 0x0074E3D0;
constexpr int addr_pMove_current = 0x008C9C90;
constexpr int addr_stepslidemove = 0x004155c0;
constexpr int addr_projectvelocity = 0x0040e330;
constexpr int addr_projectvelocity_in_stepslidemove = 0x004159bc;
constexpr int addr_clipmap_t = 0x14098C0;
constexpr int addr_material_commands = 0xD085EE0;
constexpr int addr_gfxcmdbufstate = 0xD5404F0;
constexpr int addr_gfxcmdbufsourcestate = 0xD53F5F0;
constexpr int addr_gfxworld = 0xD0701E0;
constexpr int addr_rb_endtesssurface = 0x61A2F0;
constexpr int addr_r_global_permanent_t = 0xCC98280;
constexpr int addr_cg_predictPlayerState = 0x00447260;

#endif
