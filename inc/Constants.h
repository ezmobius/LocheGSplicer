/*
 * LocheGSplicer
 * Copyright (C) 2012 Jeff P. Houde (Lochemage)
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QColor>
#include <QString>

#include <vector>


class GCodeObject;


const static QString COMPANY_NAME = "Lochemage";
const static QString APPLICATION_NAME = "LocheGSplicer";

/**
 * Settings Key names.
 */
const static QString LAST_IMPORT_FOLDER = "LastImportFolder";
const static QString LAST_EXPORT_FOLDER = "LastExportFolder";

/**
 * Some commonly used indexed type definitions.
 */
const static int X = 0;
const static int Y = 1;
const static int Z = 2;
const static int E = 3;
const static int AXIS_NUM = 4;
const static int AXIS_NUM_NO_E = 3;

const static char AXIS_NAME[AXIS_NUM] = {'X', 'Y', 'Z', 'E'};

/**
 * Various conversion multipliers.
 */
const static double INCHES_TO_MM = 25.4;

/**
 * GCode G and M Type definitions.
 */
enum GCodeType
{
   GCODE_COMMENT                          = -1,    // Comment only.

   // Core G Codes
   GCODE_EXTRUDER_MOVEMENT0               = 1000,  // G0
   GCODE_EXTRUDER_MOVEMENT1               = 1001,  // G1
   GCODE_CW_ARC                           = 1002,  // G2
   GCODE_CCW_ARC                          = 1003,  // G3
   GCODE_DWELL                            = 1004,  // G4
   GCODE_HOME                             = 1028,  // G28
   GCODE_ABSOLUTE_COORDS                  = 1090,  // G90
   GCODE_RELATIVE_COORDS                  = 1091,  // G91
   GCODE_CURRENT_POSITION                 = 1092,  // G92

   // Non-Marlin G Codes
   GCODE_INCHES_MODE                      = 1020,  // G20
   GCODE_MILLIMETERS_MODE                 = 1021,  // G21

   // Reprap M Codes
   MCODE_SET_EXTRUDER_TEMP                = 2104,  // M104
   MCODE_GET_EXTRUDER_TEMP                = 2105,  // M105
   MCODE_FAN_ENABLE                       = 2106,  // M106
   MCODE_FAN_DISABLE                      = 2107,  // M107
   MCODE_SET_EXTRUDER_TEMP_WAIT           = 2109,  // M109
   MCODE_GET_POSITION                     = 2114,  // M114

   // Marlin custom codes.
   MCODE_ENABLE_STEPPERS                  = 2017,  // M17
   MCODE_DISABLE_STEPPERS                 = 2018,  // M18
   MCODE_LIST_SD_CARD                     = 2020,  // M20
   MCODE_INIT_SD_CARD                     = 2021,  // M21
   MCODE_RELEASE_SD_CARD                  = 2022,  // M22
   MCODE_SELECT_SD_FILE                   = 2023,  // M23
   MCODE_RESUME_SD_PRINT                  = 2024,  // M24
   MCODE_PAUSE_SD_PRINT                   = 2025,  // M25
   MCODE_SET_SD_POSITION                  = 2026,  // M26
   MCODE_SD_PRINT_STATUS                  = 2027,  // M27
   MCODE_START_SD_WRITE                   = 2028,  // M28
   MCODE_STOP_SD_WRITE                    = 2029,  // M29
   MCODE_DELETE_SD_FILE                   = 2030,  // M30
   MCODE_TIME_SINCE_LAST_TEMP_OR_SD_START = 2031,  // M31
   MCODE_PIN_CHANGE                       = 2042,  // M42
   MCODE_PSU_ON                           = 2080,  // M80
   MCODE_PSU_OFF                          = 2081,  // M81
   MCODE_E_ABSOLUTE_COORDS                = 2082,  // M82
   MCODE_E_RELATIVE_COORDS                = 2083,  // M83
   MCODE_DISABLE_STEPPERS_TILL_NEXT_MOVE  = 2084,  // M84
   MCODE_INACTIVITY_SHUTDOWN_TIME         = 2085,  // M85
   MCODE_SET_AXIS_STEPS                   = 2092,  // M92
   MCODE_OUTPUT_POSITION                  = 2114,  // M114
   MCODE_OUTPUT_CAPABILITIES              = 2115,  // M115
   MCODE_DISPLAY_MESSAGE                  = 2117,  // M117
   MCODE_OUTPUT_ENDSTOP_STATUS            = 2119,  // M119
   MCODE_SET_BED_TEMP                     = 2140,  // M140
   MCODE_SET_BED_TEMP_WAIT                = 2190,  // M190
   MCODE_SET_FILAMENT_DIAMETER            = 2200,  // M200
   MCODE_SET_MAX_ACCEL_PRINT              = 2201,  // M201
   MCODE_SET_MAX_ACCEL_TRAVEL             = 2202,  // M202
   MCODE_SET_MAX_FEEDRATE                 = 2203,  // M203
   MCODE_SET_DEFAULT_ACCEL                = 2204,  // M204
   MCODE_SET_MIN_TRAVEL                   = 2205,  // M205
   MCODE_SET_HOMING_OFFSET                = 2206,  // M206
   MCODE_SET_SPEED_FACTOR                 = 2220,  // M220
   MCODE_SET_EXTRUDE_FACTOR               = 2221,  // M221
   MCODE_TRIGGER_CAMERA                   = 2240,  // M240
   MCODE_SET_PID                          = 2301,  // M301
   MCODE_ALLOW_COLD_EXTRUDES              = 2302,  // M302
   MCODE_PID_RELAY_AUTOTUNE               = 2303,  // M303
   MCODE_FINISH_ALL_MOVES                 = 2400,  // M400
   MCODE_STORE_PARAMS_EEPROM              = 2500,  // M500
   MCODE_READ_PARAMS_EEPROM               = 2501,  // M501
   MCODE_RESET_PARAMS_EEPROM              = 2502,  // M502
   MCODE_PRINT_PARAMS_EEPROM              = 2503,  // M503
   MCODE_RESTART                          = 2999,  // M999

