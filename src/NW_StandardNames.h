// GENERATED from NW-Device-Specification/standard-names.csv. Do not edit.
// Regenerate with: python3 generate_names.py > this file
//
// Each NW_HDR_* is a complete CSV header cell: the CSDMS standard name
// followed by its UCUM unit in square brackets, as DATA-FORMAT.md specifies.

#ifndef NW_STANDARD_NAMES_H
#define NW_STANDARD_NAMES_H

// Time - Instant of the reading in UTC. The unit names a format rather than a unit.
#define NW_NAME_TIME "time"
#define NW_UNIT_TIME "ISO8601"
#define NW_HDR_TIME  "time [ISO8601]"

// Air temperature - Temperature of air outside any enclosure.
#define NW_NAME_WEATHER_SENSOR_HAAR_AIR__TEMPERATURE "weather-sensor~haar_air__temperature"
#define NW_UNIT_WEATHER_SENSOR_HAAR_AIR__TEMPERATURE "Cel"
#define NW_HDR_WEATHER_SENSOR_HAAR_AIR__TEMPERATURE  "weather-sensor~haar_air__temperature [Cel]"
#define NW_HDR_MEAN_OF_WEATHER_SENSOR_HAAR_AIR__TEMPERATURE  "weather-sensor~haar_air__mean_of_temperature [Cel]"
#define NW_HDR_STD_OF_WEATHER_SENSOR_HAAR_AIR__TEMPERATURE  "weather-sensor~haar_air__standard_deviation_of_temperature [Cel]"
#define NW_HDR_STERR_OF_WEATHER_SENSOR_HAAR_AIR__TEMPERATURE  "weather-sensor~haar_air__standard_error_of_temperature [Cel]"
#define NW_HDR_MEDIAN_OF_WEATHER_SENSOR_HAAR_AIR__TEMPERATURE  "weather-sensor~haar_air__median_of_temperature [Cel]"

// Relative humidity - Relative humidity of air outside any enclosure.
#define NW_NAME_WEATHER_SENSOR_HAAR_AIR__RELATIVE_HUMIDITY "weather-sensor~haar_air__relative_humidity"
#define NW_UNIT_WEATHER_SENSOR_HAAR_AIR__RELATIVE_HUMIDITY "%"
#define NW_HDR_WEATHER_SENSOR_HAAR_AIR__RELATIVE_HUMIDITY  "weather-sensor~haar_air__relative_humidity [%]"
#define NW_HDR_MEAN_OF_WEATHER_SENSOR_HAAR_AIR__RELATIVE_HUMIDITY  "weather-sensor~haar_air__mean_of_relative_humidity [%]"
#define NW_HDR_STD_OF_WEATHER_SENSOR_HAAR_AIR__RELATIVE_HUMIDITY  "weather-sensor~haar_air__standard_deviation_of_relative_humidity [%]"
#define NW_HDR_STERR_OF_WEATHER_SENSOR_HAAR_AIR__RELATIVE_HUMIDITY  "weather-sensor~haar_air__standard_error_of_relative_humidity [%]"
#define NW_HDR_MEDIAN_OF_WEATHER_SENSOR_HAAR_AIR__RELATIVE_HUMIDITY  "weather-sensor~haar_air__median_of_relative_humidity [%]"

// Air pressure - Near-surface barometric pressure.
#define NW_NAME_WEATHER_SENSOR_HAAR_AIR__PRESSURE "weather-sensor~haar_air__pressure"
#define NW_UNIT_WEATHER_SENSOR_HAAR_AIR__PRESSURE "mbar"
#define NW_HDR_WEATHER_SENSOR_HAAR_AIR__PRESSURE  "weather-sensor~haar_air__pressure [mbar]"
#define NW_HDR_MEAN_OF_WEATHER_SENSOR_HAAR_AIR__PRESSURE  "weather-sensor~haar_air__mean_of_pressure [mbar]"
#define NW_HDR_STD_OF_WEATHER_SENSOR_HAAR_AIR__PRESSURE  "weather-sensor~haar_air__standard_deviation_of_pressure [mbar]"
#define NW_HDR_STERR_OF_WEATHER_SENSOR_HAAR_AIR__PRESSURE  "weather-sensor~haar_air__standard_error_of_pressure [mbar]"
#define NW_HDR_MEDIAN_OF_WEATHER_SENSOR_HAAR_AIR__PRESSURE  "weather-sensor~haar_air__median_of_pressure [mbar]"

