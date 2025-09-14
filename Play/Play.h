#pragma once

#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/OpDefinition.h"

#include "Play/PlayDialect.h.inc"
#include "Play/PlayEnums.h.inc"

#define GET_ATTRDEF_CLASSES
#include "Play/PlayAttributes.h.inc"

#define GET_TYPEDEF_CLASSES
#include "Play/PlayTypes.h.inc"

#define GET_OP_CLASSES
#include "Play/Play.h.inc"
