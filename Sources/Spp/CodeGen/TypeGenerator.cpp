/**
 * @file Spp/CodeGen/TypeGenerator.cpp
 * Contains the implementation of class Spp::CodeGen::TypeGenerator.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"
#include <regex>

namespace Spp { namespace CodeGen
{

//==============================================================================
// Initialization Functions

void TypeGenerator::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->generateType,
    &this->generateVoidType,
    &this->generateIntegerType,
    &this->generateFloatType,
    &this->generatePointerType,
    &this->generateArrayType,
    &this->generateUserType,
    &this->generateUserTypeMemberVars,
    &this->generateFunctionType,
    &this->generateCast,
    &this->generateDefaultValue,
    &this->generateDefaultArrayValue,
    &this->generateDefaultUserTypeValue,
    &this->getTypeAllocationSize
  });
}


void TypeGenerator::initBindings()
{
  this->generateType = &TypeGenerator::_generateType;
  this->generateVoidType = &TypeGenerator::_generateVoidType;
  this->generateIntegerType = &TypeGenerator::_generateIntegerType;
  this->generateFloatType = &TypeGenerator::_generateFloatType;
  this->generatePointerType = &TypeGenerator::_generatePointerType;
  this->generateArrayType = &TypeGenerator::_generateArrayType;
  this->generateUserType = &TypeGenerator::_generateUserType;
  this->generateUserTypeMemberVars = &TypeGenerator::_generateUserTypeMemberVars;
  this->generateFunctionType = &TypeGenerator::_generateFunctionType;
  this->generateCast = &TypeGenerator::_generateCast;
  this->generateDefaultValue = &TypeGenerator::_generateDefaultValue;
  this->generateDefaultArrayValue = &TypeGenerator::_generateDefaultArrayValue;
  this->generateDefaultUserTypeValue = &TypeGenerator::_generateDefaultUserTypeValue;
  this->getTypeAllocationSize = &TypeGenerator::_getTypeAllocationSize;
}


//==============================================================================
// Main Operation Functions

Bool TypeGenerator::getGeneratedType(
  TiObject *ref, TargetGeneration *tg, TiObject *&targetTypeResult, Ast::Type **astTypeResult
) {
  Spp::Ast::Type *astType;
  if (!this->_getGeneratedType(ref, tg, astType)) return false;
  targetTypeResult = getCodeGenData<TiObject>(astType);
  if (astTypeResult != 0) {
    *astTypeResult = astType;
  }
  return true;
}


Bool TypeGenerator::getGeneratedVoidType(
  TargetGeneration *tg, TiObject *&tgTypeResult, Ast::VoidType **astTypeResult
) {
  auto voidType = this->astHelper->getVoidType();
  Ast::Type *dummy;
  if (!this->_getGeneratedType(voidType, tg, dummy)) return false;
  tgTypeResult = getCodeGenData<TiObject>(voidType);
  if (astTypeResult != 0) {
    *astTypeResult = voidType;
  }
  return true;
}


Bool TypeGenerator::_getGeneratedType(TiObject *ref, TargetGeneration *tg, Spp::Ast::Type *&type)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(ref);
  if (metadata == 0) {
    throw EXCEPTION(GenericException, S("Reference does not contain metadata."));
  }

  Bool shouldPushSl = ref->isDerivedFrom<Spp::Ast::Type>() ? false : true;

  type = this->astHelper->traceType(ref);
  if (type == 0) return false;

  Core::Data::SourceLocation *sourceLocation = 0;
  if (shouldPushSl && metadata->findSourceLocation() != 0) {
    sourceLocation = metadata->findSourceLocation().get();
    this->noticeStore->pushPrefixSourceLocation(sourceLocation);
  }
  Bool result = this->generateType(type, tg);
  if (shouldPushSl && sourceLocation != 0) {
    this->noticeStore->popPrefixSourceLocation(
      Core::Data::getSourceLocationRecordCount(sourceLocation)
    );
  }

  return result;
}


//==============================================================================
// Code Generation Functions

Bool TypeGenerator::_generateType(TiObject *self, Spp::Ast::Type *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);

  auto cgType = tryGetCodeGenData<TiObject>(astType);
  if (cgType != 0) return true;

  if (astType->isDerivedFrom<Spp::Ast::VoidType>()) {
    return typeGenerator->generateVoidType(static_cast<Spp::Ast::VoidType*>(astType), tg);
  } else if (astType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    return typeGenerator->generateIntegerType(static_cast<Spp::Ast::IntegerType*>(astType), tg);
  } else if (astType->isDerivedFrom<Spp::Ast::FloatType>()) {
    return typeGenerator->generateFloatType(static_cast<Spp::Ast::FloatType*>(astType), tg);
  } else if (astType->isDerivedFrom<Spp::Ast::PointerType>()) {
    return typeGenerator->generatePointerType(static_cast<Spp::Ast::PointerType*>(astType), tg);
  } else if (astType->isDerivedFrom<Spp::Ast::ArrayType>()) {
    return typeGenerator->generateArrayType(static_cast<Spp::Ast::ArrayType*>(astType), tg);
  } else if (astType->isDerivedFrom<Spp::Ast::UserType>()) {
    return typeGenerator->generateUserType(static_cast<Spp::Ast::UserType*>(astType), tg);
  } else if (astType->isDerivedFrom<Spp::Ast::FunctionType>()) {
    return typeGenerator->generateFunctionType(static_cast<Spp::Ast::FunctionType*>(astType), tg);
  } else {
    typeGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidTypeNotice>());
    return false;
  }
}


Bool TypeGenerator::_generateVoidType(TiObject *self, Spp::Ast::VoidType *astType, TargetGeneration *tg)
{
  TioSharedPtr tgType;
  if (!tg->generateVoidType(tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateIntegerType(TiObject *self, Spp::Ast::IntegerType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto bitCount = astType->getBitCount(typeGenerator->astHelper);
  // TODO: Support 128 bits?
  if (bitCount != 1 && bitCount != 8 && bitCount != 16 && bitCount != 32 && bitCount != 64) {
    typeGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidIntegerBitCountNotice>());
    return false;
  }
  TioSharedPtr tgType;
  if (!tg->generateIntType(bitCount, astType->isSigned(), tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateFloatType(TiObject *self, Spp::Ast::FloatType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto bitCount = astType->getBitCount(typeGenerator->astHelper);
  // TODO: Support 128 bits?
  if (bitCount != 32 && bitCount != 64) {
    typeGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidFloatBitCountNotice>());
    return false;
  }
  TioSharedPtr tgType;
  if (!tg->generateFloatType(bitCount, tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generatePointerType(TiObject *self, Spp::Ast::PointerType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->astHelper);
  if (typeGenerator->astHelper->isVoid(contentAstType)) {
    contentAstType = typeGenerator->astHelper->getCharType();
  }
  if (!typeGenerator->generateType(contentAstType, tg)) return false;
  TiObject *contentTgType = getCodeGenData<TiObject>(contentAstType);
  TioSharedPtr tgType;
  if (!tg->generatePointerType(contentTgType, tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateArrayType(TiObject *self, Spp::Ast::ArrayType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  auto contentAstType = astType->getContentType(typeGenerator->astHelper);
  if (!typeGenerator->generateType(contentAstType, tg)) return false;
  TiObject *contentTgType = getCodeGenData<TiObject>(contentAstType);
  auto size = astType->getSize(typeGenerator->astHelper);
  TioSharedPtr tgType;
  if (!tg->generateArrayType(contentTgType, size, tgType)) return false;
  setCodeGenData(astType, tgType);
  return true;
}


Bool TypeGenerator::_generateUserType(TiObject *self, Spp::Ast::UserType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  Str name = std::regex_replace(
    typeGenerator->astHelper->resolveNodePath(astType), std::regex("[^a-zA-Z0-9_]"), S("_")
  );
  TioSharedPtr tgType;
  if (!tg->generateStructTypeDecl(name.c_str(), tgType)) return false;
  setCodeGenData(astType, tgType);
  return typeGenerator->generateUserTypeMemberVars(astType, tg);
}


Bool TypeGenerator::_generateUserTypeMemberVars(TiObject *self, Spp::Ast::UserType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);

  TiObject *tgType = tryGetCodeGenData<TiObject>(astType);
  ASSERT(tgType != 0);

  // Prepare struct members.
  auto body = astType->getBody().get();
  if (body == 0) {
    throw EXCEPTION(GenericException, S("User type missing body block."));
  }
  auto prevInProgress = tryGetCodeGenData<TiBool>(body);
  if (prevInProgress != 0) {
    if (prevInProgress->get()) {
      typeGenerator->noticeStore->add(
        std::make_shared<Spp::Notices::CircularUserTypeDefinitionsNotice>(astType->findSourceLocation())
      );
      return false;
    } else {
      return true;
    }
  }
  auto inProgress = TiBool::create(true);
  setCodeGenData(body, inProgress);

  // Generate the structure.
  Bool result = true;
  PlainList<TiObject> members;
  PlainMap<TiObject> tgMemberTypes;
  SharedList<TiObject> tgMembers;
  for (Int i = 0; i < body->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(body->getElement(i));
    if (def != 0) {
      if (typeGenerator->astHelper->getDefinitionDomain(def) != Ast::DefinitionDomain::GLOBAL) {
        auto obj = def->getTarget().get();
        if (typeGenerator->astHelper->isAstReference(obj)) {
          TiObject *tgType;
          Ast::Type *astMemberType;
          if (!typeGenerator->getGeneratedType(obj, tg, tgType, &astMemberType)) {
            result = false;
            continue;
          }
          Ast::setAstType(obj, astMemberType);
          tgMemberTypes.add(def->getName().get(), tgType);
          members.add(obj);
        } else if (obj->isDerivedFrom<Core::Data::Ast::Bridge>()) {
          if (!typeGenerator->astHelper->validateUseStatement(static_cast<Core::Data::Ast::Bridge*>(obj))) {
            result = false;
          }
          continue;
        }
      }
    }
  }
  if (!result) return false;

  if (!tg->generateStructTypeBody(tgType, &tgMemberTypes, &tgMembers)) return false;
  if (tgMemberTypes.getElementCount() != tgMembers.getCount()) {
    throw EXCEPTION(GenericException, S("Unexpected error while generating struct body."));
  }
  for (Int i = 0; i < tgMemberTypes.getElementCount(); ++i) {
    setCodeGenData(members.get(i), tgMembers.get(i));
  }
  inProgress->set(false);

  return true;
}


Bool TypeGenerator::_generateFunctionType(TiObject *self, Spp::Ast::FunctionType *astType, TargetGeneration *tg)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);

  // Construct the list of argument TG types.
  auto astArgs = astType->getArgTypes().get();
  auto argCount = astArgs == 0 ? 0 : astArgs->getCount();
  PlainMap<TiObject> tgArgs;
  for (Int i = 0; i < argCount; ++i) {
    auto argType = astArgs->getElement(i);
    if (argType->isDerivedFrom<Ast::ArgPack>()) break;
    TiObject *tgType;
    Ast::Type *astType;
    if (!typeGenerator->getGeneratedType(argType, tg, tgType, &astType)) {
      return false;
    }
    tgArgs.add(astArgs->getKey(i).c_str(), tgType);
    Ast::setAstType(argType, astType);
  }

  // Get the return TG type.
  TiObject *tgRetType = 0;
  if (astType->getRetType() != 0) {
    Ast::Type *astRetType;
    if (!typeGenerator->getGeneratedType(astType->getRetType().get(), tg, tgRetType, &astRetType)) {
      return false;
    }
    Ast::setAstType(astType->getRetType().get(), astRetType);
  } else {
    if (!typeGenerator->getGeneratedVoidType(tg, tgRetType, 0)) return false;
  }

  // Generate the type.
  TioSharedPtr tgFuncType;
  if (!tg->generateFunctionType(&tgArgs, tgRetType, astType->isVariadic(), tgFuncType)) return false;
  setCodeGenData(astType, tgFuncType);
  return true;
}


Bool TypeGenerator::_generateCast(
    TiObject *self, TargetGeneration *tg, TiObject *tgContext, Spp::Ast::Type *srcType, Spp::Ast::Type *targetType,
    TiObject *tgValue, TioSharedPtr &tgCastedValue
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);
  if (srcType->isEqual(targetType, typeGenerator->astHelper, tg->getExecutionContext())) {
    // Same type, return value as is.
    tgCastedValue = getSharedPtr(tgValue);
    return true;
  } else if (srcType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    // Casting from integer.
    auto srcIntegerType = static_cast<Spp::Ast::IntegerType*>(srcType);
    TiObject *srcTgType;
    if (!typeGenerator->getGeneratedType(srcIntegerType, tg, srcTgType, 0)) return false;
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast from integer to another integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetIntegerType, tg, targetTgType, 0)) return false;
      if (!tg->generateCastIntToInt(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::FloatType>()) {
      // Cast from integer to float.
      auto targetFloatType = static_cast<Spp::Ast::FloatType*>(targetType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetFloatType, tg, targetTgType, 0)) return false;
      if (!tg->generateCastIntToFloat(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::PointerType>()) {
      // Cast from integer to pointer.
      auto targetPointerType = static_cast<Spp::Ast::PointerType*>(targetType);
      if (
        srcIntegerType->isNullLiteral() ||
        srcIntegerType->getBitCount(typeGenerator->astHelper) == tg->getExecutionContext()->getPointerBitCount()
      ) {
        TiObject *targetTgType;
        if (!typeGenerator->getGeneratedType(targetPointerType, tg, targetTgType, 0)) return false;
        if (!tg->generateCastIntToPointer(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
        return true;
      }
    }
  } else if (srcType->isDerivedFrom<Spp::Ast::FloatType>()) {
    // Casting from float.
    auto srcFloatType = static_cast<Spp::Ast::FloatType*>(srcType);
    TiObject *srcTgType;
    if (!typeGenerator->getGeneratedType(srcFloatType, tg, srcTgType, 0)) return false;
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast from float to integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetIntegerType, tg, targetTgType, 0)) return false;
      if (!tg->generateCastFloatToInt(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
      return true;
    } else if (targetType->isDerivedFrom<Spp::Ast::FloatType>()) {
      // Cast from float to another float.
      auto targetFloatType = static_cast<Spp::Ast::FloatType*>(targetType);
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetFloatType, tg, targetTgType, 0)) return false;
      if (!tg->generateCastFloatToFloat(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
      return true;
    }
  } else if (srcType->isDerivedFrom<Spp::Ast::PointerType>()) {
    // Casting from pointer.
    if (targetType->isDerivedFrom<Spp::Ast::IntegerType>()) {
      // Cast pointer to integer.
      auto targetIntegerType = static_cast<Spp::Ast::IntegerType*>(targetType);
      Word targetBitCount = targetIntegerType->getBitCount(typeGenerator->astHelper);
      if (tg->getExecutionContext()->getPointerBitCount() == targetBitCount) {
        TiObject *srcTgType;
        if (!typeGenerator->getGeneratedType(srcType, tg, srcTgType, 0)) return false;
        TiObject *targetTgType;
        if (!typeGenerator->getGeneratedType(targetIntegerType, tg, targetTgType, 0)) return false;
        if (!tg->generateCastPointerToInt(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) return false;
        return true;
      }
    } else if (targetType->isDerivedFrom<Spp::Ast::PointerType>()) {
      // Cast pointer to another pointer.
      auto targetPointerType = static_cast<Spp::Ast::PointerType*>(targetType);
      TiObject *srcTgType;
      if (!typeGenerator->getGeneratedType(srcType, tg, srcTgType, 0)) return false;
      TiObject *targetTgType;
      if (!typeGenerator->getGeneratedType(targetPointerType, tg, targetTgType, 0)) return false;
      if (!tg->generateCastPointerToPointer(tgContext, srcTgType, targetTgType, tgValue, tgCastedValue)) {
        return false;
      }
      return true;
    }
  } else if (srcType->isDerivedFrom<Spp::Ast::ReferenceType>()) {
    // Casting from reference.
    auto srcReferenceType = static_cast<Spp::Ast::ReferenceType*>(srcType);
    auto srcContentType = srcReferenceType->getContentType(typeGenerator->astHelper);
    auto tgContentType = getCodeGenData<TiObject>(srcContentType);
    TioSharedPtr tgDerefVal;
    if (!tg->generateDereference(tgContext, tgContentType, tgValue, tgDerefVal)) return false;
    return typeGenerator->generateCast(
      tg, tgContext, srcContentType, targetType, tgDerefVal.get(), tgCastedValue
    );
  }

  typeGenerator->noticeStore->add(std::make_shared<Spp::Notices::InvalidCastNotice>());
  return false;
}


Bool TypeGenerator::_generateDefaultValue(
  TiObject *self, Spp::Ast::Type *astType, TargetGeneration *tg, TiObject *tgContext, TioSharedPtr &result
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  if (astType->isDerivedFrom<Spp::Ast::IntegerType>()) {
    // Generate integer 0
    auto tgType = tryGetCodeGenData<TiObject>(astType);
    if (tgType == 0) {
      if (!typeGenerator->generateType(astType, tg)) return false;
      tgType = getCodeGenData<TiObject>(astType);
    }

    auto integerType = static_cast<Spp::Ast::IntegerType*>(astType);
    auto bitCount = integerType->getBitCount(typeGenerator->astHelper);
    return tg->generateIntLiteral(tgContext, bitCount, integerType->isSigned(), 0, result);
  } else if (astType->isDerivedFrom<Spp::Ast::FloatType>()) {
    // Generate float 0
    auto tgType = tryGetCodeGenData<TiObject>(astType);
    if (tgType == 0) {
      if (!typeGenerator->generateType(astType, tg)) return false;
      tgType = getCodeGenData<TiObject>(astType);
    }

    auto floatType = static_cast<Spp::Ast::FloatType*>(astType);
    auto bitCount = floatType->getBitCount(typeGenerator->astHelper);
    return tg->generateFloatLiteral(tgContext, bitCount, (Double)0, result);
  } else if (astType->isDerivedFrom<Spp::Ast::PointerType>()) {
    // Generate pointer null
    auto tgType = tryGetCodeGenData<TiObject>(astType);
    if (tgType == 0) {
      if (!typeGenerator->generateType(astType, tg)) return false;
      tgType = getCodeGenData<TiObject>(astType);
    }

    return tg->generateNullPtrLiteral(tgContext, tgType, result);
  } else if (astType->isDerivedFrom<Spp::Ast::ArrayType>()) {
    // Generate zeroed out array.
    return typeGenerator->generateDefaultArrayValue(static_cast<Ast::ArrayType*>(astType), tg, tgContext, result);
  } else if (astType->isDerivedFrom<Spp::Ast::UserType>()) {
    // Generate zeroed out structure.
    return typeGenerator->generateDefaultUserTypeValue(static_cast<Ast::UserType*>(astType), tg, tgContext, result);
  } else {
    throw EXCEPTION(GenericException, S("Invlid type for generation of default value."));
  }
}


Bool TypeGenerator::_generateDefaultArrayValue(
  TiObject *self, Spp::Ast::ArrayType *astType, TargetGeneration *tg, TiObject *tgContext, TioSharedPtr &result
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  auto tgType = tryGetCodeGenData<TiObject>(astType);
  if (tgType == 0) {
    if (!typeGenerator->generateType(astType, tg)) return false;
    tgType = getCodeGenData<TiObject>(astType);
  }

  auto elementAstType = astType->getContentType(typeGenerator->astHelper);
  auto elementTgType = tryGetCodeGenData<TiObject>(elementAstType);
  if (elementTgType == 0) {
    if (!typeGenerator->generateType(elementAstType, tg)) return false;
    elementTgType = getCodeGenData<TiObject>(elementAstType);
  }

  TioSharedPtr elementVal;
  if (!typeGenerator->generateDefaultValue(elementAstType, tg, tgContext, elementVal)) return false;

  auto size = astType->getSize(typeGenerator->astHelper);
  SharedList<TiObject> memberVals;
  for (Word i = 0; i < size; ++i) {
    memberVals.add(elementVal);
  }

  return tg->generateArrayLiteral(tgContext, tgType, &memberVals, result);
}


Bool TypeGenerator::_generateDefaultUserTypeValue(
  TiObject *self, Spp::Ast::UserType *astType, TargetGeneration *tg, TiObject *tgContext, TioSharedPtr &result
) {
  PREPARE_SELF(typeGenerator, TypeGenerator);

  auto tgType = tryGetCodeGenData<TiObject>(astType);
  if (tgType == 0) {
    if (!typeGenerator->generateType(astType, tg)) return false;
    tgType = getCodeGenData<TiObject>(astType);
  }

  auto body = astType->getBody().get();
  SharedList<TiObject> memberVals;
  PlainMap<TiObject> memberTypes;
  for (Int i = 0; i < body->getElementCount(); ++i) {
    auto def = ti_cast<Core::Data::Ast::Definition>(body->getElement(i));
    if (def != 0 && typeGenerator->getAstHelper()->getDefinitionDomain(def) == Ast::DefinitionDomain::OBJECT) {
      auto obj = def->getTarget().get();
      if (typeGenerator->getAstHelper()->isAstReference(obj)) {
        TiObject *tgMemberType;
        Ast::Type *astMemberType;
        if (!typeGenerator->getGeneratedType(obj, tg, tgMemberType, &astMemberType)) return false;
        memberTypes.addElement(def->getName().get(), tgMemberType);
        TioSharedPtr memberVal;
        if (!typeGenerator->generateDefaultValue(astMemberType, tg, tgContext, memberVal)) return false;
        memberVals.add(memberVal);
      }
    }
  }
  return tg->generateStructLiteral(tgContext, tgType, &memberTypes, &memberVals, result);
}


Bool TypeGenerator::_getTypeAllocationSize(TiObject *self, Spp::Ast::Type *astType, TargetGeneration *tg, Word &result)
{
  PREPARE_SELF(typeGenerator, TypeGenerator);
  TiObject *tgType;
  if (!typeGenerator->getGeneratedType(astType, tg, tgType, 0)) return false;
  result = tg->getTypeAllocationSize(tgType);
  return true;
}

} } // namespace
