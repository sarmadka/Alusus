/**
 * @file Spp/CodeGen/code_gen.h
 * Contains the definitions and include statements of all types in the
 * CodeGen namespace.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_CODEGEN_CODEGEN_H
#define SPP_CODEGEN_CODEGEN_H

namespace Spp::CodeGen
{

/**
 * @defgroup spp_codegen Code Generation
 * @ingroup spp
 * @brief Classes for code generation.
 */


//==============================================================================
// Types

struct GenResult
{
  TiObject *astNode = 0;
  Ast::Type *astType = 0;
  TioSharedPtr targetData;
};

s_enum(TerminalStatement, UNKNOWN, NO, YES);


//==============================================================================
// Global Constants

constexpr Char const* META_EXTRA_CODE_GEN = S("codeGen");
constexpr Char const* META_EXTRA_CODE_GEN_FAILED = S("codeGenFailed");


//==============================================================================
// Global Functions

// tryGetCodeGenData

template <class DT, class OT,
          typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline DT* tryGetCodeGenData(OT *object)
{
  return object->getExtra(META_EXTRA_CODE_GEN).template ti_cast_get<DT>();
}

template <class DT, class OT,
          typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline DT* tryGetCodeGenData(OT *object)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) return 0;
  return metadata->getExtra(META_EXTRA_CODE_GEN).template ti_cast_get<DT>();
}

// getCodeGenData

template <class DT, class OT>
inline DT* getCodeGenData(OT *object)
{
  auto result = tryGetCodeGenData<DT>(object);
  if (result == 0) {
    throw EXCEPTION(GenericException, S("Object is missing the generated data."));
  }
  return result;
}

// setCodeGenData

template <class DT, class OT,
          typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void setCodeGenData(OT *object, SharedPtr<DT> const &data)
{
  object->setExtra(META_EXTRA_CODE_GEN, data);
}

template <class DT, class OT,
          typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void setCodeGenData(OT *object, SharedPtr<DT> const &data)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) {
    throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
  }
  metadata->setExtra(META_EXTRA_CODE_GEN, data);
}

// removeCodeGenData

template <class OT,
          typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void removeCodeGenData(OT *object)
{
  object->removeExtra(META_EXTRA_CODE_GEN);
}

template <class OT,
          typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void removeCodeGenData(OT *object)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) {
    throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
  }
  metadata->removeExtra(META_EXTRA_CODE_GEN);
}

// didCodeGenFail

template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline Bool didCodeGenFail(OT *object)
{
  auto f = object->getExtra(META_EXTRA_CODE_GEN_FAILED).template ti_cast_get<TiBool>();
  return f && f->get();
}

template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline Bool didCodeGenFail(OT *object)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) return false;
  auto f = metadata->getExtra(META_EXTRA_CODE_GEN_FAILED).template ti_cast_get<TiBool>();
  return f && f->get();
}

// setCodeGenFailed

template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void setCodeGenFailed(OT *object, Bool f)
{
  object->setExtra(META_EXTRA_CODE_GEN_FAILED, TiBool::create(f));
}

template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void setCodeGenFailed(OT *object, Bool f)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) {
    throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
  }
  metadata->setExtra(META_EXTRA_CODE_GEN_FAILED, TiBool::create(f));
}

// resetCodeGenFailed

template <class OT, typename std::enable_if<std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void resetCodeGenFailed(OT *object)
{
  object->removeExtra(META_EXTRA_CODE_GEN_FAILED);
}

template <class OT, typename std::enable_if<!std::is_base_of<Core::Data::Ast::MetaHaving, OT>::value, int>::type = 0>
inline void resetCodeGenFailed(OT *object)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(object);
  if (metadata == 0) {
    throw EXCEPTION(InvalidArgumentException, S("object"), S("Object does not implement the MetaHaving interface."));
  }
  metadata->removeExtra(META_EXTRA_CODE_GEN_FAILED);
}

} // namespace


//==============================================================================
// Type Names

DEFINE_TYPE_NAME(Spp::CodeGen::GenResult, "alusus.org/Spp/Spp.CodeGen.GenResult");
DEFINE_TYPE_NAME(Spp::CodeGen::TerminalStatement, "alusus.org/Spp/Spp.CodeGen.TerminalStatement");


//==============================================================================
// Classes

namespace Spp::CodeGen
{

class Generator;

}

// Data
#include "IfTgContext.h"
#include "LoopTgContext.h"
#include "GlobalItemRepo.h"

// Interfaces
#include "TargetGeneration.h"
#include "Generation.h"

// Preprocessing
#include "MacroProcessor.h"

// The Generator
#include "TypeGenerator.h"
#include "ExpressionGenerator.h"
#include "CommandGenerator.h"
#include "Generator.h"

#endif
