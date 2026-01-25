#ifndef PLAYOPS_TD
#define PLAYOPS_TD

include "mlir/IR/EnumAttr.td"
include "mlir/IR/OpBase.td"

def Play_Dialect : Dialect {
  let name = "play";
  let cppNamespace = "::mlir::play";
  let useDefaultTypePrinterParser = 1;
}

class Play_Type<string name, string typeMnemonic, list<Trait> traits = []>
    : TypeDef<Play_Dialect, name, traits> {
  let mnemonic = typeMnemonic;
}

def Sol_FatPtrType : Play_Type<"FatPtr", "fatptr">;

class Play_Op<string mnemonic, list<Trait> traits = []>
  : Op<Play_Dialect, mnemonic, traits>;

def Play_DefOp : Play_Op<"def"> {
  let results = (outs Sol_FatPtrType:$out);

  let builders = [
    OpBuilder<(ins), [{
      $_state.addTypes(play::FatPtrType::get($_builder.getContext()));
    }]>
  ];
  let skipDefaultBuilders = 1;
}

def Play_UseOp : Play_Op<"use"> {
  let arguments = (ins Sol_FatPtrType:$ptr);
  let results   = (outs I64:$out);

  let builders = [
    OpBuilder<(ins "Value":$ptr), [{
      $_state.addOperands(ptr);
      $_state.addTypes($_builder.getI64Type());
    }]>
  ];
  let skipDefaultBuilders = 1;
}

#endif // PLAYOPS_TD