// Water pressure - Absolute pressure at the sensor when submerged.
#define NW_NAME_SUBMERSIBLE_SENSOR_WALRUS_WATER__PRESSURE "submersible-sensor~walrus_water__pressure"
#define NW_UNIT_SUBMERSIBLE_SENSOR_WALRUS_WATER__PRESSURE "ubar"
#define NW_HDR_SUBMERSIBLE_SENSOR_WALRUS_WATER__PRESSURE  "submersible-sensor~walrus_water__pressure [ubar]"
#define NW_HDR_MEAN_OF_SUBMERSIBLE_SENSOR_WALRUS_WATER__PRESSURE  "submersible-sensor~walrus_water__mean_of_pressure [ubar]"
#define NW_HDR_STD_OF_SUBMERSIBLE_SENSOR_WALRUS_WATER__PRESSURE  "submersible-sensor~walrus_water__standard_deviation_of_pressure [ubar]"
#define NW_HDR_STERR_OF_SUBMERSIBLE_SENSOR_WALRUS_WATER__PRESSURE  "submersible-sensor~walrus_water__standard_error_of_pressure [ubar]"
#define NW_HDR_MEDIAN_OF_SUBMERSIBLE_SENSOR_WALRUS_WATER__PRESSURE  "submersible-sensor~walrus_water__median_of_pressure [ubar]"

// Medium temperature - Temperature at the exposed thermometer. The medium is whatever the unit is installed in; the device cannot know it.
#define NW_NAME_SUBMERSIBLE_SENSOR_WALRUS_THERMOMETER__TEMPERATURE "submersible-sensor~walrus_thermometer__temperature"
#define NW_UNIT_SUBMERSIBLE_SENSOR_WALRUS_THERMOMETER__TEMPERATURE "Cel"
#define NW_HDR_SUBMERSIBLE_SENSOR_WALRUS_THERMOMETER__TEMPERATURE  "submersible-sensor~walrus_thermometer__temperature [Cel]"
#define NW_HDR_MEAN_OF_SUBMERSIBLE_SENSOR_WALRUS_THERMOMETER__TEMPERATURE  "submersible-sensor~walrus_thermometer__mean_of_temperature [Cel]"
#define NW_HDR_STD_OF_SUBMERSIBLE_SENSOR_WALRUS_THERMOMETER__TEMPERATURE  "submersible-sensor~walrus_thermometer__standard_deviation_of_temperature [Cel]"
#define NW_HDR_STERR_OF_SUBMERSIBLE_SENSOR_WALRUS_THERMOMETER__TEMPERATURE  "submersible-sensor~walrus_thermometer__standard_error_of_temperature [Cel]"
#define NW_HDR_MEDIAN_OF_SUBMERSIBLE_SENSOR_WALRUS_THERMOMETER__TEMPERATURE  "submersible-sensor~walrus_thermometer__median_of_temperature [Cel]"

