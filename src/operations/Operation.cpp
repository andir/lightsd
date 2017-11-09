#include "Operation.h"

const MqttVarType Operation::HSV_HUE = {"float", "0:360", "hsv_hue"};
const MqttVarType Operation::HSV_SATURATION = {"float", "0:1",
                                               "hsv_saturation"};
const MqttVarType Operation::HSV_VALUE = {"float", "0:1", "hsv_value"};

const MqttVarType Operation::BOOLEAN = {"boolean", "", ""};

const MqttVarType Operation::FLOAT_0_1 = {"float", "0:1", ""};

const MqttVarType Operation::INT = {"int", "", ""};

const MqttVarType Operation::FLOAT = {"float", "", ""};

const MqttVarType Operation::STRING = {"string", "", ""};
