#pragma once
#include <mysql/mysql.h>
#include "../../framework/database/config/Config.h"
#include <stdio.h>
#include "../model/Cat.h"
#include "../../framework/logger/Logger.h"
#include "../../framework/error/Error.h"

Cat * FindCat(int id);
Cat * PersistCat(int age, char* name);