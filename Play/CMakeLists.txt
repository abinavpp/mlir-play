add_mlir_dialect(Play play)

set(LLVM_TARGET_DEFINITIONS Play.td)

mlir_tablegen(PlayEnums.h.inc -gen-enum-decls)
mlir_tablegen(PlayEnums.cpp.inc -gen-enum-defs)
mlir_tablegen(PlayAttributes.h.inc -gen-attrdef-decls
              -attrdefs-dialect=play)
mlir_tablegen(PlayAttributes.cpp.inc -gen-attrdef-defs
              -attrdefs-dialect=play)
add_public_tablegen_target(MLIRPlayExtraIncGen)

add_mlir_dialect_library(MLIRPlayDialect Play.cpp
	DEPENDS MLIRPlayIncGen MLIRPlayExtraIncGen
)
