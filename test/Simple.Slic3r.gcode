; generated by Slic3r 0.8.5-dev on 2012-07-22 at 14:48:22

; ABS
; 0.2 Layer Height

; layer_height = 0.2
; perimeters = 2
; solid_layers = 3
; fill_density = .10
; perimeter_speed = 35
; infill_speed = 35
; travel_speed = 30
; scale = 1
; nozzle_diameter = 0.35
; filament_diameter = 1.75
; extrusion_multiplier = 1
; single wall width = 0.49mm

M107 ; disable fan
M104 S215 ; set temperature

M109 S215 ; wait for temperature to be reached
G90 ; use absolute coordinates
G21 ; set units to millimeters
G92 E0 ; reset extrusion distance
M82 ; use absolute distances for extrusion
G1 Z0.200 F1800.000 ; move to next layer (0)
G1 X89.680 Y39.680 ; move to first perimeter point
G1 F15000.000 E10.00000 ; compensate retraction
G1 X90.320 Y39.680 F600.000 E10.02448 ; perimeter
G1 X90.320 Y40.320 E10.04895 ; perimeter
G1 X89.680 Y40.320 E10.07343 ; perimeter
G1 X89.680 Y39.754 E10.09509 ; perimeter
G1 X89.245 Y39.245 F1800.000 ; move to first perimeter point
G1 X90.755 Y39.245 F600.000 E10.15284 ; perimeter
G1 X90.755 Y40.755 E10.21059 ; perimeter
G1 X89.245 Y40.755 E10.26834 ; perimeter
G1 X89.245 Y39.319 E10.32328 ; perimeter
G1 F15000.000 E0.32328 ; retract
G92 E0 ; reset extrusion distance
G1 Z0.400 F1800.000 ; move to next layer (1)
G1 X89.680 Y39.680 ; move to first perimeter point
G1 F15000.000 E10.00000 ; compensate retraction
G1 X90.320 Y39.680 F600.000 E10.02448 ; perimeter
G1 X90.320 Y40.320 E10.04895 ; perimeter
G1 X89.680 Y40.320 E10.07343 ; perimeter
G1 X89.680 Y39.754 E10.09509 ; perimeter
G1 X89.245 Y39.245 F1800.000 ; move to first perimeter point
G1 X90.755 Y39.245 F600.000 E10.15284 ; perimeter
G1 X90.755 Y40.755 E10.21059 ; perimeter
G1 X89.245 Y40.755 E10.26834 ; perimeter
G1 X89.245 Y39.319 E10.32328 ; perimeter
M106 S255 ; enable fan
G1 F15000.000 E0.32328 ; retract
G92 E0 ; reset extrusion distance
G1 Z0.600 F1800.000 ; move to next layer (2)
G1 X89.680 Y39.680 ; move to first perimeter point
G1 F15000.000 E10.00000 ; compensate retraction
G1 X90.320 Y39.680 F600.000 E10.02448 ; perimeter
G1 X90.320 Y40.320 E10.04895 ; perimeter
G1 X89.680 Y40.320 E10.07343 ; perimeter
G1 X89.680 Y39.754 E10.09509 ; perimeter
G1 X89.245 Y39.245 F1800.000 ; move to first perimeter point
G1 X90.755 Y39.245 F600.000 E10.15284 ; perimeter
G1 X90.755 Y40.755 E10.21059 ; perimeter
G1 X89.245 Y40.755 E10.26834 ; perimeter
G1 X89.245 Y39.319 E10.32328 ; perimeter
G1 F15000.000 E0.32328 ; retract
G92 E0 ; reset extrusion distance
G1 Z0.800 F1800.000 ; move to next layer (3)
G1 X89.680 Y39.680 ; move to first perimeter point
G1 F15000.000 E10.00000 ; compensate retraction
G1 X90.320 Y39.680 F600.000 E10.02448 ; perimeter
G1 X90.320 Y40.320 E10.04895 ; perimeter
G1 X89.680 Y40.320 E10.07343 ; perimeter
G1 X89.680 Y39.754 E10.09509 ; perimeter
G1 X89.245 Y39.245 F1800.000 ; move to first perimeter point
G1 X90.755 Y39.245 F600.000 E10.15284 ; perimeter
G1 X90.755 Y40.755 E10.21059 ; perimeter
G1 X89.245 Y40.755 E10.26834 ; perimeter
G1 X89.245 Y39.319 E10.32328 ; perimeter
G1 F15000.000 E0.32328 ; retract
G92 E0 ; reset extrusion distance
G1 Z1.000 F1800.000 ; move to next layer (4)
G1 X89.680 Y39.680 ; move to first perimeter point
G1 F15000.000 E10.00000 ; compensate retraction
G1 X90.320 Y39.680 F600.000 E10.02448 ; perimeter
G1 X90.320 Y40.320 E10.04895 ; perimeter
G1 X89.680 Y40.320 E10.07343 ; perimeter
G1 X89.680 Y39.754 E10.09509 ; perimeter
G1 X89.245 Y39.245 F1800.000 ; move to first perimeter point
G1 X90.755 Y39.245 F600.000 E10.15284 ; perimeter
G1 X90.755 Y40.755 E10.21059 ; perimeter
G1 X89.245 Y40.755 E10.26834 ; perimeter
G1 X89.245 Y39.319 E10.32328 ; perimeter
G1 F15000.000 E0.32328 ; retract
G92 E0 ; reset extrusion distance
G1 Z1.200 F1800.000 ; move to next layer (5)
G1 X89.680 Y39.680 ; move to first perimeter point
G1 F15000.000 E10.00000 ; compensate retraction
G1 X90.320 Y39.680 F600.000 E10.02448 ; perimeter
G1 X90.320 Y40.320 E10.04895 ; perimeter
G1 X89.680 Y40.320 E10.07343 ; perimeter
G1 X89.680 Y39.754 E10.09509 ; perimeter
G1 X89.245 Y39.245 F1800.000 ; move to first perimeter point
G1 X90.755 Y39.245 F600.000 E10.15284 ; perimeter
G1 X90.755 Y40.755 E10.21059 ; perimeter
G1 X89.245 Y40.755 E10.26834 ; perimeter
G1 X89.245 Y39.319 E10.32328 ; perimeter
G1 F15000.000 E0.32328 ; retract
G92 E0 ; reset extrusion distance
G1 Z1.400 F1800.000 ; move to next layer (6)
G1 X89.680 Y39.680 ; move to first perimeter point
G1 F15000.000 E10.00000 ; compensate retraction
G1 X90.320 Y39.680 F600.000 E10.02448 ; perimeter
G1 X90.320 Y40.320 E10.04895 ; perimeter
G1 X89.680 Y40.320 E10.07343 ; perimeter
G1 X89.680 Y39.754 E10.09509 ; perimeter
G1 X89.245 Y39.245 F1800.000 ; move to first perimeter point
G1 X90.755 Y39.245 F600.000 E10.15284 ; perimeter
G1 X90.755 Y40.755 E10.21059 ; perimeter
G1 X89.245 Y40.755 E10.26834 ; perimeter
G1 X89.245 Y39.319 E10.32328 ; perimeter
G1 F15000.000 E0.32328 ; retract
G92 E0 ; reset extrusion distance
G1 Z1.600 F1800.000 ; move to next layer (7)
G1 X89.680 Y39.680 ; move to first perimeter point
G1 F15000.000 E10.00000 ; compensate retraction
G1 X90.320 Y39.680 F600.000 E10.02448 ; perimeter
G1 X90.320 Y40.320 E10.04895 ; perimeter
G1 X89.680 Y40.320 E10.07343 ; perimeter
G1 X89.680 Y39.754 E10.09509 ; perimeter
G1 X89.245 Y39.245 F1800.000 ; move to first perimeter point
G1 X90.755 Y39.245 F600.000 E10.15284 ; perimeter
G1 X90.755 Y40.755 E10.21059 ; perimeter
G1 X89.245 Y40.755 E10.26834 ; perimeter
G1 X89.245 Y39.319 E10.32328 ; perimeter
G1 F15000.000 E0.32328 ; retract
G92 E0 ; reset extrusion distance
G1 Z1.800 F1800.000 ; move to next layer (8)
G1 X89.680 Y39.680 ; move to first perimeter point
G1 F15000.000 E10.00000 ; compensate retraction
G1 X90.320 Y39.680 F600.000 E10.02448 ; perimeter
G1 X90.320 Y40.320 E10.04895 ; perimeter
G1 X89.680 Y40.320 E10.07343 ; perimeter
G1 X89.680 Y39.754 E10.09509 ; perimeter
G1 X89.245 Y39.245 F1800.000 ; move to first perimeter point
G1 X90.755 Y39.245 F600.000 E10.15284 ; perimeter
G1 X90.755 Y40.755 E10.21059 ; perimeter
G1 X89.245 Y40.755 E10.26834 ; perimeter
G1 X89.245 Y39.319 E10.32328 ; perimeter
G1 F15000.000 E0.32328 ; retract
G92 E0 ; reset extrusion distance
G1 Z2.000 F1800.000 ; move to next layer (9)
G1 X89.680 Y39.680 ; move to first perimeter point
G1 F15000.000 E10.00000 ; compensate retraction
G1 X90.320 Y39.680 F600.000 E10.02448 ; perimeter
G1 X90.320 Y40.320 E10.04895 ; perimeter
G1 X89.680 Y40.320 E10.07343 ; perimeter
G1 X89.680 Y39.754 E10.09509 ; perimeter
G1 X89.245 Y39.245 F1800.000 ; move to first perimeter point
G1 X90.755 Y39.245 F600.000 E10.15284 ; perimeter
G1 X90.755 Y40.755 E10.21059 ; perimeter
G1 X89.245 Y40.755 E10.26834 ; perimeter
G1 X89.245 Y39.319 E10.32328 ; perimeter
G1 F15000.000 E0.32328 ; retract
G92 E0 ; reset extrusion distance
M107 ; disable fan

; filament used = 13.2mm (0.0cm3)
