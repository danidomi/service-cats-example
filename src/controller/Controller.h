#include "../../framework/Server.h"
#include "../service/Service.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../utils/Converter.h"

Response *handleGET(Request *request);
Response *handlePOST(Request *request);
