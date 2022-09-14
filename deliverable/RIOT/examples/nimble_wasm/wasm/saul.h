
typedef void saul_reg_t;

uintptr_t saul_reg_find_type(int type);
float saul_reg_read(uintptr_t dev);

enum {
    SAUL_CAT_UNDEF        = 0x00,   /**< device class undefined */
    SAUL_CAT_ACT          = 0x40,   /**< Actuator device class */
    SAUL_CAT_SENSE        = 0x80,   /**< Sensor device class */
};

/**
 * @brief   Definition of actuator intra-category IDs
 */
enum {
    SAUL_ACT_ID_ANY,                  /**< any actuator - wildcard */
    SAUL_ACT_ID_LED_RGB,              /**< actuator: RGB LED */
    SAUL_ACT_ID_SERVO,                /**< actuator: servo motor */
    SAUL_ACT_ID_MOTOR,                /**< actuator: motor */
    SAUL_ACT_ID_SWITCH,               /**< actuator: simple on/off switch */
    SAUL_ACT_ID_DIMMER,               /**< actuator: dimmable switch */
    SAUL_ACT_NUMOF                  /**< Number of actuators supported */
    /* Extend this list as needed, but keep SAUL_ACT_ID_ANY the first and
     * SAUL_ACT_NUMOF the last entry
     */
};

/**
 * @brief   Definition of sensor intra-category IDs
 */
enum {
    SAUL_SENSE_ID_ANY = 0,          /**< any sensor - wildcard */
    SAUL_SENSE_ID_BTN,              /**< sensor: simple button */
    SAUL_SENSE_ID_TEMP,             /**< sensor: temperature */
    SAUL_SENSE_ID_HUM,              /**< sensor: humidity */
    SAUL_SENSE_ID_LIGHT,            /**< sensor: light */
    SAUL_SENSE_ID_ACCEL,            /**< sensor: accelerometer */
    SAUL_SENSE_ID_MAG,              /**< sensor: magnetometer */
    SAUL_SENSE_ID_GYRO,             /**< sensor: gyroscope */
    SAUL_SENSE_ID_COLOR,            /**< sensor: (light) color */
    SAUL_SENSE_ID_PRESS,            /**< sensor: pressure */
    SAUL_SENSE_ID_ANALOG,           /**< sensor: raw analog value */
    SAUL_SENSE_ID_UV,               /**< sensor: UV index */
    SAUL_SENSE_ID_OBJTEMP,          /**< sensor: object temperature */
    SAUL_SENSE_ID_COUNT,            /**< sensor: pulse counter */
    SAUL_SENSE_ID_DISTANCE,         /**< sensor: distance */
    SAUL_SENSE_ID_CO2,              /**< sensor: CO2 Gas */
    SAUL_SENSE_ID_TVOC,             /**< sensor: TVOC Gas */
    SAUL_SENSE_ID_GAS,              /**< sensor: Gas common */
    SAUL_SENSE_ID_OCCUP,            /**< sensor: occupancy */
    SAUL_SENSE_ID_PROXIMITY,        /**< sensor: proximity */
    SAUL_SENSE_ID_RSSI,             /**< sensor: RSSI */
    SAUL_SENSE_ID_CHARGE,           /**< sensor: coulomb counter */
    SAUL_SENSE_ID_CURRENT,          /**< sensor: ammeter */
    SAUL_SENSE_ID_PM,               /**< sensor: particulate matter */
    SAUL_SENSE_ID_CAPACITANCE,      /**< sensor: capacitance */
    SAUL_SENSE_ID_VOLTAGE,          /**< sensor: voltage */
    SAUL_SENSE_ID_PH,               /**< sensor: pH */
    SAUL_SENSE_ID_POWER,            /**< sensor: power */
    SAUL_SENSE_ID_SIZE,             /**< sensor: size */
    SAUL_SENSE_NUMOF                /**< Number of actuators supported */
    /* Extend this list as needed, but keep SAUL_SENSE_ID_ANY the first and
     * SAUL_SENSE_NUMOF the last entry
     */
};

/**
 * @brief   Definition of SAUL actuator and sensor classes
 *
 * These values consists of the SAUL category ID (two most significant bits)
 * and the SAUL intra-category ID (six least significant bits).
 */
