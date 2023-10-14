#pragma once
#include <mysql/mysql.h>
#include <c-framework-service/database/config/Config.h>
#include <stdio.h>
#include "../model/Cat.h"
#include <c-framework-service/logger/Logger.h>
#include <c-framework-service/error/Error.h>

Cat * find_cat(int id);
Cat * persist_cat(int age, char* name);