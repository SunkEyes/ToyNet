#pragma once

#include <vector>
#include <map>
#include <string>
#include <stdio.h>

bool split_config(const char* file, std::map<std::string, std::string>& kv);