enum {
    /** any actuator - wildcard */
    SAUL_ACT_ANY            = SAUL_CAT_ACT | SAUL_ACT_ID_ANY,
    /** actuator: RGB LED */
    SAUL_ACT_LED_RGB        = SAUL_CAT_ACT | SAUL_ACT_ID_LED_RGB,
    /** actuator: servo motor */
    SAUL_ACT_SERVO          = SAUL_CAT_ACT | SAUL_ACT_ID_SERVO,
    /** actuator: motor */
    SAUL_ACT_MOTOR          = SAUL_CAT_ACT | SAUL_ACT_ID_MOTOR,
    /** actuator: simple on/off switch */
    SAUL_ACT_SWITCH         = SAUL_CAT_ACT | SAUL_ACT_ID_SWITCH,
    /** actuator: dimmable switch */
    SAUL_ACT_DIMMER         = SAUL_CAT_ACT | SAUL_ACT_ID_DIMMER,
    /** any sensor - wildcard */
    SAUL_SENSE_ANY          = SAUL_CAT_SENSE | SAUL_SENSE_ID_ANY,
    /** sensor: simple button */
    SAUL_SENSE_BTN          = SAUL_CAT_SENSE | SAUL_SENSE_ID_BTN,
    /** sensor: temperature */
    SAUL_SENSE_TEMP         = SAUL_CAT_SENSE | SAUL_SENSE_ID_TEMP,
    /** sensor: humidity */
    SAUL_SENSE_HUM          = SAUL_CAT_SENSE | SAUL_SENSE_ID_HUM,
    /** sensor: light */
    SAUL_SENSE_LIGHT        = SAUL_CAT_SENSE | SAUL_SENSE_ID_LIGHT,
    /** sensor: accelerometer */
    SAUL_SENSE_ACCEL        = SAUL_CAT_SENSE | SAUL_SENSE_ID_ACCEL,
    /** sensor: magnetometer */
    SAUL_SENSE_MAG          = SAUL_CAT_SENSE | SAUL_SENSE_ID_MAG,
    /** sensor: gyroscope */
    SAUL_SENSE_GYRO         = SAUL_CAT_SENSE | SAUL_SENSE_ID_GYRO,
    /** sensor: (light) color */
    SAUL_SENSE_COLOR        = SAUL_CAT_SENSE | SAUL_SENSE_ID_COLOR,
    /** sensor: pressure */
    SAUL_SENSE_PRESS        = SAUL_CAT_SENSE | SAUL_SENSE_ID_PRESS,
    /** sensor: raw analog value */
    SAUL_SENSE_ANALOG       = SAUL_CAT_SENSE | SAUL_SENSE_ID_ANALOG,
    /** sensor: UV index */
    SAUL_SENSE_UV           = SAUL_CAT_SENSE | SAUL_SENSE_ID_UV,
    /** sensor: object temperature */
    SAUL_SENSE_OBJTEMP      = SAUL_CAT_SENSE | SAUL_SENSE_ID_OBJTEMP,
    /** sensor: pulse counter */
    SAUL_SENSE_COUNT        = SAUL_CAT_SENSE | SAUL_SENSE_ID_COUNT,
    /** sensor: distance */
    SAUL_SENSE_DISTANCE     = SAUL_CAT_SENSE | SAUL_SENSE_ID_DISTANCE,
    /** sensor: CO2 Gas */
    SAUL_SENSE_CO2          = SAUL_CAT_SENSE | SAUL_SENSE_ID_CO2,
    /** sensor: TVOC Gas */
    SAUL_SENSE_TVOC         = SAUL_CAT_SENSE | SAUL_SENSE_ID_TVOC,
    /** sensor: Gas common */
    SAUL_SENSE_GAS          = SAUL_CAT_SENSE | SAUL_SENSE_ID_GAS,
    /** sensor: occupancy */
    SAUL_SENSE_OCCUP        = SAUL_CAT_SENSE | SAUL_SENSE_ID_OCCUP,
    /** sensor: proximity */
    SAUL_SENSE_PROXIMITY    = SAUL_CAT_SENSE | SAUL_SENSE_ID_PROXIMITY,
    /** sensor: RSSI */
    SAUL_SENSE_RSSI         = SAUL_CAT_SENSE | SAUL_SENSE_ID_RSSI,
    /** sensor: coulomb counter */
    SAUL_SENSE_CHARGE       = SAUL_CAT_SENSE | SAUL_SENSE_ID_CHARGE,
    /** sensor: ammeter */
    SAUL_SENSE_CURRENT      = SAUL_CAT_SENSE | SAUL_SENSE_ID_CURRENT,
    /** sensor: particulate matter */
    SAUL_SENSE_PM           = SAUL_CAT_SENSE | SAUL_SENSE_ID_PM,
    /** sensor: capacitance */
    SAUL_SENSE_CAPACITANCE  = SAUL_CAT_SENSE | SAUL_SENSE_ID_CAPACITANCE,
    /** sensor: voltage */
    SAUL_SENSE_VOLTAGE      = SAUL_CAT_SENSE | SAUL_SENSE_ID_VOLTAGE,
    /** sensor: pH */
    SAUL_SENSE_PH           = SAUL_CAT_SENSE | SAUL_SENSE_ID_PH,
    /** sensor: power */
    SAUL_SENSE_POWER        = SAUL_CAT_SENSE | SAUL_SENSE_ID_POWER,
    /** sensor: size */
    SAUL_SENSE_SIZE         = SAUL_CAT_SENSE | SAUL_SENSE_ID_SIZE,
    /** any device - wildcard */
    SAUL_CLASS_ANY          = 0xff
    /* extend this list as needed... */
};

/**
 * @brief   Bitmask to retrieve the class ID and intra-category ID from a SAUL
 *          class
 */
enum {
    SAUL_CAT_MASK           = 0xc0, /**< Bitmask to obtain the category ID */
    SAUL_ID_MASK            = 0x3f, /**< Bitmask to obtain the intra-category ID */
};
/** @} */
