; KISSlicer - PRO
; Windows
; version 1.0.9.7
; Built: Jun 13 2012, 22:16:39
;
; Saved: Sat Jul 21 02:29:03 2012
; 'Simple.KISS.gcode'
;
; *** Printer Settings ***
;
; printer_name = Prusa
; extension = gcode
; cost_per_hour = 0
; g_code_prefix = NULL
; g_code_postfix = NULL
; g_code_ext_change = NULL
; post_process = NULL
; num_extruders = 1
; firmware_type = 2
; M104_M109 = 1
; add_comments = 1
; z_speed_mm_per_s = 3.5
; z_settle_mm = 0.25
; bed_size_x_mm = 180
; bed_size_y_mm = 190
; bed_size_z_mm = 150
; bed_offset_x_mm = 90
; bed_offset_y_mm = 95
; bed_offset_z_mm = 0
; bed_roughness_mm = 0
; travel_speed_mm_per_s = 90
; first_layer_speed_mm_per_s = 10
; lo_speed_perim_mm_per_s = 4
; lo_speed_solid_mm_per_s = 15
; lo_speed_sparse_mm_per_s = 15
; hi_speed_perim_mm_per_s = 50
; hi_speed_solid_mm_per_s = 60
; hi_speed_sparse_mm_per_s = 60
; ext_mm_per_rev_1 = 1
; ext_material_1 = 2
; ext_axis_1 = 0
; ext_mm_per_rev_2 = 1
; ext_material_2 = 4
; ext_axis_2 = 0
; ext_mm_per_rev_3 = 1.25
; ext_material_3 = 0
; ext_axis_3 = 0
; model_ext = 0
; support_ext = 0
; support_body_ext = 0
; raft_ext = 0
; solid_loop_overlap_fraction = 1
;
; *** Material Settings for Extruder 1 ***
;
; material_name = ABS
; fan_off_ISOs_all = 1
; temperature_C = 220
; keep_warm_C = 220
; first_layer_C = 220
; bed_C = 110
; sec_per_C_per_C = 0.0015
; RPM_min = 10
; RPM_max = 300
; destring_suck = 10
; destring_prime = 10
; destring_min_mm = 1
; destring_speed_mm_per_s = 30
; min_layer_time_s = 10
; wipe_mm = 2
; cost_per_cm3 = 0.0712331
; flowrate_tweak = 1
; fiber_dia_mm = 1.75
; color = -16774400
;
; *** Style Settings ***
;
; style_name = Normal
; layer_thickness_mm = 0.2
; extrusion_width_mm = 0.4
; num_loops = 2
; skin_thickness_mm = 1.2
; infill_extrusion_width = 0.4
; infill_density_denominator = 2
; stacked_layers = 1
; use_destring = 1
; loops_insideout = 1
; round_infill = 1
;
; *** Actual Slicing Settings As Used ***
;
; layer_thickness_mm = 0.2
; extrusion_width = 0.4
; num_ISOs = 2
; wall_thickness = 1.2
; infill_style = 3
; support_style = 0
; support_angle = 90
; destring_min_mm = 1
; stacked_infill_layers = 1
; ISO_wipe_mm = 2
; raft_style = 0
; extra_raft_depth = 0
; oversample_res_mm = 0.125
; crowning_threshold_mm = -1
; loops_insideout = 1
; sheathed_support = 0
; solid_loop_overlap_fraction = 1
; inflate_raft_mm = 0
; inflate_support_mm = 1
; model_support_gap_mm = 0.5
; round_infill = 1
; Speed vs Quality = 0.40
;
; *** Header ***
;
; fan off
M107
; [mm] mode
G21
; absolute mode
G90
; *** Main G-code ***
;
; Reset extruder pos
G92 E0
; BEGIN_LAYER_OBJECT z=0.20
;
; *** Warming Extruder 1 to 220 C ***
M109 S220
G92 E0
;
; *** Cooling...speed multiplier is 0.119811 ***
;
; 'Loop', 10.0 [RPM], 5.0 [mm/s]
G1 F5400
G1 X90.38 Y95.38 Z0.45 E0
G1 F210
G1 X90.38 Y95.38 Z0.2 E0
G1 F300.7
G1 X89.62 Y95.38 E0.0254
G1 X89.62 Y94.62 E0.0509
G1 X90.38 Y94.62 E0.0763
G1 X90.38 Y95.38 E0.1018
;
; 'Perimeter', 10.0 [RPM], 5.0 [mm/s]
G1 F647
G1 X90.64 Y95.54 E0.1018
G1 F300.7
G1 X90.79 Y95.79 E0.1111
G1 X89.21 Y95.79 E0.1634
G1 X89.21 Y94.21 E0.2156
G1 X90.79 Y94.21 E0.2679
G1 X90.79 Y95.79 E0.3202
G1 X90.54 Y95.64 E0.3295
;
; 'Wipe', 0.0 [RPM], 1.2 [mm/s]
G1 F647
G1 X90.54 Y95.64 E0.3295
G1 F1800
G1 E-9.6705
G1 F71.9
G1 X90.79 Y95.79 E-9.6705
G1 X90.79 Y94.21 E-9.6705
G1 X90.6 Y94.21 E-9.6705
G1 F210
G1 X90.6 Y94.21 Z0.65 E-9.6705
; END_LAYER_OBJECT z=0.20
; Reset extruder pos
G92 E0
; BEGIN_LAYER_OBJECT z=0.40
; *** Cooling...speed multiplier is 0.034722 ***
; fan on
M106
;
; 'Loop', 10.0 [RPM], 5.0 [mm/s]
G1 F5400
G1 X90.38 Y95.38 E0
G1 F210
G1 X90.38 Y95.38 Z0.4 E0
G1 F1800
G1 E10
G1 F300.7
G1 X89.62 Y95.38 E10.0254
G1 X89.62 Y94.62 E10.0509
G1 X90.38 Y94.62 E10.0763
G1 X90.38 Y95.38 E10.1018
;
; 'Perimeter', 10.0 [RPM], 5.0 [mm/s]
G1 F187.5
G1 X90.64 Y95.54 E10.1018
G1 F300.7
G1 X90.79 Y95.79 E10.1111
G1 X89.21 Y95.79 E10.1634
G1 X89.21 Y94.21 E10.2156
G1 X90.79 Y94.21 E10.2679
G1 X90.79 Y95.79 E10.3202
G1 X90.54 Y95.64 E10.3295
; fan off
M107
;
; 'Wipe', 0.0 [RPM], 1.5 [mm/s]
G1 F187.5
G1 X90.54 Y95.64 E10.3295
G1 F1800
G1 E0.3295
G1 F87.5
G1 X90.79 Y95.79 E0.3295
G1 X90.79 Y94.21 E0.3295
G1 X90.6 Y94.21 E0.3295
G1 F210
G1 X90.6 Y94.21 Z0.85 E0.3295
; END_LAYER_OBJECT z=0.40
; Reset extruder pos
G92 E0
; BEGIN_LAYER_OBJECT z=0.60
; *** Cooling...speed multiplier is 0.034722 ***
; fan on
M106
;
; 'Loop', 10.0 [RPM], 5.0 [mm/s]
G1 F5400
G1 X90.38 Y95.38 E0
G1 F210
G1 X90.38 Y95.38 Z0.6 E0
G1 F1800
G1 E10
G1 F300.7
G1 X89.62 Y95.38 E10.0254
G1 X89.62 Y94.62 E10.0509
G1 X90.38 Y94.62 E10.0763
G1 X90.38 Y95.38 E10.1018
;
; 'Perimeter', 10.0 [RPM], 5.0 [mm/s]
G1 F187.5
G1 X90.64 Y95.54 E10.1018
G1 F300.7
G1 X90.79 Y95.79 E10.1111
G1 X89.21 Y95.79 E10.1634
G1 X89.21 Y94.21 E10.2156
G1 X90.79 Y94.21 E10.2679
G1 X90.79 Y95.79 E10.3202
G1 X90.54 Y95.64 E10.3295
; fan off
M107
;
; 'Wipe', 0.0 [RPM], 1.5 [mm/s]
G1 F187.5
G1 X90.54 Y95.64 E10.3295
G1 F1800
G1 E0.3295
G1 F87.5
G1 X90.79 Y95.79 E0.3295
G1 X90.79 Y94.21 E0.3295
G1 X90.6 Y94.21 E0.3295
G1 F210
G1 X90.6 Y94.21 Z1.05 E0.3295
; END_LAYER_OBJECT z=0.60
; Reset extruder pos
G92 E0
; BEGIN_LAYER_OBJECT z=0.80
; *** Cooling...speed multiplier is 0.034722 ***
; fan on
M106
;
; 'Loop', 10.0 [RPM], 5.0 [mm/s]
G1 F5400
G1 X90.38 Y95.38 E0
G1 F210
G1 X90.38 Y95.38 Z0.8 E0
G1 F1800
G1 E10
G1 F300.7
G1 X89.62 Y95.38 E10.0254
G1 X89.62 Y94.62 E10.0509
G1 X90.38 Y94.62 E10.0763
G1 X90.38 Y95.38 E10.1018
;
; 'Perimeter', 10.0 [RPM], 5.0 [mm/s]
G1 F187.5
G1 X90.64 Y95.54 E10.1018
G1 F300.7
G1 X90.79 Y95.79 E10.1111
G1 X89.21 Y95.79 E10.1634
G1 X89.21 Y94.21 E10.2156
G1 X90.79 Y94.21 E10.2679
G1 X90.79 Y95.79 E10.3202
G1 X90.54 Y95.64 E10.3295
; fan off
M107
;
; 'Wipe', 0.0 [RPM], 1.5 [mm/s]
G1 F187.5
G1 X90.54 Y95.64 E10.3295
G1 F1800
G1 E0.3295
G1 F87.5
G1 X90.79 Y95.79 E0.3295
G1 X90.79 Y94.21 E0.3295
G1 X90.6 Y94.21 E0.3295
G1 F210
G1 X90.6 Y94.21 Z1.25 E0.3295
; END_LAYER_OBJECT z=0.80
; Reset extruder pos
G92 E0
; BEGIN_LAYER_OBJECT z=1.00
; *** Cooling...speed multiplier is 0.034722 ***
; fan on
M106
;
; 'Loop', 10.0 [RPM], 5.0 [mm/s]
G1 F5400
G1 X90.38 Y95.38 E0
G1 F210
G1 X90.38 Y95.38 Z1 E0
G1 F1800
G1 E10
G1 F300.7
G1 X89.62 Y95.38 E10.0254
G1 X89.62 Y94.62 E10.0509
G1 X90.38 Y94.62 E10.0763
G1 X90.38 Y95.38 E10.1018
;
; 'Perimeter', 10.0 [RPM], 5.0 [mm/s]
G1 F187.5
G1 X90.64 Y95.54 E10.1018
G1 F300.7
G1 X90.79 Y95.79 E10.1111
G1 X89.21 Y95.79 E10.1634
G1 X89.21 Y94.21 E10.2156
G1 X90.79 Y94.21 E10.2679
G1 X90.79 Y95.79 E10.3202
G1 X90.54 Y95.64 E10.3295
; fan off
M107
;
; 'Wipe', 0.0 [RPM], 1.5 [mm/s]
G1 F187.5
G1 X90.54 Y95.64 E10.3295
G1 F1800
G1 E0.3295
G1 F87.5
G1 X90.79 Y95.79 E0.3295
G1 X90.79 Y94.21 E0.3295
G1 X90.6 Y94.21 E0.3295
G1 F210
G1 X90.6 Y94.21 Z1.45 E0.3295
; END_LAYER_OBJECT z=1.00
; Reset extruder pos
G92 E0
; BEGIN_LAYER_OBJECT z=1.20
; *** Cooling...speed multiplier is 0.034722 ***
; fan on
M106
;
; 'Loop', 10.0 [RPM], 5.0 [mm/s]
G1 F5400
G1 X90.38 Y95.38 E0
G1 F210
G1 X90.38 Y95.38 Z1.2 E0
G1 F1800
G1 E10
G1 F300.7
G1 X89.62 Y95.38 E10.0254
G1 X89.62 Y94.62 E10.0509
G1 X90.38 Y94.62 E10.0763
G1 X90.38 Y95.38 E10.1018
;
; 'Perimeter', 10.0 [RPM], 5.0 [mm/s]
G1 F187.5
G1 X90.64 Y95.54 E10.1018
G1 F300.7
G1 X90.79 Y95.79 E10.1111
G1 X89.21 Y95.79 E10.1634
G1 X89.21 Y94.21 E10.2156
G1 X90.79 Y94.21 E10.2679
G1 X90.79 Y95.79 E10.3202
G1 X90.54 Y95.64 E10.3295
; fan off
M107
;
; 'Wipe', 0.0 [RPM], 1.5 [mm/s]
G1 F187.5
G1 X90.54 Y95.64 E10.3295
G1 F1800
G1 E0.3295
G1 F87.5
G1 X90.79 Y95.79 E0.3295
G1 X90.79 Y94.21 E0.3295
G1 X90.6 Y94.21 E0.3295
G1 F210
G1 X90.6 Y94.21 Z1.65 E0.3295
; END_LAYER_OBJECT z=1.20
; Reset extruder pos
G92 E0
; BEGIN_LAYER_OBJECT z=1.40
; *** Cooling...speed multiplier is 0.034722 ***
; fan on
M106
;
; 'Loop', 10.0 [RPM], 5.0 [mm/s]
G1 F5400
G1 X90.38 Y95.38 E0
G1 F210
G1 X90.38 Y95.38 Z1.4 E0
G1 F1800
G1 E10
G1 F300.7
G1 X89.62 Y95.38 E10.0254
G1 X89.62 Y94.62 E10.0509
G1 X90.38 Y94.62 E10.0763
G1 X90.38 Y95.38 E10.1018
;
; 'Perimeter', 10.0 [RPM], 5.0 [mm/s]
G1 F187.5
G1 X90.64 Y95.54 E10.1018
G1 F300.7
G1 X90.79 Y95.79 E10.1111
G1 X89.21 Y95.79 E10.1634
G1 X89.21 Y94.21 E10.2156
G1 X90.79 Y94.21 E10.2679
G1 X90.79 Y95.79 E10.3202
G1 X90.54 Y95.64 E10.3295
; fan off
M107
;
; 'Wipe', 0.0 [RPM], 1.5 [mm/s]
G1 F187.5
G1 X90.54 Y95.64 E10.3295
G1 F1800
G1 E0.3295
G1 F87.5
G1 X90.79 Y95.79 E0.3295
G1 X90.79 Y94.21 E0.3295
G1 X90.6 Y94.21 E0.3295
G1 F210
G1 X90.6 Y94.21 Z1.85 E0.3295
; END_LAYER_OBJECT z=1.40
; Reset extruder pos
G92 E0
; BEGIN_LAYER_OBJECT z=1.60
; *** Cooling...speed multiplier is 0.034722 ***
; fan on
M106
;
; 'Loop', 10.0 [RPM], 5.0 [mm/s]
G1 F5400
G1 X90.38 Y95.38 E0
G1 F210
G1 X90.38 Y95.38 Z1.6 E0
G1 F1800
G1 E10
G1 F300.7
G1 X89.62 Y95.38 E10.0254
G1 X89.62 Y94.62 E10.0509
G1 X90.38 Y94.62 E10.0763
G1 X90.38 Y95.38 E10.1018
;
; 'Perimeter', 10.0 [RPM], 5.0 [mm/s]
G1 F187.5
G1 X90.64 Y95.54 E10.1018
G1 F300.7
G1 X90.79 Y95.79 E10.1111
G1 X89.21 Y95.79 E10.1634
G1 X89.21 Y94.21 E10.2156
G1 X90.79 Y94.21 E10.2679
G1 X90.79 Y95.79 E10.3202
G1 X90.54 Y95.64 E10.3295
; fan off
M107
;
; 'Wipe', 0.0 [RPM], 1.5 [mm/s]
G1 F187.5
G1 X90.54 Y95.64 E10.3295
G1 F1800
G1 E0.3295
G1 F87.5
G1 X90.79 Y95.79 E0.3295
G1 X90.79 Y94.21 E0.3295
G1 X90.6 Y94.21 E0.3295
G1 F210
G1 X90.6 Y94.21 Z2.05 E0.3295
; END_LAYER_OBJECT z=1.60
; Reset extruder pos
G92 E0
; BEGIN_LAYER_OBJECT z=1.80
; *** Cooling...speed multiplier is 0.034722 ***
; fan on
M106
;
; 'Loop', 10.0 [RPM], 5.0 [mm/s]
G1 F5400
G1 X90.38 Y95.38 E0
G1 F210
G1 X90.38 Y95.38 Z1.8 E0
G1 F1800
G1 E10
G1 F300.7
G1 X89.62 Y95.38 E10.0254
G1 X89.62 Y94.62 E10.0509
G1 X90.38 Y94.62 E10.0763
G1 X90.38 Y95.38 E10.1018
;
; 'Perimeter', 10.0 [RPM], 5.0 [mm/s]
G1 F187.5
G1 X90.64 Y95.54 E10.1018
G1 F300.7
G1 X90.79 Y95.79 E10.1111
G1 X89.21 Y95.79 E10.1634
G1 X89.21 Y94.21 E10.2156
G1 X90.79 Y94.21 E10.2679
G1 X90.79 Y95.79 E10.3202
G1 X90.54 Y95.64 E10.3295
; fan off
M107
;
; 'Wipe', 0.0 [RPM], 1.5 [mm/s]
G1 F187.5
G1 X90.54 Y95.64 E10.3295
G1 F1800
G1 E0.3295
G1 F87.5
G1 X90.79 Y95.79 E0.3295
G1 X90.79 Y94.21 E0.3295
G1 X90.6 Y94.21 E0.3295
G1 F210
G1 X90.6 Y94.21 Z2.25 E0.3295
; END_LAYER_OBJECT z=1.80
; Reset extruder pos
G92 E0
; BEGIN_LAYER_OBJECT z=2.00
; *** Cooling...speed multiplier is 0.034722 ***
; fan on
M106
;
; 'Loop', 10.0 [RPM], 5.0 [mm/s]
G1 F5400
G1 X90.38 Y95.38 E0
G1 F210
G1 X90.38 Y95.38 Z2 E0
G1 F1800
G1 E10
G1 F300.7
G1 X89.62 Y95.38 E10.0254
G1 X89.62 Y94.62 E10.0509
G1 X90.38 Y94.62 E10.0763
G1 X90.38 Y95.38 E10.1018
;
; 'Perimeter', 10.0 [RPM], 5.0 [mm/s]
G1 F187.5
G1 X90.64 Y95.54 E10.1018
G1 F300.7
G1 X90.79 Y95.79 E10.1111
G1 X89.21 Y95.79 E10.1634
G1 X89.21 Y94.21 E10.2156
G1 X90.79 Y94.21 E10.2679
G1 X90.79 Y95.79 E10.3202
G1 X90.54 Y95.64 E10.3295
; fan off
M107
;
; 'Wipe', 0.0 [RPM], 1.5 [mm/s]
G1 F187.5
G1 X90.54 Y95.64 E10.3295
G1 F1800
G1 E0.3295
G1 F87.5
G1 X90.79 Y95.79 E0.3295
G1 X90.79 Y94.21 E0.3295
G1 X90.6 Y94.21 E0.3295
G1 F210
G1 X90.6 Y94.21 Z2.65 E0.3295
; END_LAYER_OBJECT z=2.00
M104 S0
;
; Estimated Build Time:   2.91 minutes
; Estimated Build Volume: 0.008 cm^3
; Estimated Build Cost:   $0.00
;
; *** Extrusion Time Breakdown ***
; * estimated time in [s]
; * before possibly slowing down for 'cool'
; * not including Z-travel
;	+-------------+-------------+-------------+-----------------------+
;	| Extruder #1 | Extruder #2 | Extruder #3 | Path Type             |
;	+-------------+-------------+-------------+-----------------------+
;	| 0.0342112   | 0           | 0           | Move                  |
;	| 0           | 0           | 0           | Pillar                |
;	| 0           | 0           | 0           | Raft                  |
;	| 0           > 0           > 0           > Support Interface     |
;	| 0           | 0           | 0           | Support Base          |
;	| 2.6346      | 0           | 0           | Perimeter             |
;	| 1.01311     | 0           | 0           | Loop                  |
;	| 0           > 0           > 0           > Solid                 |
;	| 0           | 0           | 0           | Sparse Infill         |
;	| 0           | 0           | 0           | Stacked Sparse Infill |
;	| 0.641169    | 0           | 0           | Wipe                  |
;	| 0           > 0           > 0           > Crown                 |
;	| 0           | 0           | 0           | Prime Pillar          |
;	| 0           | 0           | 0           | Skirt                 |
;	| 72.6        | 0           | 0           | Extruder Warm-Up      |
;	+-------------+-------------+-------------+-----------------------+
; Total estimated (pre-cool) minutes: 1.28
