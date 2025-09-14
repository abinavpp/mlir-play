#include "Play.h"
#include "Play/PlayDialect.cpp.inc"
#include "Play/PlayEnums.cpp.inc"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/OpImplementation.h"
#include "llvm/ADT/TypeSwitch.h"

using namespace mlir;
using namespace mlir::play;

void PlayDialect::initialize() {
  addTypes<
#define GET_TYPEDEF_LIST
#include "Play/PlayTypes.cpp.inc"
      >();

  addOperations<
#define GET_OP_LIST
#include "Play/Play.cpp.inc"
      >();

  addAttributes<
#define GET_ATTRDEF_LIST
#include "Play/PlayAttributes.cpp.inc"
      >();
}

#define GET_ATTRDEF_CLASSES
#include "Play/PlayAttributes.cpp.inc"

#define GET_TYPEDEF_CLASSES
#include "Play/PlayTypes.cpp.inc"

#define GET_OP_CLASSES
#include "Play/Play.cpp.inc"
