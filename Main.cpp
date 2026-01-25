#include "Play/Play.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Func/Transforms/FuncConversions.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "mlir/Transforms/DialectConversion.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"

using namespace mlir;

struct PlayTypeConverter : public mlir::TypeConverter {
  PlayTypeConverter() {
    addConversion([](Type ty) { return ty; });
    addConversion([](play::FatPtrType ty, SmallVectorImpl<Type> &results) {
      auto i64Ty = IntegerType::get(ty.getContext(), 64);
      results.push_back(i64Ty);
      results.push_back(i64Ty);
      return success();
    });
  }
};

struct DefOpLowering : public OpConversionPattern<play::DefOp> {
  using OpConversionPattern<play::DefOp>::OpConversionPattern;
  LogicalResult matchAndRewrite(play::DefOp op, OpAdaptor adaptor,
                                ConversionPatternRewriter &r) const override {
    auto loc = op.getLoc();
    Value base = r.create<arith::ConstantIntOp>(loc, 0, 64);
    Value offset = r.create<arith::ConstantIntOp>(loc, 1, 64);
    r.replaceOpWithMultiple(op, {{base, offset}});
    return success();
  }
};

struct UseOpLowering : public OpConversionPattern<play::UseOp> {
  using OpConversionPattern<play::UseOp>::OpConversionPattern;
  LogicalResult matchAndRewrite(play::UseOp op, OneToNOpAdaptor adaptor,
                                ConversionPatternRewriter &r) const override {
    auto loc = op.getLoc();
    ValueRange ptr = adaptor.getPtr();
    r.replaceOpWithNewOp<arith::AddIOp>(op, ptr[0], ptr[1]);
    return success();
  }
};

struct Main {
  MLIRContext *ctx;
  OpBuilder b;
  ModuleOp mod;

  Main(MLIRContext *ctx) : ctx(ctx), b(ctx) {}

  void convert() {
    PlayTypeConverter tyConv;
    ConversionTarget convTgt(*ctx);
    convTgt.markUnknownOpDynamicallyLegal([](Operation *) { return true; });
    convTgt.addDynamicallyLegalOp<func::FuncOp>([&](func::FuncOp op) {
      return tyConv.isSignatureLegal(op.getFunctionType());
    });
    convTgt.addDynamicallyLegalOp<func::ReturnOp>(
        [&](Operation *op) { return tyConv.isLegal(op); });
    convTgt.addIllegalOp<play::DefOp, play::UseOp>();
    RewritePatternSet pats(ctx);
    pats.add<DefOpLowering, UseOpLowering>(tyConv, ctx);
    populateAnyFunctionOpInterfaceTypeConversionPattern(pats, tyConv);
    populateReturnOpTypeConversionPattern(pats, tyConv);
    if (failed(applyPartialConversion(mod, convTgt, std::move(pats)))) {
      llvm_unreachable("");
    }
  }

  void build() {
    auto loc = b.getUnknownLoc();
    mod = b.create<ModuleOp>(loc);
    b.setInsertionPointToStart(mod.getBody());
    auto fn =
        b.create<func::FuncOp>(loc, "foo",
                               b.getFunctionType({play::FatPtrType::get(ctx)},
                                                 {play::FatPtrType::get(ctx)}));
    b.setInsertionPointToStart(fn.addEntryBlock());
    b.create<play::UseOp>(loc, fn.getArgument(0));
    Value ptr = b.create<play::DefOp>(loc);
    b.create<func::ReturnOp>(loc, ptr);
  }

  void dump() { mod.dump(); }

  void verify() {
    if (failed(mlir::verify(mod)))
      llvm_unreachable("");
  }
};

bool parseMlirOpts(std::vector<const char *> &argv) {
  std::vector<const char *> fooArgv{"foo"};
  for (const char *arg : argv)
    fooArgv.push_back(arg);
  return llvm::cl::ParseCommandLineOptions(fooArgv.size(), fooArgv.data(), "");
}

int main() {
  std::vector<const char *> args = {"-debug-only=dialect-conversion"};
  parseMlirOpts(args);
  MLIRContext ctx;
  ctx.getOrLoadDialect<arith::ArithDialect>();
  ctx.getOrLoadDialect<func::FuncDialect>();
  ctx.getOrLoadDialect<play::PlayDialect>();
  Main m(&ctx);
  m.build();
  m.dump();
  m.convert();
  m.dump();
  m.verify();
}