   MCODE_EMERGENCY_STOP                   = 2000,  // M0
};

enum DrawQuality
{
   DRAW_QUALITY_LOW,
   DRAW_QUALITY_MED,
   DRAW_QUALITY_HIGH,
};

enum PointType
{
   POINT_FIRST_TOP_LEFT,
   POINT_FIRST_TOP_RIGHT,
   POINT_FIRST_BOT_LEFT,
   POINT_FIRST_BOT_RIGHT,

   POINT_SECOND_TOP_LEFT,
   POINT_SECOND_TOP_RIGHT,
   POINT_SECOND_BOT_LEFT,
   POINT_SECOND_BOT_RIGHT,
};

/**
 * Structural data for each G code command.
 */
struct GCodeCommand
{
   GCodeCommand()
   {
      clear();
   }

   void clear()
   {
      type = -1;

      hasAxis = false;

      for (int index = 0; index < AXIS_NUM; ++index)
      {
         axisValue[index] = 0.0;
      }

      hasF = false;
      hasS = false;
      hasP = false;

      f = 0.0;
      s = 0.0;
      p = 0.0;
   }

   int type;

   QString command;
   QString comment;

   bool hasAxis;
   double axisValue[AXIS_NUM];

   bool hasF;
   bool hasS;
   bool hasP;

   double f;
   double s;
   double p;
};

struct ExtruderData
{
   ExtruderData()
   {
      for (int axis = 0; axis < AXIS_NUM_NO_E; ++axis)
      {
         offset[axis] = 0.0;
      }

      flow = 1.0;
      color = Qt::white;
   }

   ExtruderData(QColor col, double xOffset = 0.0, double yOffset = 0.0, double zOffset = 0.0, double flowRate = 1.0)
   {
      offset[X] = xOffset;
      offset[Y] = yOffset;
      offset[Z] = zOffset;

      flow = flowRate;
      color = col;
   }

   double offset[AXIS_NUM_NO_E];
   double flow;
   QColor color;
};

struct LayerData
{
   double height;
   std::vector<GCodeCommand> codes;
};

struct VisualizerBufferData
{
   VisualizerBufferData()
   {
      vertexBuffer = NULL;
      normalBuffer = NULL;
      indexBuffer = NULL;
      vertexCount = 0;
      quadCount = 0;
      height = 0.0;
   }

   void free()
   {
      if (vertexBuffer)
      {
         delete [] vertexBuffer;
         vertexBuffer = NULL;
      }

      if (normalBuffer)
      {
         delete [] normalBuffer;
         normalBuffer = NULL;
      }

      if (indexBuffer)
      {
         delete [] indexBuffer;
         indexBuffer = NULL;
      }

      vertexCount = 0;
      quadCount = 0;
      height = 0.0;
   }

   double*        vertexBuffer;
   double*        normalBuffer;
   unsigned int*  indexBuffer;
   int            vertexCount;
   unsigned int   quadCount;
   double         height;
};

struct VisualizerObjectData
{
   GCodeObject*   object;

   std::vector<VisualizerBufferData> layers;
};

struct PreferenceData
{
   PreferenceData()
   {
      // Editor Properties
      backgroundColor = Qt::darkGray;
      drawQuality = DRAW_QUALITY_MED;
      layerSkipSize = 0;

      // Splicing Properties
      customPrefixCode.clear();
      exportComments = true;
      exportDuplicateAxisPositions = false;
      printSkirt = true;
      skirtDistance = 2;

      // Printer properties.
      extruderList.push_back(ExtruderData(Qt::green));
      extruderList.push_back(ExtruderData(Qt::blue, 3.0));
      //extruderList.push_back(ExtruderData(Qt::blue, 23.5));
      platformWidth = 200;
      platformHeight = 200;

      // Advanced properties.
      exportAbsoluteMode = true;
      exportAbsoluteEMode = true;

      retraction = -1.0;
      primer = -1.0;
   }

   // Editor properties.
   QColor backgroundColor;
   DrawQuality drawQuality;
   int layerSkipSize;

   // Splicing properties.
   QString customPrefixCode;
   bool exportComments;
   bool exportDuplicateAxisPositions;
   bool printSkirt;
   int skirtDistance;

   // Printer properties.
   std::vector<ExtruderData> extruderList;
   int platformWidth;
   int platformHeight;

   // Advanced properties.
   bool exportAbsoluteMode;
   bool exportAbsoluteEMode;
   double retraction;
   double primer;
};

#endif  // CONSTANTS_H