// Distance to target - Line-of-sight distance to the first surface the rangefinder detects.
#define NW_NAME_RANGEFINDER_APIS__DISTANCE "rangefinder~apis__distance"
#define NW_UNIT_RANGEFINDER_APIS__DISTANCE "cm"
#define NW_HDR_RANGEFINDER_APIS__DISTANCE  "rangefinder~apis__distance [cm]"
#define NW_HDR_MEAN_OF_RANGEFINDER_APIS__DISTANCE  "rangefinder~apis__mean_of_distance [cm]"
#define NW_HDR_STD_OF_RANGEFINDER_APIS__DISTANCE  "rangefinder~apis__standard_deviation_of_distance [cm]"
#define NW_HDR_STERR_OF_RANGEFINDER_APIS__DISTANCE  "rangefinder~apis__standard_error_of_distance [cm]"
#define NW_HDR_MEDIAN_OF_RANGEFINDER_APIS__DISTANCE  "rangefinder~apis__median_of_distance [cm]"

// Signal strength - Strength of the returned signal as the rangefinder reports it.
#define NW_NAME_RANGEFINDER_APIS__SIGNAL_STRENGTH "rangefinder~apis__signal_strength"
#define NW_UNIT_RANGEFINDER_APIS__SIGNAL_STRENGTH "1"
#define NW_HDR_RANGEFINDER_APIS__SIGNAL_STRENGTH  "rangefinder~apis__signal_strength [1]"

// Acceleration X - Acceleration along the accelerometer X axis.
#define NW_NAME_RANGEFINDER_APIS_ACCELEROMETER__X_COMPONENT_OF_ACCELERATION "rangefinder~apis_accelerometer__x_component_of_acceleration"
#define NW_UNIT_RANGEFINDER_APIS_ACCELEROMETER__X_COMPONENT_OF_ACCELERATION "m/s2"
#define NW_HDR_RANGEFINDER_APIS_ACCELEROMETER__X_COMPONENT_OF_ACCELERATION  "rangefinder~apis_accelerometer__x_component_of_acceleration [m/s2]"

// Acceleration Y - Acceleration along the accelerometer Y axis.
#define NW_NAME_RANGEFINDER_APIS_ACCELEROMETER__Y_COMPONENT_OF_ACCELERATION "rangefinder~apis_accelerometer__y_component_of_acceleration"
#define NW_UNIT_RANGEFINDER_APIS_ACCELEROMETER__Y_COMPONENT_OF_ACCELERATION "m/s2"
#define NW_HDR_RANGEFINDER_APIS_ACCELEROMETER__Y_COMPONENT_OF_ACCELERATION  "rangefinder~apis_accelerometer__y_component_of_acceleration [m/s2]"

// Acceleration Z - Acceleration along the accelerometer Z axis.
#define NW_NAME_RANGEFINDER_APIS_ACCELEROMETER__Z_COMPONENT_OF_ACCELERATION "rangefinder~apis_accelerometer__z_component_of_acceleration"
#define NW_UNIT_RANGEFINDER_APIS_ACCELEROMETER__Z_COMPONENT_OF_ACCELERATION "m/s2"
#define NW_HDR_RANGEFINDER_APIS_ACCELEROMETER__Z_COMPONENT_OF_ACCELERATION  "rangefinder~apis_accelerometer__z_component_of_acceleration [m/s2]"

// Pitch - Pitch of the sensor housing, derived from the accelerometer axes.
#define NW_NAME_RANGEFINDER_APIS_ACCELEROMETER__PITCH_ANGLE "rangefinder~apis_accelerometer__pitch_angle"
#define NW_UNIT_RANGEFINDER_APIS_ACCELEROMETER__PITCH_ANGLE "deg"
#define NW_HDR_RANGEFINDER_APIS_ACCELEROMETER__PITCH_ANGLE  "rangefinder~apis_accelerometer__pitch_angle [deg]"
#define NW_HDR_MEAN_OF_RANGEFINDER_APIS_ACCELEROMETER__PITCH_ANGLE  "rangefinder~apis_accelerometer__mean_of_pitch_angle [deg]"
#define NW_HDR_STD_OF_RANGEFINDER_APIS_ACCELEROMETER__PITCH_ANGLE  "rangefinder~apis_accelerometer__standard_deviation_of_pitch_angle [deg]"
#define NW_HDR_STERR_OF_RANGEFINDER_APIS_ACCELEROMETER__PITCH_ANGLE  "rangefinder~apis_accelerometer__standard_error_of_pitch_angle [deg]"
#define NW_HDR_MEDIAN_OF_RANGEFINDER_APIS_ACCELEROMETER__PITCH_ANGLE  "rangefinder~apis_accelerometer__median_of_pitch_angle [deg]"

