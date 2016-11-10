//
// Created by andi on 11/10/16.
//

#include "util.h"




static int getValueByKeyInt(const std::string key, YAML::const_iterator start, YAML::const_iterator end, int default_value) {
    return getValueByKey<int>(key, start, end, default_value);
}
static float getValueByKeyFloat(const std::string key, YAML::const_iterator start, YAML::const_iterator end, float default_value) {
    return getValueByKey<float>(key, start, end, default_value);
}

