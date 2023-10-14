#pragma once

#include <mysql/mysql.h>
#include <stdio.h>

#include <c-framework-service/database/config/config.h>
#include <c-framework-service/error/error.h>
#include <c-framework-service/logger/logger.h>

#include "../model/cat.h"

Cat * find_cat(int id);
Cat * persist_cat(int age, char* name);