// Roll - Roll of the sensor housing, derived from the accelerometer axes.
#define NW_NAME_RANGEFINDER_APIS_ACCELEROMETER__ROLL_ANGLE "rangefinder~apis_accelerometer__roll_angle"
#define NW_UNIT_RANGEFINDER_APIS_ACCELEROMETER__ROLL_ANGLE "deg"
#define NW_HDR_RANGEFINDER_APIS_ACCELEROMETER__ROLL_ANGLE  "rangefinder~apis_accelerometer__roll_angle [deg]"
#define NW_HDR_MEAN_OF_RANGEFINDER_APIS_ACCELEROMETER__ROLL_ANGLE  "rangefinder~apis_accelerometer__mean_of_roll_angle [deg]"
#define NW_HDR_STD_OF_RANGEFINDER_APIS_ACCELEROMETER__ROLL_ANGLE  "rangefinder~apis_accelerometer__standard_deviation_of_roll_angle [deg]"
#define NW_HDR_STERR_OF_RANGEFINDER_APIS_ACCELEROMETER__ROLL_ANGLE  "rangefinder~apis_accelerometer__standard_error_of_roll_angle [deg]"
#define NW_HDR_MEDIAN_OF_RANGEFINDER_APIS_ACCELEROMETER__ROLL_ANGLE  "rangefinder~apis_accelerometer__median_of_roll_angle [deg]"

// Accelerometer temperature change - Accelerometer temperature relative to the temperature at which the zero was stored. The part measures change, not temperature: LIS3DH Table 5 specifies only its output change versus temperature, 1 digit per degree C and not guaranteed, with no offset or reference point.
#define NW_NAME_RANGEFINDER_APIS_ACCELEROMETER__ANOMALY_OF_TEMPERATURE "rangefinder~apis_accelerometer__anomaly_of_temperature"
#define NW_UNIT_RANGEFINDER_APIS_ACCELEROMETER__ANOMALY_OF_TEMPERATURE "Cel"
#define NW_HDR_RANGEFINDER_APIS_ACCELEROMETER__ANOMALY_OF_TEMPERATURE  "rangefinder~apis_accelerometer__anomaly_of_temperature [Cel]"

// Pressure sensor temperature - Die temperature of the submersible pressure sensor; used to compensate its reading.
#define NW_NAME_SUBMERSIBLE_SENSOR_WALRUS_PRESSURE_SENSOR__TEMPERATURE "submersible-sensor~walrus_pressure-sensor__temperature"
#define NW_UNIT_SUBMERSIBLE_SENSOR_WALRUS_PRESSURE_SENSOR__TEMPERATURE "Cel"
#define NW_HDR_SUBMERSIBLE_SENSOR_WALRUS_PRESSURE_SENSOR__TEMPERATURE  "submersible-sensor~walrus_pressure-sensor__temperature [Cel]"
#define NW_HDR_MEAN_OF_SUBMERSIBLE_SENSOR_WALRUS_PRESSURE_SENSOR__TEMPERATURE  "submersible-sensor~walrus_pressure-sensor__mean_of_temperature [Cel]"
#define NW_HDR_STD_OF_SUBMERSIBLE_SENSOR_WALRUS_PRESSURE_SENSOR__TEMPERATURE  "submersible-sensor~walrus_pressure-sensor__standard_deviation_of_temperature [Cel]"
#define NW_HDR_STERR_OF_SUBMERSIBLE_SENSOR_WALRUS_PRESSURE_SENSOR__TEMPERATURE  "submersible-sensor~walrus_pressure-sensor__standard_error_of_temperature [Cel]"
#define NW_HDR_MEDIAN_OF_SUBMERSIBLE_SENSOR_WALRUS_PRESSURE_SENSOR__TEMPERATURE  "submersible-sensor~walrus_pressure-sensor__median_of_temperature [Cel]"

