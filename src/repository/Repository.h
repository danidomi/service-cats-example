#pragma once
#include <mysql/mysql.h>
#include "../../framework/database/config/Config.h"
#include <stdio.h>
#include "../model/Cat.h"
#include "../../framework/logger/Logger.h"
#include "../../framework/error/Error.h"

Cat * find_cat(int id);
Cat * persist_cat(int age, char* name);