// Barometer temperature - Die temperature of the barometric pressure sensor.
#define NW_NAME_WEATHER_SENSOR_HAAR_BAROMETER__TEMPERATURE "weather-sensor~haar_barometer__temperature"
#define NW_UNIT_WEATHER_SENSOR_HAAR_BAROMETER__TEMPERATURE "Cel"
#define NW_HDR_WEATHER_SENSOR_HAAR_BAROMETER__TEMPERATURE  "weather-sensor~haar_barometer__temperature [Cel]"
#define NW_HDR_MEAN_OF_WEATHER_SENSOR_HAAR_BAROMETER__TEMPERATURE  "weather-sensor~haar_barometer__mean_of_temperature [Cel]"
#define NW_HDR_STD_OF_WEATHER_SENSOR_HAAR_BAROMETER__TEMPERATURE  "weather-sensor~haar_barometer__standard_deviation_of_temperature [Cel]"
#define NW_HDR_STERR_OF_WEATHER_SENSOR_HAAR_BAROMETER__TEMPERATURE  "weather-sensor~haar_barometer__standard_error_of_temperature [Cel]"
#define NW_HDR_MEDIAN_OF_WEATHER_SENSOR_HAAR_BAROMETER__TEMPERATURE  "weather-sensor~haar_barometer__median_of_temperature [Cel]"

// Air temperature - Air temperature  at the logger.
#define NW_NAME_DATA_LOGGER_MARGAY_AIR__TEMPERATURE "data-logger~margay_air__temperature"
#define NW_UNIT_DATA_LOGGER_MARGAY_AIR__TEMPERATURE "Cel"
#define NW_HDR_DATA_LOGGER_MARGAY_AIR__TEMPERATURE  "data-logger~margay_air__temperature [Cel]"

// Air humidity - Relative humidity  at the logger; a rise indicates a leak.
#define NW_NAME_DATA_LOGGER_MARGAY_AIR__RELATIVE_HUMIDITY "data-logger~margay_air__relative_humidity"
#define NW_UNIT_DATA_LOGGER_MARGAY_AIR__RELATIVE_HUMIDITY "%"
#define NW_HDR_DATA_LOGGER_MARGAY_AIR__RELATIVE_HUMIDITY  "data-logger~margay_air__relative_humidity [%]"

// Air pressure - Air pressure  at the logger. Equals ambient only when vented.
#define NW_NAME_DATA_LOGGER_MARGAY_AIR__PRESSURE "data-logger~margay_air__pressure"
#define NW_UNIT_DATA_LOGGER_MARGAY_AIR__PRESSURE "mbar"
#define NW_HDR_DATA_LOGGER_MARGAY_AIR__PRESSURE  "data-logger~margay_air__pressure [mbar]"

// Board temperature - Temperature of the logger board as read by its thermistor.
#define NW_NAME_DATA_LOGGER_MARGAY_BOARD__TEMPERATURE "data-logger~margay_board__temperature"
#define NW_UNIT_DATA_LOGGER_MARGAY_BOARD__TEMPERATURE "Cel"
#define NW_HDR_DATA_LOGGER_MARGAY_BOARD__TEMPERATURE  "data-logger~margay_board__temperature [Cel]"

// Clock temperature - Die temperature of the real-time clock; its drift reference.
#define NW_NAME_DATA_LOGGER_MARGAY_CLOCK__TEMPERATURE "data-logger~margay_clock__temperature"
#define NW_UNIT_DATA_LOGGER_MARGAY_CLOCK__TEMPERATURE "Cel"
#define NW_HDR_DATA_LOGGER_MARGAY_CLOCK__TEMPERATURE  "data-logger~margay_clock__temperature [Cel]"

// Battery voltage - Terminal voltage of the primary battery.
#define NW_NAME_DATA_LOGGER_MARGAY_BATTERY__VOLTAGE "data-logger~margay_battery__voltage"
#define NW_UNIT_DATA_LOGGER_MARGAY_BATTERY__VOLTAGE "V"
#define NW_HDR_DATA_LOGGER_MARGAY_BATTERY__VOLTAGE  "data-logger~margay_battery__voltage [V]"

// Battery charge - Remaining charge as a percentage of full.
#define NW_NAME_DATA_LOGGER_MARGAY_BATTERY__CHARGE_FRACTION "data-logger~margay_battery__charge_fraction"
#define NW_UNIT_DATA_LOGGER_MARGAY_BATTERY__CHARGE_FRACTION "%"
#define NW_HDR_DATA_LOGGER_MARGAY_BATTERY__CHARGE_FRACTION  "data-logger~margay_battery__charge_fraction [%]"

// LiPo voltage - Terminal voltage of the rechargeable lithium-ion battery.
#define NW_NAME_DATA_LOGGER_OKAPI_BATTERY_LITHIUM_ION__VOLTAGE "data-logger~okapi_battery~lithium-ion__voltage"
#define NW_UNIT_DATA_LOGGER_OKAPI_BATTERY_LITHIUM_ION__VOLTAGE "V"
#define NW_HDR_DATA_LOGGER_OKAPI_BATTERY_LITHIUM_ION__VOLTAGE  "data-logger~okapi_battery~lithium-ion__voltage [V]"

// LiPo charge - Remaining charge of the lithium-ion battery as a percentage of full.
#define NW_NAME_DATA_LOGGER_OKAPI_BATTERY_LITHIUM_ION__CHARGE_FRACTION "data-logger~okapi_battery~lithium-ion__charge_fraction"
#define NW_UNIT_DATA_LOGGER_OKAPI_BATTERY_LITHIUM_ION__CHARGE_FRACTION "%"
#define NW_HDR_DATA_LOGGER_OKAPI_BATTERY_LITHIUM_ION__CHARGE_FRACTION  "data-logger~okapi_battery~lithium-ion__charge_fraction [%]"

// Backup battery voltage - Terminal voltage of the backup primary cells.
#define NW_NAME_DATA_LOGGER_OKAPI_BATTERY_BACKUP__VOLTAGE "data-logger~okapi_battery~backup__voltage"
#define NW_UNIT_DATA_LOGGER_OKAPI_BATTERY_BACKUP__VOLTAGE "V"
#define NW_HDR_DATA_LOGGER_OKAPI_BATTERY_BACKUP__VOLTAGE  "data-logger~okapi_battery~backup__voltage [V]"

// Supercapacitor voltage - Terminal voltage of the supercapacitor holding the counter alive.
#define NW_NAME_EVENT_COUNTER_TALLY_CAPACITOR_SUPER__VOLTAGE "event-counter~tally_capacitor~super__voltage"
#define NW_UNIT_EVENT_COUNTER_TALLY_CAPACITOR_SUPER__VOLTAGE "V"
#define NW_HDR_EVENT_COUNTER_TALLY_CAPACITOR_SUPER__VOLTAGE  "event-counter~tally_capacitor~super__voltage [V]"

// Event count - Cumulative count of events since power-up.
#define NW_NAME_EVENT_COUNTER_TALLY__COUNT "event-counter~tally__count"
#define NW_UNIT_EVENT_COUNTER_TALLY__COUNT "1"
#define NW_HDR_EVENT_COUNTER_TALLY__COUNT  "event-counter~tally__count [1]"

// 28 accepted of 28 names
#endif
